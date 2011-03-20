#include "pwm.h"

#include "Compiler.h"
#include "logging.h"
#include "board.h"

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

void SetPwmPeriod(int pwm_num, int period, int scale256) {
  volatile OC_REGS* regs;
  log_printf("SetPwmPeriod(%d, %d, %d)", pwm_num, period, scale256);
  regs = OC_REG(pwm_num);
  regs->con1 = 0x0000;
  if (period) {
    regs->r = 0;
    regs->rs = period;
    regs->con2 = 0x001F;
    regs->con1 = 0x0006 | (scale256 ? 0x1000 : 0x1C00);
  }
}
