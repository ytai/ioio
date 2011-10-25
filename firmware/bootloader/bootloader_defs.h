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

#define FORCEROM __attribute__((space(auto_psv)))

#define BOOTLOADER_MIN_APP_ADDRESS 0x4000
#define BOOTLOADER_MAX_APP_ADDRESS (APP_PROGSPACE_END - 0x100)  // last page reseved for fingerprint
#define BOOTLOADER_FINGERPRINT_ADDRESS BOOTLOADER_MAX_APP_ADDRESS
#define BOOTLOADER_FINGERPRINT_PAGE (BOOTLOADER_FINGERPRINT_ADDRESS & 0xFFFFF400)
#define BOOTLOADER_INVALID_ADDRESS ((DWORD) -1)

static const char manager_app_name[] = "ioio.manager";


#endif  // __BOOTLOADERDEFS_H__
