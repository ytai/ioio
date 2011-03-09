// Synchronization utilities.

#ifndef __SYNC_H__
#define __SYNC_H__

#include "GenericTypeDefs.h"

// Disable interrupts at or below a certain level.
// Returns the previous interrupt state. Call again with the returned value in
// order to return to the previous state.
//
// Example:
// BYTE prev = SyncInterruptLevel(5);  // disable interrupt with priority <= 5
// ... do something critical ...
// SyncInterruptLevel(prev);  // return to previous state
static inline BYTE SyncInterruptLevel(BYTE level) {
    BYTE ret = SRbits.IPL;
    SRbits.IPL = level;
    return ret;
}


#endif  // __SYNC_H__
