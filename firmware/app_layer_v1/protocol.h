#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "blapi/adb.h"
#include "protocol_defs.h"

void AppProtocolInit(ADB_CHANNEL_HANDLE h);
void AppProtocolTasks(ADB_CHANNEL_HANDLE h);
BOOL AppProtocolHandleIncoming(const BYTE* data, UINT32 data_len);
void AppProtocolSendMessage(const OUTGOING_MESSAGE* msg);
void AppProtocolSendMessageWithVarArg(const OUTGOING_MESSAGE* msg, const void* data, int size);

#endif  // __PROTOCOL_H__
