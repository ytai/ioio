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

#include "pwm.h"

#include "Compiler.h"
#include "logging.h"
#include "platform.h"

typedef struct {
  unsigned int con1;
  unsigned int con2;
  unsigned int rs;
  unsigned int r;
  unsigned int tmr;
} OC_REGS;

#define OC_REG(num) (((volatile OC_REGS *) &OC1CON1) + num)

void PWMInit() {
  int i;
  // disable PWMs
  for (i = 0; i < NUM_PWM_MODULES; ++i) {
    SetPwmPeriod(i, 0, 0);
  }
}

void SetPwmDutyCycle(int pwm_num, int dc, int fraction) {
  volatile OC_REGS* regs;
  log_printf("SetPwmDutyCycle(%d, %d, %d)", pwm_num, dc, fraction);
  regs = OC_REG(pwm_num);
  regs->con2 &= ~0x0600;
  regs->con2 |= fraction << 9;
  regs->r = dc;
}

void SetPwmPeriod(int pwm_num, int period, int scale) {
  volatile OC_REGS* regs;
  log_printf("SetPwmPeriod(%d, %d, %d)", pwm_num, period, scale);
  regs = OC_REG(pwm_num);
  regs->con1 = 0x0000;
  if (period) {
    static const int CLK_SRC[] = {
      0x1C00, // 1x   - system clk
      0x0C00, // 256x - timer 5
      0x0800, // 64x  - timer 4
      0x0400, // 8x   - timer 3
    };
    regs->r = 0;
    regs->rs = period;
    regs->con2 = 0x001F;
    regs->con1 = 0x0006 | CLK_SRC[scale];
  }
}
