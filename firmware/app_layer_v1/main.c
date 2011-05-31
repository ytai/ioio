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
#include "connection.h"
#include "features.h"
#include "protocol.h"
#include "logging.h"
#include "digital.h"

typedef enum {
  STATE_WAIT_CONNECTION,
  STATE_CONNECTED,
  STATE_ERROR,
  STATE_INCOMPATIBLE_DEVICE
} STATE;

STATE state = STATE_WAIT_CONNECTION;

void ChannelCallback(const void *data, int size) {
  if (!AppProtocolHandleIncoming(data, size)) {
    log_printf("Got corrupt input. Closing the connection and soft reset.");
    state = STATE_ERROR;
  }
}

int main() {
  int led_counter = 0;
  
  log_init();
  log_printf("***** Hello from app-layer! *******\r\n");
  ConnectionInit();
  ConnectionSetReadCallback(&ChannelCallback);
  SoftReset();

  while (1) {
    int connected = ConnectionTasks();
    if (connected == -1) {
      state = STATE_INCOMPATIBLE_DEVICE;
    } else if (connected == 0 && state > STATE_WAIT_CONNECTION) {
      log_printf("Disconnected");
      SoftReset();
      state = STATE_WAIT_CONNECTION;
    }
    switch (state) {
      case STATE_WAIT_CONNECTION:
        if (connected) {
          log_printf("Connected");
          AppProtocolInit();
          state = STATE_CONNECTED;
        }
        break;

      case STATE_CONNECTED:
        AppProtocolTasks();
        break;

      case STATE_ERROR:
        ConnectionReset();
        state = STATE_WAIT_CONNECTION;
        break;

      case STATE_INCOMPATIBLE_DEVICE:
        SetDigitalOutLevel(0, (led_counter++ >> 10) & 1);
        break;
    }
  }
  return 0;
}
