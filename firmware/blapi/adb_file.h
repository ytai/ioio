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
//   BotloaderTasks();
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
