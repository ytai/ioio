// This module provides a single function that must be called periodically in
// order to provide context for all bootloader services.
// In particular, the ADB-related services need context in order to maintain
// communications.

#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#include "GenericTypeDefs.h"


// Needs to be called by the application periodically in order to provide
// context for the service provided by the bootloader API (BLAPI).
// Returns TRUE iff an ADB connection is established.
BOOL BootloaderTasks();


#endif  // __BOOTLOADER_H__
