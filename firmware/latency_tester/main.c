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

#include <stdint.h>

#include "Compiler.h"
#include "libconn/connection.h"
#include "common/logging.h"
#include "app_layer_v1/byte_queue.h"

// define in non-const arrays to ensure data space
static char descManufacturer[] = "IOIO Open Source Project";
static char descModel[] = "IOIO";
static char descDesc[] = "IOIO Latency Tester";
static char descVersion[] = "IOIO0300";
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
  STATE_WAIT_CONNECTION,
  STATE_WAIT_CHANNEL_OPEN,
  STATE_CONNECTED,
  STATE_ERROR
} STATE;

static STATE state = STATE_INIT;
static CHANNEL_HANDLE handle;
static int out_size = 0;
static const BYTE *out_data;
static int max_packet;
DEFINE_STATIC_BYTE_QUEUE(out_queue, 4096);

void AppCallback(CHANNEL_HANDLE h, const void* data, UINT32 data_len);

static inline CHANNEL_HANDLE OpenAvailableChannel() {
  if (ConnectionCanOpenChannel(CHANNEL_TYPE_ADB)) {
    return ConnectionOpenChannelAdb("tcp:4545", &AppCallback);
  }
  if (ConnectionCanOpenChannel(CHANNEL_TYPE_BT)) {
    return ConnectionOpenChannelBtServer(&AppCallback);
  }
  return INVALID_CHANNEL_HANDLE;
}

void AppCallback(CHANNEL_HANDLE h, const void* data, UINT32 data_len) {
  if (data) {
    const uint8_t *p = (const uint8_t *) data;
    while (data_len--) {
      ByteQueuePushByte(&out_queue, -*(p++));
    }
  } else {
    // connection closed, soft reset and re-establish
    if (state == STATE_CONNECTED) {
      log_printf("Channel closed");
      ByteQueueClear(&out_queue);
      out_size = 0;
    } else {
      log_printf("Channel failed to open");
    }
    handle = OpenAvailableChannel();
    max_packet = ConnectionGetMaxPacket(handle);
    state = STATE_WAIT_CHANNEL_OPEN;
  }
}

void AppTasks() {
  if (ConnectionCanSend(handle)) {
    if (out_size) {
      ByteQueuePull(&out_queue, out_size);
      out_size = 0;
    }
    if (ByteQueueSize(&out_queue)) {
      ByteQueuePeek(&out_queue, &out_data, &out_size);
      if (out_size > max_packet) {
        out_size = max_packet;
      }
      ConnectionSend(handle, out_data, out_size);
    }
  }
}

int main() {
  log_init();
  log_printf("***** Hello from app-layer! *******\r\n");
  TRISFbits.TRISF3 = 1;
  
  ConnectionInit();
  ByteQueueInit(&out_queue, out_queue_buf, sizeof out_queue_buf);
  while (1) {
    ConnectionTasks();
    BOOL can_open_channel = ConnectionCanOpenChannel(CHANNEL_TYPE_ADB)
                            || ConnectionCanOpenChannel(CHANNEL_TYPE_BT);
    if (!can_open_channel
        && state > STATE_WAIT_CONNECTION) {
      // just got disconnected
      log_printf("Disconnected");
      ByteQueueClear(&out_queue);
      out_size = 0;
      state = STATE_INIT;
    }
    switch (state) {
      case STATE_INIT:
        handle = INVALID_CHANNEL_HANDLE;
        state = STATE_WAIT_CONNECTION;
        break;

      case STATE_WAIT_CONNECTION:
        if (can_open_channel) {
          log_printf("Connected");
          handle = OpenAvailableChannel();
          max_packet = ConnectionGetMaxPacket(handle);
          state = STATE_WAIT_CHANNEL_OPEN;
        }
        break;

      case STATE_WAIT_CHANNEL_OPEN:
       if (ConnectionCanSend(handle)) {
          log_printf("Channel open");
          state = STATE_CONNECTED;
        }
        break;

      case STATE_CONNECTED:
        AppTasks();
        break;

      case STATE_ERROR:
        ConnectionCloseChannel(handle);
        state = STATE_INIT;
        break;
    }
  }
  return 0;
}
