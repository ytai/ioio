#ifndef __ADB_H__
#define __ADB_H__

#include "GenericTypeDefs.h"
#include "adb_types.h"

typedef int ADB_CHANNEL_HANDLE;
#define ADB_INVALID_CHANNEL_HANDLE (-1)
#define ADB_MAX_CHANNELS 8
#define ADB_CHANNEL_NAME_MAX_LENGTH 16

// call after every new attach
//void ADBReset();
//ADB_RESULT ADBResetStatus();
void ADBInit();
BOOL ADBConnected();
ADB_CHANNEL_HANDLE ADBOpen(const char* name);
BOOL ADBChannelReady(ADB_CHANNEL_HANDLE handle);
void ADBWrite(ADB_CHANNEL_HANDLE handle, const void* data, UINT32 data_len);
//ADB_RESULT ADBWriteStatus();
//void ADBRead(ADB_CHANNEL_HANDLE handle);
//ADB_RESULT ADBReadStatus(ADB_CHANNEL_HANDLE handle, void** data, UINT32* data_len);
void ADBTasks();


#endif  // __ADB_H__
