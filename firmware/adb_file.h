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

#define ADB_FILE_MAX_PATH_LENGTH 64

// The signature of a channel incoming data callback.
// The h argument is useful in case the same function is used for several
// channels, but can be safely ignored otherwise.
// The data buffer is valid until the client calls ADBReleaseBuffer().
// This function MUST be called from within the callback or shortly after
// whenever the 'data' argument is non-NULL. Until this happens, no new data
// can be received on any channel.
// When the data argument is NULL, check data_len:
// - When 0, the EOF has been reached.
// - When 1, an error has occured.
typedef void (*ADBFileRecvFunc)(ADB_FILE_HANDLE h, const void* data, UINT32 data_len);

ADB_FILE_HANDLE ADBFileRead(const char* path, ADBFileRecvFunc recv_func);
void ADBFileInit();
void ADBFileTasks();


#endif  // __ADBFILE_H__
