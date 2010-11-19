#include <assert.h>

#include "adb_packet.h"
#include "usb_host_android.h"
#include "logging.h"

////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////

typedef struct {
  UINT32 command;       /* command identifier constant      */
  UINT32 arg0;          /* first argument                   */
  UINT32 arg1;          /* second argument                  */
  UINT32 data_length;   /* length of payload (0 is allowed) */
  UINT32 data_check;    /* checksum of data payload         */
  UINT32 magic;         /* command ^ 0xffffffff             */
} ADB_PACKET_HEADER;

typedef enum {
  ADB_PACKET_STATE_START,
  ADB_PACKET_STATE_WAIT_HEADER,
  ADB_PACKET_STATE_WAIT_DATA,
  ADB_PACKET_STATE_IDLE,
  ADB_PACKET_STATE_ERROR
} ADB_PACKET_STATE;

////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////

static ADB_PACKET_HEADER adb_packet_send_header;
static ADB_PACKET_HEADER adb_packet_recv_header;
static ADB_PACKET_STATE adb_packet_send_state;
static ADB_PACKET_STATE adb_packet_recv_state;
static const BYTE* adb_packet_send_data;
static BYTE adb_packet_recv_data[ADB_PACKET_MAX_RECV_DATA_BYTES];

////////////////////////////////////////////////////////////////////////////////
// Functions & Macros
////////////////////////////////////////////////////////////////////////////////

#define ADB_PACKET_STATE_BUSY(state) ((state) < ADB_PACKET_STATE_IDLE)
#define ADB_PACKET_CHANGE_STATE(var, state) \
  do { var = state; print2("ADBP: %s changed to %s\r\n", #var, #state); } while(0)

static UINT32 ADBChecksum(const BYTE* data, UINT32 len) {
  UINT32 sum = 0;
  UINT32 i;
  for (i = 0; i < len; ++i) {
    sum += *(data++);
  }
  return sum;
}

static void ADBPacketSendTasks() {
  BYTE ret_val;
  switch (adb_packet_send_state) {
   case ADB_PACKET_STATE_START:
//    UART2PrintString("ADB: Sending message...");
//    ADBPrintMessage((BYTE*)send_amessage_pointer, sizeof(ADB_MESSAGE_HEADER));
    if (USBHostAndroidWrite((BYTE*) &adb_packet_send_header, sizeof(ADB_PACKET_HEADER)) != USB_SUCCESS) {
//      print1("ADB: USBHostAndroidWrite failed: %x\r\n", ret_val);
      ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_ERROR);
      break;
    }
    ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_WAIT_HEADER);
    break;

   case ADB_PACKET_STATE_WAIT_HEADER:
    if (USBHostAndroidTxIsComplete(&ret_val)) {
      if (ret_val != USB_SUCCESS) {
//        print1("ADB: Message sending Tx got: %x\r\n", ret_val);
        ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_ERROR);
        break;
      }

      if (adb_packet_send_header.data_length == 0) {
//        UART2PrintString("ADB: No data to send...\r\n");
        ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_IDLE);
        break;
      }
//      UART2PrintString("ADB: Sending data...");
//      ADBPrintMessage(send_data_pointer, send_data_length);
      if (USBHostAndroidWrite(adb_packet_send_data, adb_packet_send_header.data_length) != USB_SUCCESS) {
//        print1("ADB: USBHostAndroidWrite failed: %x\r\n", ret_val);
        ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_ERROR);
        break;
      }

      ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_WAIT_DATA);
    }
    break;

   case ADB_PACKET_STATE_WAIT_DATA:
    if (USBHostAndroidTxIsComplete(&ret_val)) {
      if (ret_val != USB_SUCCESS) {
//        print1("Data sending Tx got: %x\r\n", ret_val);
        ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_ERROR);
        break;
      }

//      UART2PrintString("ADB: Sending data complete...\r\n");
      ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_IDLE);
    }
    break;

   case ADB_PACKET_STATE_IDLE:
   case ADB_PACKET_STATE_ERROR:
    break;
  }
}

static void ADBPacketRecvTasks() {
  BYTE ret_val;
  DWORD bytes_received;
  switch (adb_packet_recv_state) {
   case ADB_PACKET_STATE_START:
    if (USBHostAndroidRead((BYTE*) &adb_packet_recv_header, sizeof(ADB_PACKET_HEADER)) != USB_SUCCESS) {
//      print1("ADB: USBHostAndroidRead failed: %x\r\n", ret_val);
      ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
      break;
    }
    ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_WAIT_HEADER);
    break;

   case ADB_PACKET_STATE_WAIT_HEADER:
    if (USBHostAndroidRxIsComplete(&ret_val, &bytes_received)) {
      if (ret_val != USB_SUCCESS || bytes_received != sizeof(ADB_PACKET_HEADER)) {
//        print1("ADB: Message reciving Rx got: %x\r\n", ret_val);
        ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
        break;
      }
//      print1("ADB: Got response of %lu bytes:\r\n", *recv_data_length_pointer);
//      ADBPrintMessage((BYTE*)recv_amessage_pointer, *recv_data_length_pointer);
      if (adb_packet_recv_header.command != (~adb_packet_recv_header.magic)
          || adb_packet_recv_header.data_length >= ADB_PACKET_MAX_RECV_DATA_BYTES) {
//        UART2PrintString("ADB: Recieved bad magic\r\n");
        ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
        break;
      }
      if (adb_packet_recv_header.data_length == 0) {
        ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_IDLE);
        break;
      }
      if (USBHostAndroidRead(adb_packet_recv_data, adb_packet_recv_header.data_length) != USB_SUCCESS) {
//        print1("ADB: USBHostAndroidRead failed: %x\r\n", ret_val);
        ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
        break;
      }
      ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_WAIT_DATA);
    }
    break;

   case ADB_PACKET_STATE_WAIT_DATA:
    if (USBHostAndroidRxIsComplete(&ret_val, &bytes_received)) {
      if (ret_val != USB_SUCCESS || bytes_received != adb_packet_recv_header.data_length) {
 //       print1("ADB: Data reciving Rx got: %x\r\n", ret_val);
        ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
        break;
      }

//      print1("ADB: Got response of %lu bytes:\r\n", *recv_data_length_pointer);
//      ADBPrintMessage(recv_data_pointer, *recv_data_length_pointer);
      if (ADBChecksum(adb_packet_recv_data, adb_packet_recv_header.data_length) != adb_packet_recv_header.data_check) {
//        UART2PrintString("ADB: Recieved wrong data checksum\r\n");
        ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
        break;
      }
	    ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_IDLE);
    }
    break;
   case ADB_PACKET_STATE_IDLE:
   case ADB_PACKET_STATE_ERROR:
    break;
  }
}

void ADBPacketSend(UINT32 cmd, UINT32 arg0, UINT32 arg1, const void* data, UINT32 data_len) {                                                                        
  assert(!ADB_PACKET_STATE_BUSY(adb_packet_send_state));
  adb_packet_send_header.command = cmd;
  adb_packet_send_header.arg0 = arg0;
  adb_packet_send_header.arg1 = arg1;
  adb_packet_send_header.data_length = data_len;
  adb_packet_send_header.data_check = ADBChecksum(data, data_len);
  adb_packet_send_header.magic = ~cmd;
  adb_packet_send_data = (const BYTE*) data;
  ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_START);
}

ADB_RESULT ADBPacketSendStatus() {
  if (ADB_PACKET_STATE_BUSY(adb_packet_send_state)) return ADB_RESULT_BUSY;
  return adb_packet_send_state == ADB_PACKET_STATE_IDLE ? ADB_RESULT_OK : ADB_RESULT_ERROR;
}

void ADBPacketRecv() {
  assert(!ADB_PACKET_STATE_BUSY(adb_packet_recv_state));
  ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_START);
}

ADB_RESULT ADBPacketRecvStatus(UINT32* cmd, UINT32* arg0, UINT32* arg1, void** data, UINT32* data_len) {
  if (ADB_PACKET_STATE_BUSY(adb_packet_recv_state)) return ADB_RESULT_BUSY;
  if (adb_packet_recv_state == ADB_PACKET_STATE_ERROR) return ADB_RESULT_ERROR;
  *cmd = adb_packet_recv_header.command;
  *arg0 = adb_packet_recv_header.arg0;
  *arg1 = adb_packet_recv_header.arg1;
  *data_len = adb_packet_recv_header.data_length;
  *data = adb_packet_recv_data;
  return ADB_RESULT_OK;
}

void ADBPacketReset() {
  ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_IDLE);
  ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_IDLE);
}

void ADBPacketTasks() {
  ADBPacketSendTasks();
  ADBPacketRecvTasks();
}
