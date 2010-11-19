#ifndef __ADB_H__
#define __ADB_H__

#include "GenericTypeDefs.h"  // For BOOL

typedef UINT32 ADB_CHANNEL_HANDLE;

// Waits for an Android device to attach, and connects to it.
// This is a blocking method, which will unblock only after a device got
// attached.
// This method will return FALSE in the following cases:
// - The attached device is not an Android device.
// - The attached device did not respond according to ADB protocol.
BOOL ADBConnect();

// Returns true if there is an attached and connected device.
// NOTE: This method will not actively check for attached and connected device,
//       it will only return the state after the latest command.
BOOL is_connected();

// Sends the data to the connected device.
// This method will return FALSE in the following cases:
// - data pointer is null, len is 0 or len is bigger than ADB_MAX_PAYLOAD.
// - device got detached (In that case, calling is_connected() right after this
//   method would return FALSE.
// - sending data over USB couldn't complete of a different reason.
BOOL send_data(BYTE* data, DWORD len);

// Recieve data from the connected device.
// data pointer MUST be allocated and big enough to store the whole response.
// This method will return FALSE in the following cases:
// -  data pointer is null.
// -  the message recieved does not correspond to ADB protocol.
// -  device got detached (In that case, calling is_connected() right after this
// -  method would return FALSE.
// -  recieving data from USB couldn't complete of a different reason.
BOOL recv_data(BYTE* data, DWORD* len);

// Runs the refresh sequance of the USB state.
// This method refreshes the state of is_connected() and responds similarly.
BOOL refresh_device();

#define ADB_MAX_CHANNELS 8

// call after every new attach
void ADBReset();
ADB_RESULT ADBResetStatus();
void ADBOpen(const char* name);
ADB_RESULT ADBOpenStatus(ADB_CHANNEL_HANDLE* handle);
void ADBWrite(ADB_CHANNEL_HANDLE handle, const void* data, UINT32 data_len);
ADB_RESULT ADBWriteStatus();
void ADBRead(ADB_CHANNEL_HANDLE handle);
ADB_RESULT ADBReadStatus(ADB_CHANNEL_HANDLE handle, void** data, UINT32* data_len);
void ADBTasks();


#endif  // __ADB_H__
