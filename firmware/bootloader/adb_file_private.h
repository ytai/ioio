#ifndef __ADBFILEPRIVATE_H__
#define __ADBFILEPRIVATE_H__

#include "blapi/adb_file.h"


// Reset the state of this module.
// Should be called at least once before doing anything else.
void ADBFileInit();

// Call this function periodically in order to provide context to this module.
void ADBFileTasks();


#endif  // __ADBFILEPRIVATE_H__
