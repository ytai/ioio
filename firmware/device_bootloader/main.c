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

// Bootloader main

#include <string.h>
#include "HardwareProfile.h"
#include <libpic30.h>
#include "GenericTypeDefs.h"
#include "board.h"
#include "boot_protocol.h"
#include "bootloader_defs.h"
#include "flash.h"
#include "logging.h"
#include "ioio_file.h"
#include "USB/usb_common.h"
#include "USB/usb_function_cdc.h"
#include "USB/usb_device.h"

// Desired behavior:
// 1. Read the status of the "boot" pin (LED). If high, skip the bootloader
//    and run the app.
// 2. Wait for the "boot" pin to go high.
// 3. Initialize USB device.
// 4. Wait for an incoming connection and run the bootloader protocol. Repeat
//    forever (although one of the protocol commands forces a reset, which will
//    kick us out).


////////////////////////////////////////////////////////////////////////////////
// Configuration Bits
#if defined(__PIC24FJ256GB206__)
  _CONFIG1(FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
  _CONFIG2(POSCMOD_NONE & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL96MHZ_ON & PLLDIV_NODIV & IESO_OFF)
  _CONFIG3(WPDIS_WPEN & WPFP_WPFP19 & WPCFG_WPCFGEN & WPEND_WPSTARTMEM & SOSCSEL_EC)
#else
  #error Unsupported target
#endif


// When IOIO gets reset for an unexpected reason, its default behavior is to
// restart normally. This is desired in most cases.
// For debug purposes, however, we want to know what went wrong. With defining
// SIGANAL_AFTER_BAD_RESET, on any reset other than power-on, IOIO will blink
// a 16-bit code (long-1 short-0) to designtae the error.
// One very useful case is catching failed asserts. Their blink code will start
// with short-long-...
#ifdef SIGNAL_AFTER_BAD_RESET
static void SignalBit(int bit) {
  led_on();
  __delay_ms(bit ? 900 : 100);
  led_off();
  __delay_ms(bit ? 100 : 900);
}

static void SignalWord(unsigned int word) {
  int i;
  for (i = 0; i < 16; ++i) {
    SignalBit(word & 0x8000);
    word <<= 1;
  }
}

static void SignalRcon() {
  log_printf("RCON = 0x%x", RCON);
  while (1) {
   SignalWord(RCON);
   Delay(8000000UL);
  }
}
#endif

static void StartBlink() {
  int i = 5;
  while (i-- > 0) {
    led_on();
    __delay_ms(100);
    led_off();
    __delay_ms(200);
  }
}


int main() {
  // First thing: if "boot" is not grounded, go immediately to app.
  if (led_read()) {
    __asm__("goto __APP_RESET");
  }
  // We need to enter bootloader mode, wait for the boot pin to be released.
  while (!led_read());

  // Now we can start!
  led_init();
  log_init();
#ifdef SIGNAL_AFTER_BAD_RESET
  if (RCON & 0b1100001001000000) {
    SignalRcon();
  }
#endif
  StartBlink();

  log_printf("Hello from Bootloader!!!");
  USBInitialize();

  while (1) {
    // Wait for connection
    while (!(USBGetDeviceState() == CONFIGURED_STATE
      && CDCIsDtePresent())) USBTasks();

    log_printf("Connected!");
    BootProtocolInit();

    while (USBGetDeviceState() == CONFIGURED_STATE && CDCIsDtePresent()) {
      static char in_buf[64];
      USBTasks();
      BYTE size = getsUSBUSART(in_buf, sizeof(in_buf));
      if (!BootProtocolProcess(in_buf, size)) {
        log_printf("Protocol error. Will detach / re-attach.");
        USBSoftDetach();
        __delay_ms(2000);
        USBDeviceAttach();
        break;
      }
      BootProtocolTasks();
    }
    log_printf("Disconnected!");
  }
  return 0;
}
