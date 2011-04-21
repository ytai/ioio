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
#include "digital.h"

#include "Compiler.h"
#include "logging.h"
#include "pins.h"
#include "protocol.h"
#include "sync.h"

void SetDigitalOutLevel(int pin, int value) {
  log_printf("SetDigitalOutLevel(%d, %d)", pin, value);
  SAVE_PIN_FOR_LOG(pin);
  BYTE prev = SyncInterruptLevel(4);
  PinSetLat(pin, value);
  SyncInterruptLevel(prev);
}

void SetChangeNotify(int pin, int changeNotify) {
  int cnie_backup = _CNIE;
  log_printf("SetChangeNotify(%d, %d)", pin, changeNotify);
  SAVE_PIN_FOR_LOG(pin);
  _CNIE = 0;
  PinSetCnen(pin, changeNotify);
  if (changeNotify) {
    PinSetCnforce(pin);
    _CNIF = 1;  // force a status message on the new pin
  }
  _CNIE = cnie_backup;
}

static void SendDigitalInStatusMessage(int pin, int value) {
  log_printf("SendDigitalInStatusMessage(%d, %d)", pin, value);
  SAVE_PIN_FOR_LOG(pin);
  OUTGOING_MESSAGE msg;
  msg.type = REPORT_DIGITAL_IN_STATUS;
  msg.args.report_digital_in_status.pin = pin;
  msg.args.report_digital_in_status.level = value;
  AppProtocolSendMessage(&msg);
}

#define CHECK_PORT_CHANGE(name)                                        \
  do {                                                                 \
    unsigned int i = 0;                                                \
    unsigned int port = PORT##name;                                    \
    unsigned int changed = (CNFORCE##name | (port ^ CNBACKUP##name))   \
                           & CNEN##name;                               \
    CNBACKUP##name = port;                                             \
    CNFORCE##name = 0x0000;                                            \
    while (changed) {                                                  \
      if (changed & 1) {                                               \
        SendDigitalInStatusMessage(PinFromPort##name(i), (port & 1));  \
      }                                                                \
      ++i;                                                             \
      port >>= 1;                                                      \
      changed >>= 1;                                                   \
    }                                                                  \
  } while (0)


void __attribute__((__interrupt__, auto_psv)) _CNInterrupt() {
  _CNIF = 0;
  log_printf("_CNInterrupt()");

  CHECK_PORT_CHANGE(B);
  CHECK_PORT_CHANGE(C);
  CHECK_PORT_CHANGE(D);
  CHECK_PORT_CHANGE(E);
  CHECK_PORT_CHANGE(F);
  CHECK_PORT_CHANGE(G);
}
