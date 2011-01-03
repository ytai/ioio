#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#include "GenericTypeDefs.h"


// Needs to be called by the application periodically in order to provide
// context for the service provided by the bootloader API (BLAPI).
// Returns TRUE iff an ADB connection is established.
BOOL BootloaderTasks();


#endif  // __BOOTLOADER_H__
