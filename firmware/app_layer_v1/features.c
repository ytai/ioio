#include "features.h"

#include "Compiler.h"
#include "pins.h"
#include "logging.h"
#include "protocol.h"
#include "adc.h"
#include "pwm.h"
#include "uart.h"
#include "spi.h"

////////////////////////////////////////////////////////////////////////////////
// Pin modes
////////////////////////////////////////////////////////////////////////////////

static void PinsInit() {
  int i;
  _CNIE = 0;
  // reset pin states
  SetPinDigitalOut(0, 1, 1);  // LED pin: output, open-drain, high (off)
  for (i = 1; i < NUM_PINS; ++i) {
    SetPinDigitalIn(i, 0);    // all other pins: input, no-pull
  }
  for (i = 0; i < NUM_UART_MODULES; ++i) {
    SetPinUartRx(0, i, 0);
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
  PinSetRpor(pin, pwm_num == 0x0F ? 0 : (pwm_num == 8 ? 35 : 18 + pwm_num));
}

void SetPinUartRx(int pin, int uart, int enable) {
  log_printf("SetPinUartRx(%d, %d, %d)", pin, uart, enable);
  SAVE_PIN4_FOR_LOG();
  SAVE_UART1_FOR_LOG();
  int rpin = enable ? PinToRpin(pin) : 0x3F;
  switch (uart) {
    case 0:
      _U1RXR = rpin;
      break;

    case 1:
      _U2RXR = rpin;
      break;

    case 2:
      _U3RXR = rpin;
      break;

    case 3:
      _U4RXR = rpin;
      break;
  }
}

void SetPinUartTx(int pin, int uart, int enable) {
  log_printf("SetPinUartTx(%d, %d, %d)", pin, uart, enable);
  SAVE_PIN4_FOR_LOG();
  const BYTE rp[] = { 3, 5, 28, 30 };
  PinSetRpor(pin, enable ? rp[uart] : 0);
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

void SetPinSpi(int pin, int spi_num, int mode, int enable) {
  log_printf("SetPinSpi(%d, %d, %d, %d)", pin, spi_num, mode, enable);
  SAVE_PIN4_FOR_LOG();
  switch (mode) {
    case 0:  // data out
      {
        const BYTE rp[] = { 7, 10, 32 };
        PinSetRpor(pin, enable ? rp[spi_num] : 0);
      }
      break;

      case 1:  // data in
      {
        int rpin = enable ? PinToRpin(pin) : 0x3F;
        switch (spi_num) {
          case 0:
            _SDI1R = rpin;
            break;

          case 1:
            _SDI2R = rpin;
            break;

          case 2:
            _SDI3R = rpin;
            break;
        }
      }
      break;

      case 2:  // clk out
      {
        const BYTE rp[] = { 8, 11, 33 };
        PinSetRpor(pin, enable ? rp[spi_num] : 0);
      }
      break;
  }
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
  SRbits.IPL = 7;  // disable interrupts
  log_printf("SoftReset()");
  PinsInit();
  PWMInit();
  ADCInit();
  UARTInit();
  SPIInit();
  // TODO: reset all peripherals!
  SRbits.IPL = ipl_backup;  // enable interrupts
}

// BOOKMARK(add_feature): Add feature implementation.
