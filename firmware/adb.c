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
// Types
////////////////////////////////////////////////////////////////////////////////

// ADB protocol defines
#define ADB_SYNC 0x434e5953
#define ADB_CNXN 0x4e584e43
#define ADB_OPEN 0x4e45504f
#define ADB_OKAY 0x59414b4f
#define ADB_CLSE 0x45534c43
#define ADB_WRTE 0x45545257

#define ADB_VERSION 0x01000000        // ADB protocol version

//const char* ADB_ANDROID_DEVICE = "device::";
//const unsigned int ADB_LOCAL_ID = 387;
//unsigned int adb_remote_id = 0;
//const char ADB_DEST_ADDR_STRING[] = "tcp:7149";

typedef enum {
  ADB_CONN_STATE_WAIT_ATTACH,
  ADB_CONN_STATE_WAIT_SEND_CONNECT,
  ADB_CONN_STATE_WAIT_RECV_CONNECT,
  ADB_CONN_STATE_IDLE,
  ADB_CONN_STATE_ERROR
} ADB_CONN_STATE;

typedef enum {
  ADB_CHAN_STATE_CLOSED,
  ADB_CHAN_STATE_START,
  ADB_CHAN_STATE_WAIT_SEND_OPEN,
  ADB_CHAN_STATE_WAIT_RECV_READY,
  ADB_CHAN_STATE_IDLE,
  ADB_CHAN_STATE_ERROR
} ADB_CHAN_STATE;

////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////
static ADB_CONN_STATE adb_conn_state;
static ADB_CHAN_STATE adb_chan_state[ADB_MAX_CHANNELS];
static char ADB_HOSTNAME_STRING[] = "host::";  // Leave non-const. USB stack
                                               // doesn't work from ROM.

////////////////////////////////////////////////////////////////////////////////
// Functions & Macros
////////////////////////////////////////////////////////////////////////////////
//static void ADBResync() {
//  ADBPacketReset();
//  adb_conn_state = ADB_CONN_STATE_RESYNC;
//}

//void ADBReset() {
//  int i;
//  adb_conn_state = ADB_CONN_STATE_WAIT_ATTACH;
//  for (i = 0; i < ADB_MAX_CHANNELS; i++) {
//    adb_chan_state = ADB_CHAN_STATE_CLOSED;
//  }
//  ADBPacketReset();
//}
//
//
//ADB_RESULT ADBResetStatus();

static void ADBChannelTasks() {
}

void ADBOpen(const char* name);
ADB_RESULT ADBOpenStatus(ADB_CHANNEL_HANDLE* handle);
void ADBWrite(ADB_CHANNEL_HANDLE handle, const void* data, UINT32 data_len);
ADB_RESULT ADBWriteStatus();
void ADBRead(ADB_CHANNEL_HANDLE handle);
ADB_RESULT ADBReadStatus(ADB_CHANNEL_HANDLE handle, void** data, UINT32* data_len);

void ADBInit() {
  BOOL res = USBHostInit(0);
  assert(res);
}

void ADBTasks() {
  ADB_RESULT adb_res;
  UINT32 cmd, arg0, arg1, data_len;
  void* recv_data;

  USBHostTasks();
#ifndef USB_ENABLE_TRANSFER_EVENT
  USBHostAndroidTasks();
#endif
  ADBPacketTasks();

  switch (adb_conn_state) {
   case ADB_CONN_STATE_WAIT_ATTACH:
    if (USBHostAndroidIsDeviceAttached()) {
      ADBPacketReset();
      ADBPacketSend(ADB_CNXN, ADB_VERSION, ADB_PACKET_MAX_RECV_DATA_BYTES, ADB_HOSTNAME_STRING, strlen(ADB_HOSTNAME_STRING) + 1);
      ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_WAIT_SEND_CONNECT);
    }
    break;

   case ADB_CONN_STATE_WAIT_SEND_CONNECT:
    if ((adb_res = ADBPacketSendStatus()) != ADB_RESULT_BUSY) {
      if (adb_res == ADB_RESULT_OK) {
        ADBPacketRecv();
        ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_WAIT_RECV_CONNECT);
      } else {
        print0("Error sending connection message.");
        ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_ERROR);
      }
    }
    break;

   case ADB_CONN_STATE_WAIT_RECV_CONNECT:
    if ((adb_res = ADBPacketRecvStatus(&cmd, &arg0, &arg1, &recv_data, &data_len)) != ADB_RESULT_BUSY) {
      if (adb_res == ADB_RESULT_OK && cmd == ADB_CNXN) {
        print1("ADB established connection with [%s]", (const char*) recv_data);
        // TODO: send app notification
        ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_IDLE);
      } else {
        print0("Error receiving connection message.");
        ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_ERROR);
      }
    }
    break;

   case ADB_CONN_STATE_IDLE:
    ADBChannelTasks();
    break;

   case ADB_CONN_STATE_ERROR:
    // USBHostAndroidReset();
    // TODO: send app notification
    ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_WAIT_ATTACH);
    break;
  }
}

BOOL USB_ApplicationEventHandler(BYTE address, USB_EVENT event, void *data, DWORD size) {
  // TODO: detach message should have a separate callback.
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

   case EVENT_DETACH:
    print0("***** Device detached *****");
    // TODO: send app notification
    ADB_CHANGE_STATE(adb_conn_state, ADB_CONN_STATE_WAIT_ATTACH);
    return TRUE;

   case EVENT_SUSPEND:
   case EVENT_RESUME:
    return TRUE;

   default:
    return FALSE;
  }
}  // USB_ApplicationEventHandler



#if 0
static void ADBPrintMessage(const BYTE* buf, size_t size) {
  while (size-- > 0) {
    UART2PutHex(*buf++);
    UART2PutChar(' ');
  }
  UART2PutChar('\r');
  UART2PutChar('\n');
}

static BOOL ADBCheckForNewAttach() {
  if (!adb_device_attached && USBHostAndroidIsDeviceAttached()) {
    adb_device_attached = TRUE;
    // ANDROID_DEVICE_ID DevID;
    // USBHostAndroidGetDeviceId(&DevID);
    // print0("ADB: device is attached - polled, deviceAddress=");
    // UART2PutDec(DevID.deviceAddress);
    // print0("");
    // TODO: need to check that the device is actually android?
    return TRUE;
  }
  return FALSE;
}

static void ADBConnectTasks() {
  switch (adb_cnct_state) {
   case ADB_CNCT_IDLE:
    break;

   case ADB_CNCT_WAIT_ATTACH:
    if (ADBCheckForNewAttach()) {
      ADBFillMessageHeader(ADB_CNXN, ADB_VERSION, ADB_MAX_PAYLOAD, (const BYTE*)ADB_HOSTNAME_STRING, sizeof(ADB_HOSTNAME_STRING));
      print0("ADB: Sending connect message");
      ADBSendMessageData(&adb_internal_amessage, (BYTE*)ADB_HOSTNAME_STRING, sizeof(ADB_HOSTNAME_STRING));
      adb_cnct_state = ADB_CNCT_SEND_CONNECT;
    }
    break;

   case ADB_CNCT_SEND_CONNECT:
    switch (adb_send_state) {
     case ADB_TXRX_IDLE:
       print0("ADB: Recieving connect message");
       ADBRecvMessageData(&adb_internal_amessage, adb_internal_buffer, &adb_internal_size);
       adb_cnct_state = ADB_CNCT_RECV_CONNECT;
       break;

     case ADB_TXRX_ERROR:
      adb_cnct_state = ADB_CNCT_ERROR;
      break;

     default:
      break;
    }
    break;
  
   case ADB_CNCT_RECV_CONNECT:
    switch (adb_recv_state) {
     case ADB_TXRX_IDLE:
      // TODO: If we didn't get CNXN as reply, do we want to wait or die?
      if ((adb_internal_amessage.command != ADB_CNXN)
          || (adb_internal_amessage.arg1 <= 0)
          || (0 != strncmp((char*)&adb_internal_buffer, ADB_ANDROID_DEVICE, sizeof(ADB_ANDROID_DEVICE)))) {
        adb_cnct_state = ADB_CNCT_ERROR;
        break;
      }
      // TODO: Need to register the new MAX_PAYLOAD (arg1).
      ADBFillMessageHeader(ADB_OPEN, ADB_LOCAL_ID, 0, (const BYTE*)ADB_DEST_ADDR_STRING, sizeof(ADB_DEST_ADDR_STRING));
      print0("ADB: Sending open message");
      ADBSendMessageData(&adb_internal_amessage, (BYTE*)ADB_DEST_ADDR_STRING, sizeof(ADB_DEST_ADDR_STRING));
      adb_cnct_state = ADB_CNCT_SEND_OPEN;
      break;

     case ADB_TXRX_ERROR:
      adb_cnct_state = ADB_CNCT_ERROR;
      break;

     default:
      break;
    }
    break;

   case ADB_CNCT_SEND_OPEN:
    switch (adb_send_state) {
     case ADB_TXRX_IDLE:
      print0("ADB: Recieving ready message");
      ADBRecvMessageData(&adb_internal_amessage, adb_internal_buffer, &adb_internal_size);
      adb_cnct_state = ADB_CNCT_RECV_READY;
      break;

     case ADB_TXRX_ERROR:
      adb_cnct_state = ADB_CNCT_ERROR;
      break;

     default:
      break;
    }
    break;

   case ADB_CNCT_RECV_READY:
    switch (adb_recv_state) {
     case ADB_TXRX_IDLE:
      if ((adb_internal_amessage.command != ADB_OKAY)
          || (adb_internal_amessage.data_length != 0)
          || (adb_internal_amessage.arg0 == 0)
          || (adb_internal_amessage.arg1 != ADB_LOCAL_ID)) {
        adb_cnct_state = ADB_CNCT_ERROR;
        break;
      }
      adb_remote_id = adb_internal_amessage.arg0;
      adb_cnct_state = ADB_CNCT_IDLE;
      print0("ADB: Connect complete.");
      break;

     case ADB_TXRX_ERROR:
      adb_cnct_state = ADB_CNCT_ERROR;
      break;

     default:
      break;
    }
    break;

   case ADB_CNCT_ERROR:
    adb_cnct_state = ADB_CNCT_IDLE;
    break;
  }
}

BOOL ADBConnect() {
  if (ADB_CNCT_BUSY(adb_cnct_state)) return FALSE;

  adb_cnct_state = ADB_CNCT_WAIT_ATTACH;
  return TRUE;
}

void ADBTasks() {
  // Maintain USB Host State
  USBHostTasks();
#ifndef USB_ENABLE_TRANSFER_EVENT
  USBHostAndroidTasks();
#endif

  // Watch for device detaching
  if (adb_device_attached && !USBHostAndroidIsDeviceAttached()) {
    print0("ADB: Device got detached.");
    adb_device_attached = FALSE;
    adb_cnct_state = ADB_CNCT_ERROR;
    adb_send_state = ADB_TXRX_IDLE;
    adb_recv_state = ADB_TXRX_IDLE;
  }

  // TODO: Add USBTasks

  ADBConnectTasks();
  ADBSendTasks();
  ADBRecvTasks();
}
#endif
