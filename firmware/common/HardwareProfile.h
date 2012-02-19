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

#ifndef _HARDWAREPROFILE_H_
#define _HARDWAREPROFILE_H_

#if defined(__PIC24FJ256DA206__) || defined(__PIC24FJ128DA106__) || defined(__PIC24FJ128DA206__)
  #include <p24Fxxxx.h>
  // Various clock values
  #define GetSystemClock()            32000000UL
  #define GetPeripheralClock()        (GetSystemClock())
  #define GetInstructionClock()       (GetSystemClock() / 2)

  // Define the baud rate constants
  #define BAUDRATE2       115200
  #define BRG_DIV2        4
  #define BRGH2           1

  // LEDS
  #define mInitAllLEDs()  {TRISFbits.TRISF3 = 0; LATFbits.LATF3 = 1;}
  #define mInitAllSwitches()
  #define mLED_0              LATFbits.LATF3
#else
  #error Unsupported target
#endif

#define mLED_0_On()         mLED_0  = 0;
#define mLED_0_Off()        mLED_0  = 1;
#define mLED_0_Toggle()     mLED_0  = !mLED_0;
    

#endif  

