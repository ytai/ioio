// Private function of the ADB File module.
// These functions are required for the operation of the ADB File module, but
// are not directly exposed via BLAPI.
// ADBFileInit() must be called to initialize the ADB File module.
// ADBFile Tasks() must be called periodically to provide context to the ADB
// File module, used for handling communications.

#ifndef __ADBFILEPRIVATE_H__
#define __ADBFILEPRIVATE_H__

#include "blapi/adb_file.h"


// Reset the state of this module.
// Should be called at least once before doing anything else.
void ADBFileInit();

// Call this function periodically in order to provide context to this module.
void ADBFileTasks();


#endif  // __ADBFILEPRIVATE_H__
