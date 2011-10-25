/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */

#include <assert.h>

#include "adb_packet.h"
#include "usb_host_android.h"
#include "logging.h"

#define CHANGE_STATE(var,state)              \
do {                                         \
  log_printf("%s set to %s", #var, #state);  \
  var = state;                               \
} while (0)

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
    if (USBHostAndroidWrite((BYTE*) &adb_packet_send_header,
                            sizeof(ADB_PACKET_HEADER),
                            ANDROID_INTERFACE_ADB) != USB_SUCCESS) {
      CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_ERROR);
      break;
    }
    CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_WAIT_HEADER);
    break;

   case ADB_PACKET_STATE_WAIT_HEADER:
    if (USBHostAndroidTxIsComplete(&ret_val, ANDROID_INTERFACE_ADB)) {
      if (ret_val != USB_SUCCESS) {
        CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_ERROR);
        break;
      }
      if (adb_packet_send_header.data_length == 0) {
        CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_IDLE);
        break;
      }
      if (USBHostAndroidWrite(adb_packet_send_data,
                              adb_packet_send_header.data_length,
                              ANDROID_INTERFACE_ADB) != USB_SUCCESS) {
        CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_ERROR);
        break;
      }
      CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_WAIT_DATA);
    }
    break;

   case ADB_PACKET_STATE_WAIT_DATA:
    if (USBHostAndroidTxIsComplete(&ret_val, ANDROID_INTERFACE_ADB)) {
      if (ret_val != USB_SUCCESS) {
        CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_ERROR);
        break;
      }
      CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_IDLE);
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
    if (USBHostAndroidRead((BYTE*) &adb_packet_recv_header,
                           sizeof(ADB_PACKET_HEADER),
                           ANDROID_INTERFACE_ADB) != USB_SUCCESS) {
      CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
      break;
    }
    CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_WAIT_HEADER);
    break;

   case ADB_PACKET_STATE_WAIT_HEADER:
    if (USBHostAndroidRxIsComplete(&ret_val, &bytes_received,
                                   ANDROID_INTERFACE_ADB)) {
      if (ret_val != USB_SUCCESS) {
        CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
        break;
      }
// TODO: probably not needed
//      if (bytes_received == 0) {
//        adb_packet_recv_header.command = 0;
//        CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_IDLE);
//        break;
//      }
      if (bytes_received != sizeof(ADB_PACKET_HEADER)
          || adb_packet_recv_header.command != (~adb_packet_recv_header.magic)
          || adb_packet_recv_header.data_length > ADB_PACKET_MAX_RECV_DATA_BYTES) {
        CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
        break;
      }
      if (adb_packet_recv_header.data_length == 0) {
        CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_IDLE);
        break;
      }
      if (USBHostAndroidRead(adb_packet_recv_data,
                             adb_packet_recv_header.data_length,
                             ANDROID_INTERFACE_ADB) != USB_SUCCESS) {
        CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
        break;
      }
      CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_WAIT_DATA);
    }
    break;

   case ADB_PACKET_STATE_WAIT_DATA:
    if (USBHostAndroidRxIsComplete(&ret_val,
                                   &bytes_received,
                                   ANDROID_INTERFACE_ADB)) {
      if (ret_val != USB_SUCCESS || bytes_received != adb_packet_recv_header.data_length) {
        CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
        break;
      }

      if (ADBChecksum(adb_packet_recv_data, adb_packet_recv_header.data_length) != adb_packet_recv_header.data_check) {
        CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_ERROR);
        break;
      }
	    CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_IDLE);
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
  CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_START);
}

ADB_RESULT ADBPacketSendStatus() {
  if (ADB_PACKET_STATE_BUSY(adb_packet_send_state)) return ADB_RESULT_BUSY;
  return adb_packet_send_state == ADB_PACKET_STATE_IDLE ? ADB_RESULT_OK : ADB_RESULT_ERROR;
}

void ADBPacketRecv() {
  assert(!ADB_PACKET_STATE_BUSY(adb_packet_recv_state));
  CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_START);
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
  CHANGE_STATE(adb_packet_send_state, ADB_PACKET_STATE_IDLE);
  CHANGE_STATE(adb_packet_recv_state, ADB_PACKET_STATE_IDLE);
}

void ADBPacketTasks() {
  ADBPacketSendTasks();
  ADBPacketRecvTasks();
}

