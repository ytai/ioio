// Private function of the ADB module.
// These functions are required for the operation of the ADB module, but are
// not directly exposed via BLAPI.
// ADBInit() must be called to initialize the ADB module.
// ADBTasks() must be called periodically to provide context to the ADB module,
// used for handling communications.

#ifndef __ADBPRIVATE_H__
#define __ADBPRIVATE_H__


#include "blapi/adb.h"

// Call this once at the start of the program.
void ADBInit();

// Call this periodically. Will not block for long.
// Returns TRUE if an ADB connection is established, FALSE otherwise. Once a
// connection drops, all open channels will be closed.
BOOL ADBTasks();


#endif  // __ADBPRIVATE_H__
