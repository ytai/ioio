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
#include "libconn/connection.h"
#include "features.h"
#include "protocol.h"
#include "logging.h"

// define in non-const arrays to ensure data space
static char descManufacturer[] = "IOIO Open Source Project";
static char descModel[] = "IOIO";
static char descDesc[] = "IOIO Standard Application";
static char descVersion[] = FW_IMPL_VER;
static char descUri[] = "https://github.com/ytai/ioio/wiki/ADK";
static char descSerial[] = "N/A";

const char* accessoryDescs[6] = {
  descManufacturer,
  descModel,
  descDesc,
  descVersion,
  descUri,
  descSerial
};

typedef enum {
  STATE_INIT,
  STATE_OPEN_CHANNEL,
  STATE_WAIT_CHANNEL_OPEN,
  STATE_CONNECTED,
  STATE_ERROR
} STATE;

static STATE state = STATE_INIT;
static CHANNEL_HANDLE handle;

void AppCallback(CHANNEL_HANDLE h, const void* data, UINT32 data_len);

static inline CHANNEL_HANDLE OpenAvailableChannel() {
  if (ConnectionTypeSupported(CHANNEL_TYPE_ADB)) {
    if (ConnectionCanOpenChannel(CHANNEL_TYPE_ADB)) {
      return ConnectionOpenChannelAdb("tcp:4545", &AppCallback);
    }
  } else if (ConnectionTypeSupported(CHANNEL_TYPE_ACC)) {
    if (ConnectionCanOpenChannel(CHANNEL_TYPE_ACC)) {
      return ConnectionOpenChannelAccessory(&AppCallback);
    }
  } else if (ConnectionTypeSupported(CHANNEL_TYPE_BT)) {
    if (ConnectionCanOpenChannel(CHANNEL_TYPE_BT)) {
      return ConnectionOpenChannelBtServer(&AppCallback);
    }
  }
  return INVALID_CHANNEL_HANDLE;
}

void AppCallback(CHANNEL_HANDLE h, const void* data, UINT32 data_len) {
  if (data) {
    if (!AppProtocolHandleIncoming(data, data_len)) {
      // got corrupt input. need to close the connection and soft reset.
      state = STATE_ERROR;
    }
  } else {
    // connection closed, soft reset and re-establish
    if (state == STATE_CONNECTED) {
      log_printf("Channel closed");
      SoftReset();
    } else {
      log_printf("Channel failed to open");
    }
    state = STATE_OPEN_CHANNEL;
  }
}

int main() {
  log_init();
  log_printf("***** Hello from app-layer! *******");

  ConnectionInit();
  SoftReset();
  while (1) {
    BOOL connected = ConnectionTasks();
    if (!connected
        && state > STATE_OPEN_CHANNEL) {
      // just got disconnected
      log_printf("Disconnected");
      SoftReset();
      state = STATE_INIT;
    }
    switch (state) {
      case STATE_INIT:
        handle = INVALID_CHANNEL_HANDLE;
        state = STATE_OPEN_CHANNEL;
        break;

      case STATE_OPEN_CHANNEL:
        if ((handle = OpenAvailableChannel()) != INVALID_CHANNEL_HANDLE) {
          log_printf("Connected");
          state = STATE_WAIT_CHANNEL_OPEN;
        }
        break;

      case STATE_WAIT_CHANNEL_OPEN:
       if (ConnectionCanSend(handle)) {
          log_printf("Channel open");
          AppProtocolInit(handle);
          state = STATE_CONNECTED;
        }
        break;

      case STATE_CONNECTED:
        AppProtocolTasks(handle);
        break;

      case STATE_ERROR:
        ConnectionCloseChannel(handle);
        state = STATE_INIT;
        break;
    }
  }
  return 0;
}
