#ifndef __FEATURES_H__
#define __FEATURES_H__

#include "GenericTypeDefs.h"

void SetPinDigitalOut(int pin, int open_drain);
void SetDigitalOutLevel(int pin, int value);
void SetPinDigitalIn(int pin, int pull);
void SetChangeNotify(int pin, int changeNotify);
void ReportDigitalInStatus(int pin);
void HardReset(DWORD magic);


#endif  // __FEATURES_H__
