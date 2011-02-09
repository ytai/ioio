#ifndef __DIGITAL_H__
#define __DIGITAL_H__


void SetDigitalOutLevel(int pin, int value);
void SetChangeNotify(int pin, int changeNotify);
void ReportDigitalInStatus(int pin);


#endif  // __DIGITAL_H__
