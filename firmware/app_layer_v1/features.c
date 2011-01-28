#include "features.h"

#include "pins.h"
#include "logging.h"

void SetPinDigitalOut(int pin, int open_drain) {
  log_printf("SetPinDigitalOut(%d, %d)", pin, open_drain);
  PinSetLat(pin, 0);
  PinSetOdc(pin, open_drain);
  PinSetTris(pin, 0);
}

void SetDigitalOutLevel(int pin, int value) {
  log_printf("SetDigitalOutLevel(%d, %d)", pin, value);
  PinSetLat(pin, value);
}
