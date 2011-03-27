// This module provides a single function that must be called periodically in
// order to provide context for all bootloader services.
// In particular, the ADB-related services need context in order to maintain
// communications.

#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#include "GenericTypeDefs.h"

// The bootloader (BLAPI) version.
// TODO: top 8 bits are lost with this reading method.
extern __prog__ const WORD BootloaderVer;
#define BLAPI_VER_1 1

// The hardware version.
// TODO: top 8 bits are lost with this reading method.
extern __prog__ const WORD HardwareVer;
#define HARDWARE_VER_IOIO_V12 0
#define HARDWARE_VER_IOIO_V13 1


// Needs to be called by the application periodically in order to provide
// context for the service provided by the bootloader API (BLAPI).
// Returns TRUE iff an ADB connection is established.
BOOL BootloaderTasks();


#endif  // __BOOTLOADER_H__
