/*
 * Copyright 2012 Markus Lanthaler <mail@markus-lanthaler.com>. All rights reserved.
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
#include "HardwareProfile.h"
#include "GenericTypeDefs.h"
#include "ir.h"
#include "pins.h"
#include "timer.h"
#include "logging.h"
#include "sync.h"

#define delayMicroseconds(x)                                                            \
{                                                                                       \
  unsigned long _dcnt = (x)*((unsigned long)(0.000001/(1.0/GetInstructionClock())/6));  \
  while(_dcnt--);                                                                       \
}

#define IR_BUFFFER_SIZE 256       // ir data buffer size

static WORD ir_buffer[IR_BUFFFER_SIZE] __attribute__((far));
static WORD ir_buffer_length = 0;


//--------------------------------------------------------------------------
void bufferIrData(WORD data) {
 if(ir_buffer_length < IR_BUFFFER_SIZE) {
    ir_buffer[ir_buffer_length] = data;
    ir_buffer_length++;
  } else {
    log_printf("IR buffer too small! Cleared it");
    ir_buffer_length = 0;
  }
}


//--------------------------------------------------------------------------
/**
 * Send a burst-pair over infrared.
 *
 * This method sends a burst pair over IR. A burst pair is a on/off
 * sequence with the specified duration. As result, this method first
 * flashes the IR transmitter LED for "on_length" periods with a
 * frequency of (1000 / (2 * pulse_width) kHz) and then it switches the IR
 * transmitter LED of for "off_length" periods.
 *
 * @param pin the pin to which the LED is attached.
 * @param pulse_width the pulse width of the carrier (= carrier frequency / 2)
 *                    in microseconds.
 * @param on_length the length of the high-signal to sent in number of carrier
 *                  periods.
 * @param off_length the length of the low-signal to sent in number of carrier
 *                   periods.
 */
static inline void sendIrBurstPair(int pin, unsigned int pulse_width, WORD on_length, WORD off_length) {
  // activate pulse modulated signal
  for(; on_length > 0; on_length--)
  {
    PinSetLat(pin, 1);  // set IR pin to HIGH
    delayMicroseconds(pulse_width);
    PinSetLat(pin, 0);  // set IR pin to LOW
    delayMicroseconds(pulse_width);
  }

  // deactivate pulse modulated signal
  for(; off_length > 0; off_length--)
  {
    PinSetLat(pin, 0);  // set IR pin to LOW
    delayMicroseconds(pulse_width);
    PinSetLat(pin, 0);  // do it again to get timing right
    delayMicroseconds(pulse_width);
  }
}


//--------------------------------------------------------------------------
void sendBufferedIrData(int pin) {
  if (ir_buffer_length < 4) {
    log_printf("IR buffer empty");
    ir_buffer_length = 0;
    return;
  }

  unsigned int i = 0;
  // TODO If the pulse width is taken from the array I get a 33% duty cycle instead of 50% - fix this
  // The current value is tuned to a Pronto value of 006E
  unsigned int pronto_frequency = 130;  // ir_buffer[1];
  unsigned int sequence1_length = ir_buffer[2] * 2;
  unsigned int sequence2_length = ir_buffer[3] * 2;

  if (ir_buffer_length < 4 + sequence1_length + sequence2_length) {
    log_printf("IR data buffer does not contain all required data");
    ir_buffer_length = 0;
    return;
  }

  unsigned int pulse_width = (pronto_frequency * 0.241246) / 2;

  BYTE prev = SyncInterruptLevel(1);

  if(sequence1_length > 0)
  {
    for(i = 0; i < sequence1_length; i += 2)
    {
      sendIrBurstPair(pin, pulse_width, ir_buffer[4 + i], ir_buffer[5 + i]);
    }
  }

  if(sequence2_length > 0)
  {
    for(i = 0; i < sequence2_length; i += 2)
    {
      sendIrBurstPair(pin, pulse_width, ir_buffer[4 + sequence1_length + i], ir_buffer[5 + sequence1_length + i]);
    }
  }

  ir_buffer_length = 0;

  SyncInterruptLevel(prev);
}
