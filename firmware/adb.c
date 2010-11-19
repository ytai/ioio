#include "adb.h"
#include "adb_packet.h"
#include "uart2.h"
#include <string.h>


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
const char ADB_HOSTNAME_STRING[] = "host::";
//const char ADB_DEST_ADDR_STRING[] = "tcp:7149";

typedef enum {
  ADB_CONN_STATE_WAIT_ATTACH,
  ADB_CONN_STATE_WAIT_SEND_CONNECT,
  ADB_CONN_STATE_WAIT_RECV_CONNECT,
  ADB_CONN_STATE_RESYNC,
  ADB_CONN_STATE_WAIT_SEND_SYNC,
  ADB_CONN_STATE_WAIT_RECV_SYNC,
  ADB_CONN_STATE_IDLE
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
//static char internal_print_char_buf[256];
//#define print1(x,a) do { sprintf(internal_print_char_buf, x, a); UART2PrintString(internal_print_char_buf); } while(0)

//static BOOL adb_device_attached = FALSE;
static ADB_CONN_STATE adb_conn_state;
static ADB_CHAN_STATE adb_chan_state[ADB_MAX_CHANNELS];

////////////////////////////////////////////////////////////////////////////////
// Functions & Macros
////////////////////////////////////////////////////////////////////////////////

static void ADBResync() {
  ADBPacketReset();
  adb_conn_state = ADB_CONN_STATE_RESYNC;
}

void ADBReset() {
  int i;
  adb_conn_state = ADB_CONN_STATE_WAIT_ATTACH;
  for (i = 0; i < ADB_MAX_CHANNELS; i++) {
    adb_chan_state = ADB_CHAN_STATE_CLOSED;
  }
  ADBPacketReset();
}


ADB_RESULT ADBResetStatus();
void ADBOpen(const char* name);
ADB_RESULT ADBOpenStatus(ADB_CHANNEL_HANDLE* handle);
void ADBWrite(ADB_CHANNEL_HANDLE handle, const void* data, UINT32 data_len);
ADB_RESULT ADBWriteStatus();
void ADBRead(ADB_CHANNEL_HANDLE handle);
ADB_RESULT ADBReadStatus(ADB_CHANNEL_HANDLE handle, void** data, UINT32* data_len);
void ADBTasks();














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
    // UART2PrintString("ADB: device is attached - polled, deviceAddress=");
    // UART2PutDec(DevID.deviceAddress);
    // UART2PrintString("\r\n");
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
      UART2PrintString("ADB: Sending connect message\r\n");
      ADBSendMessageData(&adb_internal_amessage, (BYTE*)ADB_HOSTNAME_STRING, sizeof(ADB_HOSTNAME_STRING));
      adb_cnct_state = ADB_CNCT_SEND_CONNECT;
    }
    break;

   case ADB_CNCT_SEND_CONNECT:
    switch (adb_send_state) {
     case ADB_TXRX_IDLE:
       UART2PrintString("ADB: Recieving connect message\r\n");
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
      UART2PrintString("ADB: Sending open message\r\n");
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
      UART2PrintString("ADB: Recieving ready message\r\n");
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
      UART2PrintString("ADB: Connect complete.\r\n");
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
    UART2PrintString("ADB: Device got detached.\r\n");
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
