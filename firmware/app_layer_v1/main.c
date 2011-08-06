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

#include "Compiler.h"
#include "libconn/adb.h"
#include "libconn/connection.h"
#include "features.h"
#include "protocol.h"
#include "logging.h"

typedef enum {
  STATE_INIT,
  STATE_WAIT_CONNECTION,
  STATE_WAIT_CHANNEL_OPEN,
  STATE_CONNECTED,
  STATE_ERROR
} STATE;

static STATE state = STATE_INIT;
static ADB_CHANNEL_HANDLE handle;

void ChannelCallback(ADB_CHANNEL_HANDLE h, const void* data, UINT32 data_len) {
  if (data) {
    if (!AppProtocolHandleIncoming(data, data_len)) {
      // got corrupt input. need to close the connection and soft reset.
      state = STATE_ERROR;
    }
  } else {
    // connection closed, soft reset and re-establish
    if (state == STATE_CONNECTED) {
      log_printf("ADB channel closed");
      SoftReset();
    }
    handle = ADBOpen("tcp:4545", &ChannelCallback);
    state = STATE_WAIT_CHANNEL_OPEN;
  }
}

int main() {
  log_init();
  log_printf("***** Hello from app-layer! *******\r\n");

  ConnectionInit();
  SoftReset();
  while (1) {
    BOOL adb_connected = ConnectionTasks();
    if (adb_connected) {
        adb_connected = ADBAttached();
    }
    if (!adb_connected && state > STATE_WAIT_CONNECTION) {
      // just got disconnected
      log_printf("ADB disconnected");
      SoftReset();
      state = STATE_INIT;
    }
    switch (state) {
      case STATE_INIT:
        handle = ADB_INVALID_CHANNEL_HANDLE;
        state = STATE_WAIT_CONNECTION;
        break;

      case STATE_WAIT_CONNECTION:
        if (adb_connected) {
          log_printf("ADB connected");
          handle = ADBOpen("tcp:4545", &ChannelCallback);
          state = STATE_WAIT_CHANNEL_OPEN;
        }
        break;

      case STATE_WAIT_CHANNEL_OPEN:
       if (ADBChannelReady(handle)) {
          log_printf("ADB channel open");
          AppProtocolInit(handle);
          state = STATE_CONNECTED;
        }
        break;

      case STATE_CONNECTED:
        AppProtocolTasks(handle);
        break;

      case STATE_ERROR:
        ADBClose(handle);
        state = STATE_INIT;
        break;
    }
  }
  return 0;
}
