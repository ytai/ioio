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

#include "timers.h"

#include "Compiler.h"
#include "logging.h"
#include "sync.h"

void TimersInit() {
  log_printf("TimersInit()");

  // The timers are initialized as follows:
  // T2, T5: sysclk / 256 = 62.5KHz
  // T3    : sysclk / 8 = 2MHz
  // T4    : sysclk / 64 = 250KHz
  // T3, T4, T5 all have their prescaler synchronized (i.e., in the exact same
  // cycle when T5 increments, T3 and T4 increments as well.
  // T2 lags behind T5 by exactly 46 cycles. Together with the 220 cycles it
  // takes the ISR to process a sequencer cue, we're at 256 cycles, meaning all
  // timers are immediately incremented.

  T2CON = 0x8030;  // timer 2 is sysclk / 256 = 62.5KHz
  T3CON = 0x8010;  // timer 3 is sysclk / 8 = 2MHz
  T4CON = 0x8020;  // timer 4 is sysclk / 64 = 250KHz
  T5CON = 0x8030;  // timer 5 is sysclk / 256 = 62.5KHz

  PRIORITY(7) {
    asm("clr _TMR4\n"
        "repeat #53\n"
        "nop\n"
        "clr _TMR3\n"
        "repeat #5\n"
        "nop\n"
        "clr _TMR5\n"
        "repeat #43\n"
        "nop\n"
        "clr _TMR2\n");
  }
}
