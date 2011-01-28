#ifndef __PINS_H__
#define __PINS_H__

#define IOIO_V10


#if defined(IOIO_V10)
  #define NUM_PINS 50
#elif defined(IOIO_V11)
  #define NUM_PINS 49
#else
#error Unknown board
#endif

void PinSetTris(int pin, int val);
void PinSetLat(int pin, int val);
int PinGetPort(int pin);
void PinSetOdc(int pin, int val);

#endif  // __PINS_H__
