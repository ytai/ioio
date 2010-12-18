#ifndef __BOOTLOADERDEFS_H__
#define __BOOTLOADERDEFS_H__

#include "GenericTypeDefs.h"

// attribute for variables that can be recycled after bootloader terminates,
// i.e. located in application data space.
#define BOOTDATA __attribute__((section("bootdata")))

#define BOOTLOADER_MIN_ADDRESS 0x8000
#define BOOTLOADER_MAX_ADDRESS 0x2A800
#define BOOTLOADER_INVALID_ADDRESS ((DWORD) -1)


#endif  // __BOOTLOADERDEFS_H__
