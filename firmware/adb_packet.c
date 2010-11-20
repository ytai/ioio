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
    if (USBHostAndroidWrite((BYTE*) &adb_packet_send_header, sizeof(ADB_PACKET_HEADER)) != USB_SUCCESS) {
      ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_ERROR);
      break;
    }
    ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_WAIT_HEADER);
    break;

   case ADB_PACKET_STATE_WAIT_HEADER:
    if (USBHostAndroidTxIsComplete(&ret_val)) {
      if (ret_val != USB_SUCCESS) {
        ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_ERROR);
        break;
      }
      if (adb_packet_send_header.data_length == 0) {
        ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_IDLE);
        break;
      }
      if (USBHostAndroidWrite(adb_packet_send_data, adb_packet_send_header.data_length) != USB_SUCCESS) {
        ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_ERROR);
        break;
      }
      ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_WAIT_DATA);
    }
    break;

   case ADB_PACKET_STATE_WAIT_DATA:
    if (USBHostAndroidTxIsComplete(&ret_val)) {
      if (ret_val != USB_SUCCESS) {
        ADB_PACKET_CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_ERROR);
        break;
      }
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
      ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
      break;
    }
    ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_WAIT_HEADER);
    break;

   case ADB_PACKET_STATE_WAIT_HEADER:
    if (USBHostAndroidRxIsComplete(&ret_val, &bytes_received)) {
      if (ret_val != USB_SUCCESS || bytes_received != sizeof(ADB_PACKET_HEADER)) {
        ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
        break;
      }
      if (adb_packet_recv_header.command != (~adb_packet_recv_header.magic)
          || adb_packet_recv_header.data_length >= ADB_PACKET_MAX_RECV_DATA_BYTES) {
        ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
        break;
      }
      if (adb_packet_recv_header.data_length == 0) {
        ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_IDLE);
        break;
      }
      if (USBHostAndroidRead(adb_packet_recv_data, adb_packet_recv_header.data_length) != USB_SUCCESS) {
        ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
        break;
      }
      ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_WAIT_DATA);
    }
    break;

   case ADB_PACKET_STATE_WAIT_DATA:
    if (USBHostAndroidRxIsComplete(&ret_val, &bytes_received)) {
      if (ret_val != USB_SUCCESS || bytes_received != adb_packet_recv_header.data_length) {
        ADB_PACKET_CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
        break;
      }

      if (ADBChecksum(adb_packet_recv_data, adb_packet_recv_header.data_length) != adb_packet_recv_header.data_check) {
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
