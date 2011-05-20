/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */

//
// This module exposes an ADK channel (host side).
// An ADK channel allows sending data to and from and Android application over
// USB.
// At any given point in time, there may be at most one outstanding read and one
// outstanding write. The client must ensure that an operation completes before
// issuing the next one.
//
// Important note:
// ---------------
// The USB layer does not handle transmision of ROM buffers. Do not pass ROM
// buffers to ADKWrite(). When defining an array as const, the compiler
// typically stores it in ROM, so a possible workaround is to define it as non-
// const, even if it does not change.
//
// Usage example (error handling omitted):
// ---------------------------------------
// ...
// while (!BootloaderTasks());
// ADKWrite(buffer, 4);
// while (!ADKWriteDone(&error)) {
//   BootloaderTasks();
// }
// ADKRead(buffer, sizeof buffer);
// while (!ADKReadDone(&error, &bytesRead)) {
//   BootloaderTasks();
// }
// ... do something with buffer ...

#ifndef __ADK_H__
#define __ADK_H__


#include "GenericTypeDefs.h"

// Checks whether ADK-capable device is attached. If this returns FALSE, none of
// the other functions may be used.
BOOL ADKAttached();

// Issue a read request from the device.
// Actual read will be done asynchronously. Client should call
// ADKReadDone() to check for completion and get status code.
// Returns 0 if succeeded.
// Device must be attached.
BYTE ADKRead(void *buffer, DWORD length);

// Check whether the last call to ADKRead has completed.
// In case it is complete, returns TRUE, and the error code and number of bytes
// read are returned.
BOOL ADKReadDone(BYTE *errorCode, DWORD *byteCount);

// Issue a read request to the device.
// Actual write will be done asynchronously. Client should call
// ADKWriteDone() to check for completion and get status code.
// Returns 0 if succeeded.
// Device must be attached.
BYTE ADKWrite(const void *buffer, DWORD length);

// Check whether the last call to ADKWrite has completed.
// In case it is complete, returns TRUE, and the error code is returned.
BOOL ADKWriteDone(BYTE *errorCode);


#endif  // __ADK_H__
