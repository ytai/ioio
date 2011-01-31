#ifndef __FEATURES_H__
#define __FEATURES_H__

#include "GenericTypeDefs.h"

void SetPinDigitalOut(int pin, int value, int open_drain);
void SetDigitalOutLevel(int pin, int value);
void SetPinDigitalIn(int pin, int pull);
void SetChangeNotify(int pin, int changeNotify);
void ReportDigitalInStatus(int pin);
void HardReset(DWORD magic);
void SoftReset();

// BOOKMARK(add_feature): Add feature declaration.


#endif  // __FEATURES_H__
