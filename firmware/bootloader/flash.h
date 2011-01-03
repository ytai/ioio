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
