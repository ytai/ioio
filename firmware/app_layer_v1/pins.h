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
void PinSetCnen(int pin, int cnen);
void PinSetCnpu(int pin, int cnpu);
void PinSetCnpd(int pin, int cnpd);

#endif  // __PINS_H__
