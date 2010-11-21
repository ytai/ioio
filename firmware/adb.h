#ifndef __ADB_H__
#define __ADB_H__

#include "GenericTypeDefs.h"
#include "adb_types.h"

typedef int ADB_CHANNEL_HANDLE;

#define ADB_MAX_CHANNELS 8

// call after every new attach
//void ADBReset();
//ADB_RESULT ADBResetStatus();
void ADBInit();
void ADBOpen(const char* name);
ADB_RESULT ADBOpenStatus(ADB_CHANNEL_HANDLE* handle);
void ADBWrite(ADB_CHANNEL_HANDLE handle, const void* data, UINT32 data_len);
ADB_RESULT ADBWriteStatus();
void ADBRead(ADB_CHANNEL_HANDLE handle);
ADB_RESULT ADBReadStatus(ADB_CHANNEL_HANDLE handle, void** data, UINT32* data_len);
void ADBTasks();


#endif  // __ADB_H__
