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

// This module allows Adnroid file-system access on top of the ADB protocol.
// Currently, only reading of files is supported.
// Basic usage is as follows:
// - Client opens a file for read using ADBFileRead().
// - The client's callback gets called with the file's data being passed.
// - The callback is notified for EOF or error condition.
// Optionally, client may call ADBFileClose() for premature closing of the file.
//
// Example:
// void FileCallback(ADB_FILE_HANDLE f, const void* data, UINT32 data_len) {
//   if (data) {
//     print(data, data_len);
//   } else {
//     HandleEOF();
//   }
// }
//
// ...
// while (!BootloaderTasks());
// h = ADBFileOpen("/data/data/ioio.app/files/temp", &FileCallback);
// while (1) {
//   BootloaderTasks();
// }

#ifndef __ADBFILE_H__
#define __ADBFILE_H__


#include "adb.h"

// A type designating an open file handle.
typedef int ADB_FILE_HANDLE;

// An invalid file handle - returned by an open attempt when the maximum
// number of open files or channels has been exceeded.
#define ADB_FILE_INVALID_HANDLE (-1)

// Maximum concurrently open files.
#define ADB_FILE_MAX_FILES 2

// Maximum path length.
#define ADB_FILE_MAX_PATH_LENGTH 64

// The signature of a channel incoming data callback.
// The h argument is useful in case the same function is used for several
// channels, but can be safely ignored otherwise.
// The data buffer is normally valid until the callback exits. If the client
// needs this data to persist longer, ADBBufferRef() can be used.
// ADBBufferUnref() MUST be called shortly after, since until this happens, no
// new data can be received on any channel.
// When the data argument is NULL, check data_len:
// - When 0, the EOF has been reached.
// - When 1, an error has occured.
typedef void (*ADBFileRecvFunc)(ADB_FILE_HANDLE h, const void* data, UINT32 data_len);

// Opens a file for reading.
// The contents of the file will be streamed to the callback function.
// See documentation of the callback function for more information.
ADB_FILE_HANDLE ADBFileRead(const char* path, ADBFileRecvFunc recv_func);

// Prematurely close a file being read.
// After calling this function, the callback function will no longer be called.
void ADBFileClose(ADB_FILE_HANDLE h);


#endif  // __ADBFILE_H__
