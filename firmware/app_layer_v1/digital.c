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

static int num_digital_periodic_input_pins;
static int frame_num;
static BYTE freq_scale_by_pin[NUM_PINS];

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

// timer 4 is clocked @250kHz
// we set its period to 25 so that a match occurs @10KHz
// used for PeriodicDigitalInput
static inline void Timer4Prepare() {
  _T4IE = 0;       // disable interrupt
  _T4IF = 0;       // clear interrupt
  PR4   = 0x0019;  // period is 25 clocks
  //PR4   = 0x00F0;  // period is 240 clocks
  //PR4   = 0xC350;  // period is 50000 clocks
  _T4IP = 1;       // interrupt priority 1 (this interrupt may write to outgoing channel)
}

static inline void Timer4Start() {
  log_printf("Starting Timer");
  TMR4  = 0x0000;  // reset counter
  _T4IE = 1;       // enable interrupt
}

static inline void Timer4Stop() {
  log_printf("Stopping Timer");
  _T4IE = 0;       // disable interrupt
  _T4IF = 0;       // clear interrupt
}

static inline void DigitalInputStart() {
  Timer4Start();
}

static inline void DigitalInputStop() {
  Timer4Stop();
}

void DigitalInit() {
  int i = 0;
  num_digital_periodic_input_pins = 0;
  frame_num = 0;
  for (i = 0 ; i < NUM_PINS ; ++i) {
      freq_scale_by_pin[i] = 0;
  }
  Timer4Prepare();
}

void SetDigitalPeriodicInput(int pin, int freq_scale) {
  log_printf("SetDigitalPeriodicInput( %d, %d)", pin, freq_scale);
  SAVE_PIN_FOR_LOG(pin);

  if (freq_scale) {
    // Turn on a pin
    if (0 == freq_scale_by_pin[pin]) {
      num_digital_periodic_input_pins++;
      if (num_digital_periodic_input_pins == 1) { // this is the first.
        DigitalInputStart();
      }
    }
  } else {
    // Turn off a pin
    if (freq_scale_by_pin[pin]) {
      num_digital_periodic_input_pins--;
    }
  }
  freq_scale_by_pin[pin] = freq_scale;
}

static inline void SetBit(int value, BYTE* byte_array,
                          int* p_byte_location, int* p_bit_location) {
  if (*p_bit_location == 0) {
    // initialize the byte
    byte_array[*p_byte_location] = 0;
  }
  // set the value
  BYTE mask = 1 << *p_bit_location;
  if (value) {
    byte_array[*p_byte_location] |= mask;
  } else {
    byte_array[*p_byte_location] &= ~mask;
  }
  (*p_bit_location)++;
  if (*p_bit_location >= 8) {
    *p_bit_location = 0;
    (*p_byte_location)++;
    byte_array[*p_byte_location] = 0;
  }
}

static inline void ReportPeriodicDigitalInStatus() {

  if (num_digital_periodic_input_pins) {
    int pin = 0;
    BYTE byte_message[16];
    int byte_location = 0;
    byte_message[0] = frame_num;
    byte_location = 1;
    int bit_location = 0;
    while (pin < NUM_PINS) {
      if (freq_scale_by_pin[pin] &&
          frame_num % freq_scale_by_pin[pin] == 0) {
        // capture the bit
        SetBit(PinGetPort(pin), byte_message, &byte_location, &bit_location);
      }
      pin++;
    }

    if (byte_location != 1 || bit_location != 0) {
      OUTGOING_MESSAGE msg;
      if (bit_location != 0) {
        byte_location++;
      }

      log_printf("ReportPeriodicDigitalInStatus(): #p:%d", num_digital_periodic_input_pins);
      msg.type = REPORT_PERIODIC_DIGITAL_IN_STATUS;
      msg.args.report_periodic_digital_in_status.size = byte_location;
      AppProtocolSendMessageWithVarArg(&msg, byte_message, msg.args.report_periodic_digital_in_status.size);
    }
  } else {
    DigitalInputStop();
  }
  frame_num++;
  if (frame_num >= 240) {
    frame_num = 0;
  }
}

void __attribute__((__interrupt__, auto_psv)) _T4Interrupt() {
  ReportPeriodicDigitalInStatus();
  _T4IF = 0;  // clear
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
