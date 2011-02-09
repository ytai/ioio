#ifndef __PWM_H__
#define __PWM_H__


void PWMInit();
void SetPwmDutyCycle(int pwm_num, int dc, int fraction);
void SetPwmPeriod(int pwm_num, int period, int scale256);


#endif  // __PWM_H__
