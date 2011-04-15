/*
 * Copyright 2011. All rights reserved.
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

#include "incap.h"

#include <assert.h>

#include "Compiler.h"
#include "board.h"
#include "logging.h"
#include "pp_util.h"

DEFINE_REG_SETTERS_1B(NUM_INCAP_MODULES, _IC, IF)
DEFINE_REG_SETTERS_1B(NUM_INCAP_MODULES, _IC, IE)
DEFINE_REG_SETTERS_1B(NUM_INCAP_MODULES, _IC, IP)

typedef struct {
  unsigned int con1;
  unsigned int con2;
  unsigned int buf;
  unsigned int tmr;
} INCAP_REG;

static volatile INCAP_REG* incap_regs = (volatile INCAP_REG *) &IC1CON1;

void InCapInit() {
  log_printf("InCapInit()");
  int i;
  for (i = 0; i < NUM_INCAP_MODULES; ++i) {
    InCapConfig(i, 0, 0, 0, 0);
  }
}

void InCapConfig(int incap_num, int mode, int continouos, int clock_scale,
                 int input_scale) {
  volatile INCAP_REG* reg = incap_regs + incap_num;
  log_printf("InCapConfig(%d, %d, %d, %d, %d)", incap_num, mode, continouos,
             clock_scale, input_scale);
  Set_ICIE[incap_num](0);  // disable interrupts
  reg->con1 = 0x0000;      // disable module
  Set_ICIF[incap_num](0);  // clear interrupts
  Set_ICIP[incap_num](1);  // interrupt priority 1
  if (clock_scale) {
    static const int clkbits[3] = { 7 << 10, 0, 4 << 10 };
    static const int modebits[6] = { 3, 2, 3, 4, 5, 2 };
    
    reg->con2 = 0x0000;
    reg->con1 = clkbits[clock_scale - 1] | modebits[mode];
  }
}
