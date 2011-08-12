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

// This module exposes basic read/write functionality for the program memory
// (aka. non-volative memory, or Flash).
// The Flash memory is comprised of 24-bit values.
// These values are divided into groups of 64, called "rows" or "blocks".
// The blocks are further divided into groups of 8, called "pages".
//
// In order to write a value to a Flash address, this address must first be
// erased. Erases can only be done on a full page.
// Writes can be done for a single value, or for a whole block (faster).
// Reads are always for a single value.
//
// Addressing:
// Each 24-bit value occupies 2 addresses (even, odd).

#ifndef __FLASH_H__
#define __FLASH_H__

#include "GenericTypeDefs.h"

// Erase a page of Flash memory.
// A page is 8 blocks, or 512 24-bit values (1536 bytes), and occupies an
// address range of 1024 (2 addresses for each value).
// The address must be 1024-aligned, i.e. 10 LSb's are 0 and within the valid
// range.
// The function returns TRUE if the operation succeeded.
BOOL FlashErasePage(DWORD address);

// Write a single 24-bit value to flash.
// The top 8-bits of the value passed are ignored.
// The address must be even, i.e. top bit is 0 and within the valid
// range.
// The written value will occupy a range of 2 addresses.
// It is assumed that the address has previously been erased using
// FlashErasePage().
// The function returns TRUE if the operation succeeded.
BOOL FlashWriteDWORD(DWORD address, DWORD value);

// Write a block of data to flash.
// A block contains 64 24-bit values (192 bytes), and occupies an address range
// of 128 (since every 24-bit values occupies a range of 2 addresses).
// The address must be 128-aligned, i.e. 7 LSb's are 0 and within the valid
// range.
// It is assumed that the address range has previously been erased using
// FlashErasePage().
// The function returns TRUE if the operation succeeded.
BOOL FlashWriteBlock(DWORD address, BYTE block[192]);

// Read a single 24-bit value from Flash.
// The top 8-bits of the returned value are always 0.
// The address must be even, i.e. top bit is 0. The read value will occupy a
// range of 2 addresses.
DWORD FlashReadDWORD(DWORD address);


#endif  // __FLASH_H__
