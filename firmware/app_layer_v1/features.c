/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */

#include "features.h"

#include <string.h>

#include "Compiler.h"
#include "pins.h"
#include "logging.h"
#include "protocol.h"
#include "adc.h"
#include "pwm.h"
#include "uart.h"
#include "spi.h"
#include "i2c.h"
#include "timers.h"
#include "pp_util.h"
#include "incap.h"

////////////////////////////////////////////////////////////////////////////////
// Pin modes
////////////////////////////////////////////////////////////////////////////////

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

static int PinIsMatrix(int i) {
  static const int MATRIX_PINS[] = { 7, 10, 11, 19, 20, 21, 22, 23, 24, 25, 27, 28 };
  int j;
  for (j = 0; j < ARRAY_LEN(MATRIX_PINS); ++j) {
    if (MATRIX_PINS[j] == i) return 1;
  }
  return 0;
}

static void PinsInit() {
  int i;
  _CNIE = 0;
  // reset pin states
  SetPinDigitalOut(0, 1, 1);  // LED pin: output, open-drain, high (off)
  for (i = 1; i < NUM_PINS; ++i) {
    //if (!PinIsMatrix(i)) {
      SetPinDigitalIn(i, 0);    // all other pins: input, no-pull
    //}
  }

  for (i = 0; i < NUM_UART_MODULES; ++i) {
    SetPinUart(0, i, 0, 0);  // UART RX disabled
  }
  // clear and enable global CN interrupts
  _CNIF = 0;
  _CNIE = 1;
  _CNIP = 1;  // CN interrupt priority is 1 so it can write an outgoing message
}

void SetPinDigitalOut(int pin, int value, int open_drain) {
  log_printf("SetPinDigitalOut(%d, %d, %d)", pin, value, open_drain);
  SAVE_PIN_FOR_LOG(pin);
  ADCSetScan(pin, 0);
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
  SAVE_PIN_FOR_LOG(pin);
  ADCSetScan(pin, 0);
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

void SetPinPwm(int pin, int pwm_num, int enable) {
  log_printf("SetPinPwm(%d, %d)", pin, pwm_num);
  SAVE_PIN_FOR_LOG(pin);
  PinSetRpor(pin, enable ? (pwm_num == 8 ? 35 : 18 + pwm_num) : 0);
}

void SetPinUart(int pin, int uart_num, int dir, int enable) {
  log_printf("SetPinUart(%d, %d, %d, %d)", pin, uart_num, dir, enable);
  SAVE_PIN_FOR_LOG(pin);
  SAVE_UART_FOR_LOG(uart_num);
  if (dir) {
    // TX
    const BYTE rp[] = { 3, 5, 28, 30 };
    PinSetRpor(pin, enable ? rp[uart_num] : 0);
  } else {
    // RX
    int rpin = enable ? PinToRpin(pin) : 0x3F;
    switch (uart_num) {
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
}

void SetPinInCap(int pin, int incap_num, int enable) {
  log_printf("SetPinInCap(%d, %d, %d)", pin, incap_num, enable);
    int rpin = enable ? PinToRpin(pin) : 0x3F;

    switch (incap_num) {
#define CASE(num, unused)    \
      case num - 1:          \
        _IC##num##R = rpin;  \
        break;
      REPEAT_1B(CASE, NUM_INCAP_MODULES);
    }
}

void SetPinAnalogIn(int pin) {
  log_printf("SetPinAnalogIn(%d)", pin);
  SAVE_PIN_FOR_LOG(pin);
  PinSetRpor(pin, 0);
  PinSetCnen(pin, 0);
  PinSetCnpu(pin, 0);
  PinSetCnpd(pin, 0);
  PinSetAnsel(pin, 1);
  PinSetTris(pin, 1);
  ADCSetScan(pin, 0);
}

void SetPinSpi(int pin, int spi_num, int mode, int enable) {
  log_printf("SetPinSpi(%d, %d, %d, %d)", pin, spi_num, mode, enable);
  SAVE_PIN_FOR_LOG(pin);
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
  TimersInit();
  PinsInit();
  PWMInit();
  ADCInit();
  UARTInit();
  SPIInit();
  I2CInit();
  InCapInit();
  PixelInit();


  // TODO: reset all peripherals!
  SRbits.IPL = ipl_backup;  // enable interrupts
}

void CheckInterface(BYTE interface_id[8]) {
  OUTGOING_MESSAGE msg;
  msg.type = CHECK_INTERFACE_RESPONSE;
  msg.args.check_interface_response.supported
      = (memcmp(interface_id, PROTOCOL_IID_IOIO0003, 8) == 0)
        || (memcmp(interface_id, PROTOCOL_IID_IOIO0002, 8) == 0)
        || (memcmp(interface_id, PROTOCOL_IID_IOIO0001, 8) == 0)
        || (memcmp(interface_id, PROTOCOL_IID_YTAI0001, 8) == 0)
        || (memcmp(interface_id, PROTOCOL_IID_YTAI0002, 8) == 0);
  AppProtocolSendMessage(&msg);
}

// BOOKMARK(add_feature): Add feature implementation.
