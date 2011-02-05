#ifndef __FEATURES_H__
#define __FEATURES_H__

#include "GenericTypeDefs.h"

void SetPinDigitalOut(int pin, int value, int open_drain);
void SetDigitalOutLevel(int pin, int value);
void SetPinDigitalIn(int pin, int pull);
void SetPinAnalogIn(int pin);
void SetChangeNotify(int pin, int changeNotify);
void ReportDigitalInStatus(int pin);
void HardReset();
void SoftReset();
void SetPinPwm(int pin, int pwm_num);
void SetPwmDutyCycle(int pwm_num, int dc, int fraction);
void SetPwmPeriod(int pwm_num, int period, int scale256);

// BOOKMARK(add_feature): Add feature declaration.


#endif  // __FEATURES_H__
