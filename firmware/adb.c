#include "adb.h"
#include "HardwareProfile.h"  // For UART2PrintString()
#include "usb_config.h"
#include "USB/usb_common.h"
#include "USB/usb_host.h"
#include "usb_host_android.h"  // For USBHostAndroid.*()
//#include "timer.h"  // For DelayMs()
#include <string.h>  // For strncmp()


// ADB protocol defines
#define ADB_SYNC 0x434e5953
#define ADB_CNXN 0x4e584e43
#define ADB_OPEN 0x4e45504f
#define ADB_OKAY 0x59414b4f
#define ADB_CLSE 0x45534c43
#define ADB_WRTE 0x45545257

#define ADB_VERSION 0x01000000        // ADB protocol version

typedef struct {
  UINT32 command;       /* command identifier constant      */
  UINT32 arg0;          /* first argument                   */
  UINT32 arg1;          /* second argument                  */
  UINT32 data_length;   /* length of payload (0 is allowed) */
  UINT32 data_check;    /* checksum of data payload         */
  UINT32 magic;         /* command ^ 0xffffffff             */
} ADB_MESSAGE_HEADER;

const char* ADB_ANDROID_DEVICE = "device::";
const unsigned int ADB_LOCAL_ID = 387;
unsigned int adb_remote_id = 0;
const char ADB_HOSTNAME_STRING[] = "host::";
const char ADB_DEST_ADDR_STRING[] = "tcp:7149";

typedef enum {
  ADB_PROCESS_SUCCESS,
  ADB_PROCESS_FAILURE,
  ADB_PROCESS_BUSY
} ADB_PROCESS_STATUS;

typedef enum {
  ADB_CNCT_IDLE,
  ADB_CNCT_WAIT_ATTACH,
  ADB_CNCT_SEND_CONNECT,
  ADB_CNCT_RECV_CONNECT,
  ADB_CNCT_SEND_OPEN,
  ADB_CNCT_RECV_READY,
  ADB_CNCT_ERROR
} ADB_CNCT_STATES;

typedef enum {
  ADB_SEND_RECV_STATE_IDLE,
  ADB_SEND_RECV_STATE_READ_WRITE,
  ADB_SEND_RECV_STATE_AMESSAGE,
  ADB_SEND_RECV_STATE_DATA
} ADB_SEND_RECV_STATES;

static char internal_print_char_buf[256];
#define print1(x,a) do { sprintf(internal_print_char_buf, x, a); UART2PrintString(internal_print_char_buf); } while(0)

static BOOL adb_device_attached = FALSE;
static ADB_SEND_RECV_STATES adb_send_state = ADB_SEND_RECV_STATE_IDLE;
static ADB_SEND_RECV_STATES adb_recv_state = ADB_SEND_RECV_STATE_IDLE;
static ADB_CNCT_STATES adb_cnct_state = ADB_CNCT_IDLE;
static const ADB_MESSAGE_HEADER* send_amessage_pointer;
static const BYTE* send_data_pointer;
static DWORD send_data_length;
static ADB_PROCESS_STATUS last_send_status = ADB_PROCESS_SUCCESS;
static ADB_MESSAGE_HEADER* recv_amessage_pointer;
static BYTE* recv_data_pointer;
static DWORD* recv_data_length_pointer;
static ADB_PROCESS_STATUS last_recv_status = ADB_PROCESS_SUCCESS;
static ADB_PROCESS_STATUS last_connect_status = ADB_PROCESS_SUCCESS;

static ADB_MESSAGE_HEADER adb_internal_amessage;
static BYTE adb_internal_buffer[512];
static DWORD adb_internal_size;


static void ADBPrintMessage(const BYTE* buf, size_t size) {
  while (size-- > 0) {
    UART2PutHex(*buf++);
    UART2PutChar(' ');
  }
  UART2PutChar('\r');
  UART2PutChar('\n');
}


static unsigned ADBChecksum(const BYTE* data, size_t len) {
  unsigned sum = 0;
  size_t i = 0;
  for (i = 0; i < len; ++i) {
    sum += data[i];
  }
  return sum;
}


static BOOL ADBSendMessageData(const ADB_MESSAGE_HEADER* msg, const BYTE* data, DWORD len) {
  if (adb_send_state != ADB_SEND_RECV_STATE_IDLE) {
    return FALSE;
  }
  send_amessage_pointer = msg;
  send_data_pointer = data;
  send_data_length = len;
  last_send_status = ADB_PROCESS_BUSY;
  adb_send_state = ADB_SEND_RECV_STATE_READ_WRITE;
  return TRUE;
}

static BOOL ADBRecvMessageData(ADB_MESSAGE_HEADER* msg, BYTE* buf, DWORD* len) {
  if (adb_recv_state != ADB_SEND_RECV_STATE_IDLE) {
    return FALSE;
  }
  recv_amessage_pointer = msg;
  recv_data_pointer = buf;
  recv_data_length_pointer = len;
  last_recv_status = ADB_PROCESS_BUSY;
  adb_recv_state = ADB_SEND_RECV_STATE_READ_WRITE;
  return TRUE;
}


static void ADBSendTasks() {
  BYTE ret_val;
  switch (adb_send_state) {
    case ADB_SEND_RECV_STATE_IDLE:
      break;

    case ADB_SEND_RECV_STATE_READ_WRITE:
      UART2PrintString("ADB: Sending message...");
      ADBPrintMessage((BYTE*)send_amessage_pointer, sizeof(ADB_MESSAGE_HEADER));
      if ((ret_val = USBHostAndroidWrite((BYTE*)send_amessage_pointer, sizeof(ADB_MESSAGE_HEADER))) != USB_SUCCESS) {
        print1("ADB: USBHostAndroidWrite failed: %x\r\n", ret_val);
        adb_send_state = ADB_SEND_RECV_STATE_IDLE;
        last_send_status = ADB_PROCESS_FAILURE;
        break;
      }
      adb_send_state = ADB_SEND_RECV_STATE_AMESSAGE;
      break;

    case ADB_SEND_RECV_STATE_AMESSAGE:
      if (USBHostAndroidTxIsComplete(&ret_val)) {
        if (ret_val != USB_SUCCESS) {
          print1("ADB: Message sending Tx got: %x\r\n", ret_val);
          adb_send_state = ADB_SEND_RECV_STATE_IDLE;
          last_send_status = ADB_PROCESS_FAILURE;
          break;
        }

        if (send_data_length == 0) {
          UART2PrintString("ADB: No data to send...\r\n");
          adb_send_state = ADB_SEND_RECV_STATE_IDLE;
          last_send_status = ADB_PROCESS_SUCCESS;
          break;
        }
        UART2PrintString("ADB: Sending data...");
        ADBPrintMessage(send_data_pointer, send_data_length);
        if ((ret_val = USBHostAndroidWrite((BYTE*)send_data_pointer, send_data_length)) != USB_SUCCESS) {
          print1("ADB: USBHostAndroidWrite failed: %x\r\n", ret_val);
          adb_send_state = ADB_SEND_RECV_STATE_IDLE;
          last_send_status = ADB_PROCESS_FAILURE;
          break;
        }

        adb_send_state = ADB_SEND_RECV_STATE_DATA;
      }
      break;

    case ADB_SEND_RECV_STATE_DATA:
      if (USBHostAndroidTxIsComplete(&ret_val)) {
        if (ret_val != USB_SUCCESS) {
          print1("Data sending Tx got: %x\r\n", ret_val);
          adb_send_state = ADB_SEND_RECV_STATE_IDLE;
          last_send_status = ADB_PROCESS_FAILURE;
          break;
        }

        UART2PrintString("ADB: Sending data complete...\r\n");
        adb_send_state = ADB_SEND_RECV_STATE_IDLE;
        last_send_status = ADB_PROCESS_SUCCESS;
      }
      break;
  }
}

static void ADBRecvTasks() {
  BYTE ret_val;
  switch (adb_recv_state) {
    case ADB_SEND_RECV_STATE_IDLE:
      break;

    case ADB_SEND_RECV_STATE_READ_WRITE:
      if ((ret_val = USBHostAndroidRead((BYTE*)recv_amessage_pointer, sizeof(ADB_MESSAGE_HEADER))) != USB_SUCCESS) {
        print1("ADB: USBHostAndroidRead failed: %x\r\n", ret_val);
        adb_recv_state = ADB_SEND_RECV_STATE_IDLE;
        last_recv_status = ADB_PROCESS_FAILURE;
        break;
      }
      adb_recv_state = ADB_SEND_RECV_STATE_AMESSAGE;
      break;

    case ADB_SEND_RECV_STATE_AMESSAGE:
      if (USBHostAndroidRxIsComplete(&ret_val, recv_data_length_pointer)) {
        if (ret_val != USB_SUCCESS) {
          print1("ADB: Message reciving Rx got: %x\r\n", ret_val);
          adb_recv_state = ADB_SEND_RECV_STATE_IDLE;
          last_recv_status = ADB_PROCESS_FAILURE;
          break;
        }

        print1("ADB: Got response of %lu bytes:\r\n", *recv_data_length_pointer);
        ADBPrintMessage((BYTE*)recv_amessage_pointer, *recv_data_length_pointer);
        if (recv_amessage_pointer->command != (~recv_amessage_pointer->magic)) {
          UART2PrintString("ADB: Recieved bad magic\r\n");
          adb_recv_state = ADB_SEND_RECV_STATE_IDLE;
          last_recv_status = ADB_PROCESS_FAILURE;
          break;
        }
        if (recv_amessage_pointer->data_length == 0) {
          *recv_data_length_pointer = 0;
          adb_recv_state = ADB_SEND_RECV_STATE_IDLE;
          last_recv_status = ADB_PROCESS_SUCCESS;
          break;
        }
        if ((ret_val = USBHostAndroidRead(recv_data_pointer, ADB_MAX_PAYLOAD)) != USB_SUCCESS) {
          print1("ADB: USBHostAndroidRead failed: %x\r\n", ret_val);
          adb_recv_state = ADB_SEND_RECV_STATE_IDLE;
          last_recv_status = ADB_PROCESS_FAILURE;
        }

        adb_recv_state = ADB_SEND_RECV_STATE_DATA;
      }
      break;

    case ADB_SEND_RECV_STATE_DATA:
      if (USBHostAndroidRxIsComplete(&ret_val, recv_data_length_pointer)) {
        if (ret_val != USB_SUCCESS) {
          print1("ADB: Data reciving Rx got: %x\r\n", ret_val);
          adb_recv_state = ADB_SEND_RECV_STATE_IDLE;
          last_recv_status = ADB_PROCESS_FAILURE;
        }

        print1("ADB: Got response of %lu bytes:\r\n", *recv_data_length_pointer);
        ADBPrintMessage(recv_data_pointer, *recv_data_length_pointer);
        if (ADBChecksum(recv_data_pointer, *recv_data_length_pointer) != recv_amessage_pointer->data_check) {
          UART2PrintString("ADB: Recieved wrong data checksum\r\n");
          adb_recv_state = ADB_SEND_RECV_STATE_IDLE;
          last_recv_status = ADB_PROCESS_FAILURE;
          break;
        }

		adb_recv_state = ADB_SEND_RECV_STATE_IDLE;
        last_recv_status = ADB_PROCESS_SUCCESS;
      }
      break;
   }
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

static void ADBFillMessageHeader(UINT32 cmd, UINT32 arg0, UINT32 arg1, const BYTE* data, UINT32 data_length) {
  adb_internal_amessage.command = cmd;
  adb_internal_amessage.arg0 = arg0;
  adb_internal_amessage.arg1 = arg1;
  adb_internal_amessage.data_length = data_length;
  adb_internal_amessage.data_check = ADBChecksum(data, data_length);
  adb_internal_amessage.magic = ~cmd;
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
     switch (last_send_status) {
      case ADB_PROCESS_SUCCESS:
        UART2PrintString("ADB: Recieving connect message\r\n");
        ADBRecvMessageData(&adb_internal_amessage, adb_internal_buffer, &adb_internal_size);
        adb_cnct_state = ADB_CNCT_RECV_CONNECT;
        break;

      case ADB_PROCESS_FAILURE:
       adb_cnct_state = ADB_CNCT_ERROR;
       break;

      case ADB_PROCESS_BUSY:
       break;
     }
     break;
  
    case ADB_CNCT_RECV_CONNECT:
      switch (last_recv_status) {
        case ADB_PROCESS_SUCCESS:
          // TODO: If we didn't get CNXN as reply, do we want to wait or die?
          if ((adb_internal_amessage.command != ADB_CNXN) ||
              (adb_internal_amessage.arg1 <= 0) ||
              (0 != strncmp((char*)&adb_internal_buffer, ADB_ANDROID_DEVICE, sizeof(ADB_ANDROID_DEVICE)))) {
            adb_cnct_state = ADB_CNCT_ERROR;
            break;
          }
          // TODO: Need to register the new MAX_PAYLOAD (arg1).
          ADBFillMessageHeader(ADB_OPEN, ADB_LOCAL_ID, 0, (const BYTE*)ADB_DEST_ADDR_STRING, sizeof(ADB_DEST_ADDR_STRING));
          UART2PrintString("ADB: Sending open message\r\n");
          ADBSendMessageData(&adb_internal_amessage, (BYTE*)ADB_DEST_ADDR_STRING, sizeof(ADB_DEST_ADDR_STRING));
          adb_cnct_state = ADB_CNCT_SEND_OPEN;
          break;

        case ADB_PROCESS_FAILURE:
          adb_cnct_state = ADB_CNCT_ERROR;
          break;

        case ADB_PROCESS_BUSY:
          break;
      }
      break;

    case ADB_CNCT_SEND_OPEN:
     switch (last_send_status) {
      case ADB_PROCESS_SUCCESS:
        UART2PrintString("ADB: Recieving ready message\r\n");
        ADBRecvMessageData(&adb_internal_amessage, adb_internal_buffer, &adb_internal_size);
        adb_cnct_state = ADB_CNCT_RECV_READY;
        break;

      case ADB_PROCESS_FAILURE:
       adb_cnct_state = ADB_CNCT_ERROR;
       break;

      case ADB_PROCESS_BUSY:
       break;
     }
     break;

    case ADB_CNCT_RECV_READY:
      switch (last_recv_status) {
        case ADB_PROCESS_SUCCESS:
          if ((adb_internal_amessage.command != ADB_OKAY) ||
              (adb_internal_amessage.data_length != 0) ||
              (adb_internal_amessage.arg0 == 0) ||
              (adb_internal_amessage.arg1 != ADB_LOCAL_ID)) {
            adb_cnct_state = ADB_CNCT_ERROR;
            break;
          }
          adb_remote_id = adb_internal_amessage.arg0;
          last_connect_status = ADB_PROCESS_SUCCESS;
          adb_cnct_state = ADB_CNCT_IDLE;
          UART2PrintString("ADB: Connect complete.\r\n");
          break;

        case ADB_PROCESS_FAILURE:
          adb_cnct_state = ADB_CNCT_ERROR;
          break;

        case ADB_PROCESS_BUSY:
          break;
      }
      break;

    case ADB_CNCT_ERROR:
      last_connect_status = ADB_PROCESS_FAILURE;
      adb_cnct_state = ADB_CNCT_IDLE;
      break;
  }
}

BOOL ADBConnect() {
  if (adb_cnct_state != ADB_CNCT_IDLE) {
    return FALSE;
  }
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
    adb_cnct_state = ADB_CNCT_IDLE;
    adb_send_state = ADB_SEND_RECV_STATE_IDLE;
    adb_recv_state = ADB_SEND_RECV_STATE_IDLE;
    if (last_send_status == ADB_PROCESS_BUSY) {
      last_send_status = ADB_PROCESS_FAILURE;
    }
    if (last_recv_status == ADB_PROCESS_BUSY) {
      last_recv_status = ADB_PROCESS_FAILURE;
    }
  }

  // TODO: Add USBTasks

  ADBConnectTasks();
  ADBSendTasks();
  ADBRecvTasks();
}
