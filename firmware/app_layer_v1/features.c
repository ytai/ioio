#include "features.h"

#include "Compiler.h"
#include "pins.h"
#include "logging.h"
#include "protocol.h"
#include "adc.h"
#include "pwm.h"

////////////////////////////////////////////////////////////////////////////////
// Pin modes
////////////////////////////////////////////////////////////////////////////////

static void PinsInit() {
  int i;
  // reset pin states
  SetPinDigitalOut(0, 1, 1);  // LED pin: output, open-drain, high (off)
  for (i = 1; i < NUM_PINS; ++i) {
    SetPinDigitalIn(i, 0);    // all other pins: input, no-pull
  }
  // clear and enable global CN interrupts
  _CNIF = 0;
  _CNIE = 1;
  _CNIP = 1;  // CN interrupt priority is 1 so it can write an outgoing message
}

void SetPinDigitalOut(int pin, int value, int open_drain) {
  log_printf("SetPinDigitalOut(%d, %d, %d)", pin, value, open_drain);
  SAVE_PIN4_FOR_LOG();
  ADCClrScan(pin);
  PinSetAnsel(pin, 0);
  PinSetRpor(pin, 0);
  PinSetCnen(pin, 0);
  PinSetCnpu(pin, 0);
  PinSetCnpd(pin, 0);
  PinSetLat(pin, value);
  PinSetOdc(pin, open_drain);
  PinSetTris(pin, 0);
}

void SetPinDigitalIn(int pin, int pull) {
  log_printf("SetPinDigitalIn(%d, %d)", pin, pull);
  SAVE_PIN4_FOR_LOG();
  ADCClrScan(pin);
  PinSetAnsel(pin, 0);
  PinSetRpor(pin, 0);
  PinSetCnen(pin, 0);
  switch (pull) {
    case 1:
      PinSetCnpd(pin, 0);
      PinSetCnpu(pin, 1);
      break;

    case 2:
      PinSetCnpu(pin, 0);
      PinSetCnpd(pin, 1);
      break;

    default:
      PinSetCnpu(pin, 0);
      PinSetCnpd(pin, 0);
  }
  PinSetTris(pin, 1);
}

void SetPinPwm(int pin, int pwm_num) {
  log_printf("SetPinPwm(%d, %d)", pin, pwm_num);
  SAVE_PIN4_FOR_LOG();
  PinSetRpor(pin, pwm_num == 0 ? 0 : (pwm_num == 9 ? 35 : 17 + pwm_num));
}

void SetPinAnalogIn(int pin) {
  log_printf("SetPinAnalogIn(%d)", pin);
  SAVE_PIN4_FOR_LOG();
  PinSetRpor(pin, 0);
  PinSetCnen(pin, 0);
  PinSetCnpu(pin, 0);
  PinSetCnpd(pin, 0);
  PinSetAnsel(pin, 1);
  PinSetTris(pin, 1);
  ADCSetScan(pin);
}


////////////////////////////////////////////////////////////////////////////////
// Reset
////////////////////////////////////////////////////////////////////////////////

void HardReset() {
  log_printf("HardReset()");
  log_printf("Rebooting...");
  Reset();
}

void SoftReset() {
  BYTE ipl_backup = SRbits.IPL;
  log_printf("SoftReset()");
  // disable interrupts
  SRbits.IPL = 7;
  // initialize pins
  PinsInit();
  // initialize PWM
  PWMInit();
  // initialze ADC
  ADCInit();
  // TODO: reset all peripherals!
  SRbits.IPL = ipl_backup;
}

// BOOKMARK(add_feature): Add feature implementation.
