#ifndef __FEATURES_H__
#define __FEATURES_H__

#include "GenericTypeDefs.h"

void SetPinDigitalOut(int pin, int value, int open_drain);
void SetDigitalOutLevel(int pin, int value);
void SetPinDigitalIn(int pin, int pull);
void SetChangeNotify(int pin, int changeNotify);
void ReportDigitalInStatus(int pin);
void HardReset();
void SoftReset();
void SetPinPwm(int pin, int pwmNum);
void SetPwmDutyCycle(int pwmNum, int dc, int fraction);
void SetPwmPeriod(int pwmNum, int period, int scale256);

// BOOKMARK(add_feature): Add feature declaration.


#endif  // __FEATURES_H__
