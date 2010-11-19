#ifndef __ADBPACKET_H__
#define __ADBPACKET_H__

#include "GenericTypeDefs.h"
#include "adb_types.h"

////////////////////////////////////////////////////////////////////////////////
// The following code is the public API of this layer - should be used by the
// client.
////////////////////////////////////////////////////////////////////////////////

#define ADB_PACKET_MAX_RECV_DATA_BYTES 4096

void ADBPacketSend(UINT32 cmd, UINT32 arg0, UINT32 arg1, const void* data, UINT32 data_len);
ADB_RESULT ADBPacketSendStatus();

void ADBPacketRecv();
ADB_RESULT ADBPacketRecvStatus(UINT32* cmd, UINT32* arg0, UINT32* arg1, void** data, UINT32* data_len);

void ADBPacketReset();

void ADBPacketTasks();




#endif  // __ADBPACKET_H__
