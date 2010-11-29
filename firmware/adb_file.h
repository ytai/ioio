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

ADB_FILE_HANDLE ADBFileRead(const char* path, ADBChannelRecvFunc recv_func);
void ADBFileInit();
void ADBFileTasks();


#endif  // __ADBFILE_H__
