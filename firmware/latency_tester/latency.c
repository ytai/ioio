/*
 * Copyright 2012 Ytai Ben-Tsvi. All rights reserved.
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

#include "logging.h"
#include "latency.h"
#include "../app_layer_v1/byte_queue.h"

// Connection-related stuff
DEFINE_STATIC_BYTE_QUEUE(out_queue, 4096);
static int out_size;
static int max_packet;
static CHANNEL_HANDLE handle;

// Test-related stuff
typedef enum {
  STATE_INIT = 0,
  STATE_RX_TEST = 1,
  STATE_TX_TEST = 2,
  STATE_ECHO_TEST = 3
} STATE;

static STATE state;
static uint32_t remaining;
static uint8_t buf[5];

static void Flush() {
  if (ConnectionCanSend(handle)) {
    const BYTE *data;
    if (out_size) {
      ByteQueuePull(&out_queue, out_size);
    }
    ByteQueuePeek(&out_queue, &data, &out_size);
    if (out_size > max_packet) {
      out_size = max_packet;
    }
    if (out_size) {
      ConnectionSend(handle, data, out_size);
    }
  }
}

static void Output(size_t size) {
  static uint8_t data = 0x00;
  while (size-- > 0) {
    ByteQueuePushByte(&out_queue, data++);
  }
}

void LatencyInit(CHANNEL_HANDLE h) {
  handle = h;
  max_packet = ConnectionGetMaxPacket(h);
  ByteQueueClear(&out_queue);
  state = STATE_INIT;
  remaining = 5;
  out_size = 0;
}

void LatencyTasks() {
  if (state == STATE_TX_TEST) {
    int q_remaining = ByteQueueRemaining(&out_queue);
    if (remaining <= q_remaining) {
      Output(remaining);
      state = STATE_INIT;
      remaining = 5;
    } else {
      Output(q_remaining);
      remaining -= q_remaining;
    }
  }
  Flush();
}

BOOL LatencyHandleIncoming(const void* data, size_t size) {
  const uint8_t *p = (const uint8_t *) data;
  while (size) {
    switch (state) {
      case STATE_INIT:
        if (size >= remaining) {
          memcpy(buf + 5 - remaining, p, remaining);
          size -= remaining;
          p += remaining;
          memcpy(&remaining, buf + 1, 4);
          state = buf[0];
          log_printf("Starting test #%d of size %lu", state, remaining);
        } else {
          memcpy(buf + 5 - remaining, p, size);
          remaining -= size;
          size = 0;
        }
        break;

      case STATE_RX_TEST:
        if (size >= remaining) {
          size -= remaining;
          p += remaining;
          ByteQueuePushByte(&out_queue, 1);
          state = STATE_INIT;
          remaining = 5;
        } else {
          remaining -= size;
          size = 0;
        }
        break;

      case STATE_TX_TEST:
        // we're not expecting incoming data on this test!
        return FALSE;

      case STATE_ECHO_TEST:
        if (size >= remaining) {
          ByteQueuePushBuffer(&out_queue, p, remaining);
          size -= remaining;
          p += remaining;
          state = STATE_INIT;
          remaining = 5;
        } else {
          ByteQueuePushBuffer(&out_queue, p, size);
          remaining -= size;
          size = 0;
        }
        break;
    }
  }
  return TRUE;
}

