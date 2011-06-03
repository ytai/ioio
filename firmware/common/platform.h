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

// Capabilities and features of specific platforms.
// A platform is a combination of a hardaware interface and bootloader ABI.
//
// Provides:
// NUM_PINS         - The number of physical pins on the board, including the
//                    on-board LED.
// NUM_PWM_MODULES  - The number of available PWM modules.
// NUM_UART_MODULES - The number of available UART modules.
// NUM_SPI_MODULES  - The number of available SPI modules.
// NUM_I2C_MODULES  - The number of available I2C modules.

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#define PLATFORM_IOIO_BASE 1000 // base platform number for 'official' IOIO platforms
#define PLATFORM_IOIO0000 PLATFORM_IOIO_BASE + 0
#define PLATFORM_IOIO0001 PLATFORM_IOIO_BASE + 1
#define PLATFORM_IOIO0002 PLATFORM_IOIO_BASE + 2
#define PLATFORM_IOIO0003 PLATFORM_IOIO_BASE + 3
#define PLATFORM_IOIO0010 PLATFORM_IOIO_BASE + 10
#define PLATFORM_IOIO0011 PLATFORM_IOIO_BASE + 11
#define PLATFORM_IOIO0012 PLATFORM_IOIO_BASE + 12
#define PLATFORM_IOIO0013 PLATFORM_IOIO_BASE + 13
// add more platforms here!

#ifndef PLATFORM
#error Must define PLATFORM
#endif

// sanity: assert MCU
#if PLATFORM == PLATFORM_IOIO0000
  #ifndef __PIC24FJ256DA206__
    #error Platform and MCU mismatch - expecting PIC24FJ256DA206
  #endif
#elif PLATFORM == PLATFORM_IOIO0001
  #ifndef __PIC24FJ128DA106__
    #error Platform and MCU mismatch - expecting PIC24FJ128DA106
  #endif
#elif PLATFORM == PLATFORM_IOIO0002
  #ifndef __PIC24FJ128DA206__
    #error Platform and MCU mismatch - expecting PIC24FJ128DA206
  #endif
#elif PLATFORM == PLATFORM_IOIO0003
  #ifndef __PIC24FJ256DA206__
    #error Platform and MCU mismatch - expecting PIC24FJ256DA206
  #endif
#elif PLATFORM == PLATFORM_IOIO0010
  #ifndef __PIC24FJ256DA206__
    #error Platform and MCU mismatch - expecting PIC24FJ256DA206
  #endif
#elif PLATFORM == PLATFORM_IOIO0011
  #ifndef __PIC24FJ128DA106__
    #error Platform and MCU mismatch - expecting PIC24FJ128DA106
  #endif
#elif PLATFORM == PLATFORM_IOIO0012
  #ifndef __PIC24FJ128DA206__
    #error Platform and MCU mismatch - expecting PIC24FJ128DA206
  #endif
#elif PLATFORM == PLATFORM_IOIO0013
  #ifndef __PIC24FJ256DA206__
    #error Platform and MCU mismatch - expecting PIC24FJ256DA206
  #endif
#else
  #error Unknown platform
#endif

// number of pins of each platform
#if PLATFORM == PLATFORM_IOIO0000
  #define NUM_PINS 50
#elif PLATFORM >= PLATFORM_IOIO0001 && PLATFORM <= PLATFORM_IOIO0003
  #define NUM_PINS 49
#elif PLATFORM == PLATFORM_IOIO0010
  #define NUM_PINS 50
#elif PLATFORM >= PLATFORM_IOIO0011 && PLATFORM <= PLATFORM_IOIO0013
  #define NUM_PINS 49
#else
  #error Unknown platform
#endif

#if defined(__PIC24FJ256DA206__) || defined(__PIC24FJ128DA106__) || defined(__PIC24FJ128DA206__)
  #define NUM_PWM_MODULES 9
  #define NUM_UART_MODULES 4
  #define NUM_SPI_MODULES 3
  #define NUM_I2C_MODULES 3
  #define NUM_INCAP_MODULES 9
#else
  #error Unknown MCU
#endif


#endif  // __PLATFORM_H__
