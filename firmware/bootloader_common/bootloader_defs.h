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

#ifndef __BOOTLOADERDEFS_H__
#define __BOOTLOADERDEFS_H__

#include "GenericTypeDefs.h"
#include "board.h"

// Structure of the program (Flash) space:
// [0x0000                         : BOOTLOADER_MIN_APP_ADDRESS)          - Reserved for bootloader.
// [BOOTLOADER_MIN_APP_ADDRESS     : BOOTLOADER_MAX_APP_ADDRESS)          - Application code.
// [BOOTLOADER_FINGERPRINT_ADDRESS : BOOTLOADER_FINGERPRINT_ADDRESS + 16) - Fingerprint (low-words only).
// [BOOTLOADER_OSCTUN_ADDRESS      : BOOTLOADER_OSCTUN_ADDRESS      + 2)  - Osctun (low-byte only).


#define FORCEROM __attribute__((space(auto_psv)))

#define PAGE_SIZE 0x400
#define PAGE_START(address) ((address) & ~(PAGE_SIZE - 1))

#define BOOTLOADER_MIN_APP_ADDRESS 0x5000
// last page reseved for bootloader configuration
#define BOOTLOADER_MAX_APP_ADDRESS (PAGE_START(APP_PROGSPACE_END) - PAGE_SIZE)
#define BOOTLOADER_CONFIG_PAGE BOOTLOADER_MAX_APP_ADDRESS
#define BOOTLOADER_FINGERPRINT_ADDRESS BOOTLOADER_CONFIG_PAGE
#define BOOTLOADER_OSCTUN_ADDRESS (BOOTLOADER_FINGERPRINT_ADDRESS + 16)
#define BOOTLOADER_INVALID_ADDRESS ((DWORD) -1)

static const char manager_app_name[] = "ioio.manager";


#endif  // __BOOTLOADERDEFS_H__
