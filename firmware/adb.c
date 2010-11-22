#include <string.h>
#include <assert.h>

#include "adb.h"
#include "adb_packet.h"
#include "usb_config.h"
#include "USB/usb.h"
#include "USB/usb_host.h"
#include "usb_host_android.h"
#include "logging.h"


////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

// ADB protocol defines
#define ADB_CNXN 0x4e584e43
#define ADB_OPEN 0x4e45504f
#define ADB_OKAY 0x59414b4f
#define ADB_CLSE 0x45534c43
#define ADB_WRTE 0x45545257

#define ADB_VERSION 0x01000000        // ADB protocol version

////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////

typedef enum {
  ADB_CONN_STATE_ERROR,
  ADB_CONN_STATE_WAIT_ATTACH,
  ADB_CONN_STATE_WAIT_CONNECT,
  ADB_CONN_STATE_CONNECTED
} ADB_CONN_STATE;

typedef enum {
  ADB_CHAN_STATE_FREE = 0,
  ADB_CHAN_STATE_START,
  ADB_CHAN_STATE_WAIT_OPEN,
  ADB_CHAN_STATE_IDLE,
  ADB_CHAN_STATE_WAIT_READY,
  ADB_CHAN_STATE_CLOSE_REQUESTED,
} ADB_CHAN_STATE;

typedef struct {
  ADB_CHAN_STATE state;
  const void* data;
  UINT32 data_len;
  char name[ADB_CHANNEL_NAME_MAX_LENGTH];
  UINT32 remote_id;
  BOOL pending_ack;
  ADBChannelRecvFunc recv_func;
} ADB_CHANNEL;

////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////
static ADB_CONN_STATE adb_conn_state;
static ADB_CHANNEL adb_channels[ADB_MAX_CHANNELS];
static char ADB_HOSTNAME_STRING[] = "host::";  // Leave non-const. USB stack
                                               // doesn't work from ROM.

////////////////////////////////////////////////////////////////////////////////
// Functions & Macros
////////////////////////////////////////////////////////////////////////////////


static void ADBReset() {
  // close all open channels
  ADB_CHANNEL_HANDLE h;
  for (h = 0; h < ADB_MAX_CHANNELS; ++h) {
    if (adb_channels[h].state != ADB_CHAN_STATE_FREE) {
      adb_channels[h].recv_func(h, NULL, 0);
    }
  }
  memset(adb_channels, 0, sizeof adb_channels);
  ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_WAIT_ATTACH);
}

static void ADBChannelTasks() {
  static ADB_CHANNEL_HANDLE current_channel = 0;
  ADB_RESULT adb_res;
  ADB_CHANNEL_HANDLE h;
  if ((adb_res = ADBPacketSendStatus()) == ADB_RESULT_BUSY) return;
  if (adb_res == ADB_RESULT_ERROR) {
    ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_ERROR);
    return;
  }
  for (h = 0; h < ADB_MAX_CHANNELS; ++h) {
    if (++current_channel == ADB_MAX_CHANNELS) current_channel = 0;
    if (adb_channels[current_channel].state == ADB_CHAN_STATE_FREE) {
      continue;
    }
    if (adb_channels[current_channel].state == ADB_CHAN_STATE_START) {
      ADBPacketSend(ADB_OPEN, current_channel + 1, 0, adb_channels[current_channel].name, strlen(adb_channels[current_channel].name) + 1);
      ADB_CHANGE_STATE(adb_channels[current_channel].state, ADB_CHAN_STATE_WAIT_OPEN);
      return;
    }
    if (adb_channels[current_channel].state == ADB_CHAN_STATE_CLOSE_REQUESTED) {
      ADBPacketSend(ADB_CLSE, current_channel + 1, adb_channels[current_channel].remote_id, NULL, 0);
      ADB_CHANGE_STATE(adb_channels[current_channel].state, ADB_CHAN_STATE_FREE);
      return;
    }
    if (adb_channels[current_channel].pending_ack) {
      ADBPacketSend(ADB_OKAY, current_channel + 1, adb_channels[current_channel].remote_id, NULL, 0);
      adb_channels[current_channel].pending_ack = FALSE;
      return;
    }
    if (adb_channels[current_channel].state == ADB_CHAN_STATE_IDLE
        && adb_channels[current_channel].data != NULL) {
      ADBPacketSend(ADB_WRTE, current_channel + 1, adb_channels[current_channel].remote_id, adb_channels[current_channel].data, adb_channels[current_channel].data_len);
      ADB_CHANGE_STATE(adb_channels[current_channel].state, ADB_CHAN_STATE_WAIT_READY);
      return;
    }
  }
}

static void ADBHandlePacket(UINT32 cmd, UINT32 arg0, UINT32 arg1, const void* recv_data, UINT32 data_len) {
  switch(cmd) {
   case ADB_CNXN:
    print1("ADB established connection with [%s]", (const char*) recv_data);
    // TODO: arg1 contains max_data - handle
    ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_CONNECTED);
    break;

   case ADB_OPEN:
    // should not happen. ignored.
    break;

   case ADB_OKAY:
    --arg1;
    if (arg1 >= 0 && arg1 < ADB_MAX_CHANNELS) {
      if (adb_channels[arg1].state == ADB_CHAN_STATE_WAIT_OPEN) {
        print3("Channel %ld is open. Remote ID: 0x%lx. Name: %s", arg1, arg0, adb_channels[arg1].name);
        adb_channels[arg1].remote_id = arg0;
        ADB_CHANGE_STATE(adb_channels[arg1].state, ADB_CHAN_STATE_IDLE);
      } else if (adb_channels[arg1].state == ADB_CHAN_STATE_WAIT_READY
        && adb_channels[arg1].remote_id == arg0) {
        adb_channels[arg1].data = NULL;
        ADB_CHANGE_STATE(adb_channels[arg1].state, ADB_CHAN_STATE_IDLE);
      }
    } else {
      print1("Remote side sent an OK on an unexpected channel: %ld", arg1);
      ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_ERROR);
    }
    break;

   case ADB_CLSE:
    --arg1;
    if (arg1 < ADB_MAX_CHANNELS) {
      if (adb_channels[arg1].state == ADB_CHAN_STATE_WAIT_OPEN) {
        print2("Channel %ld open failed. Name: %s", arg1, adb_channels[arg1].name);
        adb_channels[arg1].recv_func(arg1, NULL, 0);
        ADB_CHANGE_STATE(adb_channels[arg1].state, ADB_CHAN_STATE_FREE);
      } else if (adb_channels[arg1].state == ADB_CHAN_STATE_WAIT_READY
        && adb_channels[arg1].remote_id == arg0) {
        print2("Channel %ld closed by remote side. Name: %s", arg1, adb_channels[arg1].name);
        adb_channels[arg1].recv_func(arg1, NULL, 0);
        ADB_CHANGE_STATE(adb_channels[arg1].state, ADB_CHAN_STATE_FREE);
      }
    } else {
      ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_ERROR);
    }
    break;

   case ADB_WRTE:
    --arg1;
    if (arg1 < ADB_MAX_CHANNELS) {
      if (adb_channels[arg1].remote_id == arg0) {
        if (data_len > 0) {
          adb_channels[arg1].recv_func(arg1, recv_data, data_len);
        }
        adb_channels[arg1].pending_ack = TRUE;
      }
    } else {
      ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_ERROR);
    }
    break;

   default:
    print1("Unknown command 0x%lx. Ignoring.", cmd);
  }
}

ADB_CHANNEL_HANDLE ADBOpen(const char* name, ADBChannelRecvFunc recv_func) {
  assert(name != NULL);
  assert(recv_func != NULL);
  // find a free channel
  ADB_CHANNEL_HANDLE h;
  for (h = 0; h < ADB_MAX_CHANNELS; ++h) {
    if (adb_channels[h].state == ADB_CHAN_STATE_FREE) {
      ADB_CHANGE_STATE(adb_channels[h].state, ADB_CHAN_STATE_START);
      strncpy(adb_channels[h].name, name, ADB_CHANNEL_NAME_MAX_LENGTH);
      adb_channels[h].pending_ack = FALSE;
      adb_channels[h].data = NULL;
      adb_channels[h].recv_func = recv_func;
      print2("Trying to open channel %d with name: %s", h, name);
      return h;
    }
  }
  return ADB_INVALID_CHANNEL_HANDLE;
}

void ADBClose(ADB_CHANNEL_HANDLE handle) {
  assert(handle >= 0 && handle < ADB_MAX_CHANNELS);
  if (adb_channels[handle].state > ADB_CHAN_STATE_FREE) {
    adb_channels[handle].state = ADB_CHAN_STATE_CLOSE_REQUESTED;
  }
}

BOOL ADBChannelReady(ADB_CHANNEL_HANDLE handle) {
  return adb_channels[handle].state == ADB_CHAN_STATE_IDLE;
}

void ADBWrite(ADB_CHANNEL_HANDLE handle, const void* data, UINT32 data_len) {
  assert(handle >= 0 && handle < ADB_MAX_CHANNELS);
  assert(adb_channels[handle].state == ADB_CHAN_STATE_IDLE);
  adb_channels[handle].data = data;
  adb_channels[handle].data_len = data_len;
}

ADB_RESULT ADBWriteStatus();
void ADBRead(ADB_CHANNEL_HANDLE handle);
ADB_RESULT ADBReadStatus(ADB_CHANNEL_HANDLE handle, void** data, UINT32* data_len);

void ADBInit() {
  BOOL res = USBHostInit(0);
  assert(res);
  memset(adb_channels, 0, sizeof adb_channels);
  ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_WAIT_ATTACH);
}

BOOL ADBTasks() {
  ADB_RESULT adb_res;
  UINT32 cmd, arg0, arg1, data_len;
  void* recv_data;

  USBHostTasks();
#ifndef USB_ENABLE_TRANSFER_EVENT
  USBHostAndroidTasks();
#endif
  if (adb_conn_state > ADB_CONN_STATE_WAIT_ATTACH) {
    if (!USBHostAndroidIsDeviceAttached()) {
      // detached
      ADBReset();
      return FALSE;
    }
    ADBPacketTasks();
    if ((adb_res = ADBPacketRecvStatus(&cmd, &arg0, &arg1, &recv_data, &data_len)) != ADB_RESULT_BUSY) {
      if (adb_res == ADB_RESULT_ERROR) {
        ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_ERROR);
      } else {
        ADBHandlePacket(cmd, arg0, arg1, recv_data, data_len);
      }
      ADBPacketRecv();
    }
  }

  switch (adb_conn_state) {
   case ADB_CONN_STATE_WAIT_ATTACH:
    if (USBHostAndroidIsDeviceAttached()) {
      ADBPacketReset();
      ADBPacketRecv();  // start receiving
      ADBPacketSend(ADB_CNXN, ADB_VERSION, ADB_PACKET_MAX_RECV_DATA_BYTES, ADB_HOSTNAME_STRING, strlen(ADB_HOSTNAME_STRING) + 1);
      ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_WAIT_CONNECT);
    }
    break;

   case ADB_CONN_STATE_WAIT_CONNECT:
    break;

   case ADB_CONN_STATE_CONNECTED:
    ADBChannelTasks();
    break;

   case ADB_CONN_STATE_ERROR:
    USBHostAndroidReset();
    ADBReset();
    // TODO: send app notification
    break;
  }

  return adb_conn_state > ADB_CONN_STATE_WAIT_CONNECT;
}

BOOL USB_ApplicationEventHandler(BYTE address, USB_EVENT event, void *data, DWORD size) {
  // Handle specific events.
  switch (event) {
   case EVENT_VBUS_REQUEST_POWER:
    // We'll let anything attach.
    return TRUE;

   case EVENT_VBUS_RELEASE_POWER:
    // We aren't keeping track of power.
    return TRUE;

   case EVENT_HUB_ATTACH:
    print0("***** USB Error - hubs are not supported *****");
    return TRUE;

   case EVENT_UNSUPPORTED_DEVICE:
    print0("***** USB Error - device is not supported *****");
    return TRUE;

   case EVENT_CANNOT_ENUMERATE:
    print0("***** USB Error - cannot enumerate device *****");
    return TRUE;

   case EVENT_CLIENT_INIT_ERROR:
    print0("***** USB Error - client driver initialization error *****");
    return TRUE;

   case EVENT_OUT_OF_MEMORY:
    print0("***** USB Error - out of heap memory *****");
    return TRUE;

   case EVENT_UNSPECIFIED_ERROR:   // This should never be generated.
    print0("***** USB Error - unspecified *****");
    return TRUE;

   default:
    return FALSE;
  }
}  // USB_ApplicationEventHandler
