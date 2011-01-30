#ifndef __FEATURES_H__
#define __FEATURES_H__


void SetPinDigitalOut(int pin, int open_drain);
void SetDigitalOutLevel(int pin, int value);
void SetPinDigitalIn(int pin, int pull);
void SetChangeNotify(int pin, int changeNotify);
void ReportDigitalInStatus(int pin);
void HardReset();


#endif  // __FEATURES_H__
