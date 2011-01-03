#ifndef __ADBPRIVATE_H__
#define __ADBPRIVATE_H__


#include "adb.h"

// Call this once at the start of the program.
void ADBInit();

// Check whether an ADB connection has been established. Should be checked after
// every call to ADBTasks() in order to detect connection dropouts, since in
// such an event all open channels cease to be valid.
BOOL ADBConnected();

// Call this periodically. Will not block for long.
// Returns TRUE if an ADB connection is established, FALSE otherwise. Once a
// connection drops, all open channels will be closed.
BOOL ADBTasks();


#endif  // __ADBPRIVATE_H__
