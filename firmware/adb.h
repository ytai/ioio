#ifndef __ADB_H__
#define __ADB_H__

#include "GenericTypeDefs.h"
#include "adb_types.h"

typedef int ADB_CHANNEL_HANDLE;
#define ADB_INVALID_CHANNEL_HANDLE (-1)
#define ADB_MAX_CHANNELS 8
#define ADB_CHANNEL_NAME_MAX_LENGTH 16

void ChannelRecv(ADB_CHANNEL_HANDLE h, const void* data, UINT32 data_len);
#define ADBChannelRecv(h, data, len) ChannelRecv(h, data, len)

#ifndef ADBChannelRecv
#define ADBChannelRecv(h, data, len)
#endif

void ADBInit();
BOOL ADBConnected();
ADB_CHANNEL_HANDLE ADBOpen(const char* name);
BOOL ADBChannelReady(ADB_CHANNEL_HANDLE handle);
void ADBWrite(ADB_CHANNEL_HANDLE handle, const void* data, UINT32 data_len);
void ADBTasks();


#endif  // __ADB_H__
