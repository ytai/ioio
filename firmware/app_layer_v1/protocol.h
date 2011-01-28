#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "blapi/adb.h"

void AppProtocolInit(ADB_CHANNEL_HANDLE h);
void AppProtocolTasks(ADB_CHANNEL_HANDLE h);
void AppProtocolHandleIncoming(const BYTE* data, UINT32 data_len);

#endif  // __PROTOCOL_H__
