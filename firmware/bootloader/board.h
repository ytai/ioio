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

// Board implementation designators.

#ifndef __BOARD_H__
#define __BOARD_H__

#define BOARD_SPRK_BASE 1000  // base number for SparkFun's boards
#define BOARD_SPRK0010 BOARD_SPRK_BASE + 10
#define BOARD_SPRK0011 BOARD_SPRK_BASE + 11
#define BOARD_SPRK0012 BOARD_SPRK_BASE + 12
#define BOARD_SPRK0013 BOARD_SPRK_BASE + 13
#define BOARD_SPRK0014 BOARD_SPRK_BASE + 14
#define BOARD_SPRK0015 BOARD_SPRK_BASE + 15
#define BOARD_SPRK0016 BOARD_SPRK_BASE + 16

#define BOARD_MINT_BASE 2000  // base number for IOIOMint boards
#define BOARD_MINT0010 BOARD_MINT_BASE + 10

// add more boards here!

#ifndef BOARD_VER
#error Must define BOARD_VER
#endif

// sanity assert MCU
#if BOARD_VER >= BOARD_SPRK0010 && BOARD_VER <= BOARD_SPRK0012
  #ifndef __PIC24FJ128DA106__
    #error Board and MCU mismatch - expecting PIC24FJ128DA106
  #endif
#elif BOARD_VER >= BOARD_SPRK0013 && BOARD_VER <= BOARD_SPRK0015
  #ifndef __PIC24FJ128DA206__
    #error Board and MCU mismatch - expecting PIC24FJ128DA206
  #endif
#elif BOARD_VER == BOARD_SPRK0016
  #ifndef __PIC24FJ256DA206__
    #error Board and MCU mismatch - expecting PIC24FJ256DA206
  #endif
#elif BOARD_VER == BOARD_MINT0010
  #ifndef __PIC24FJ256DA206__
    #error Board and MCU mismatch - expecting PIC24FJ256DA206
  #endif
#else
  #error Unknown board
#endif

// hardware implementation versions
// Each version is an 8-byte ASCII string, comprised of 4 bytes authrity
// followed by 4 bytes revision.
#if BOARD_VER == BOARD_SPRK0010
  #define HW_IMPL_VER "SPRK0010"
#elif BOARD_VER == BOARD_SPRK0011
  #define HW_IMPL_VER "SPRK0011"
#elif BOARD_VER == BOARD_SPRK0012
  #define HW_IMPL_VER "SPRK0012"
#elif BOARD_VER == BOARD_SPRK0013
  #define HW_IMPL_VER "SPRK0013"
#elif BOARD_VER == BOARD_SPRK0014
  #define HW_IMPL_VER "SPRK0014"
#elif BOARD_VER == BOARD_SPRK0015
  #define HW_IMPL_VER "SPRK0015"
#elif BOARD_VER == BOARD_SPRK0016
  #define HW_IMPL_VER "SPRK0016"
#elif BOARD_VER == BOARD_MINT0010
  #define HW_IMPL_VER "MINT0010"
#else
  #error Unknown board
#endif

// This point to the beginning of the config word page, not writable by the app.
#if defined(__PIC24FJ256DA206__)
  #define APP_PROGSPACE_END 0x2A800
#elif defined(__PIC24FJ128DA106__) || defined(__PIC24FJ128DA206__)
  #define APP_PROGSPACE_END 0x15400
#else
  #error Unknown MCU
#endif

#endif  // __BOARD_H__
