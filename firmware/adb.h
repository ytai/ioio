#ifndef __ADB_H__
#define __ADB_H__

#include "GenericTypeDefs.h"  // For BOOL

// Default to 4096, but might change depending on device attached.
//extern const size_t ADB_MAX_PAYLOAD/* = 4096 */;
#define ADB_MAX_PAYLOAD 2048


void ADBTasks();
BOOL ADBConnect();




/**
 * Waits for an Android device to attach, and connects to it.
 * This is a blocking method, which will unblock only after a device got
 * attached.
 * This method will return FALSE in the following cases:
 *   The attached device is not an Android device.
 *   The attached device did not respond according to ADB protocol.
 */
BOOL connect();
/**
 * Returns true if there is an attached and connected device.
 * NOTE: This method will not actively check for attached and connected device,
 *   it will only return the state after the latest command.
 */
BOOL is_connected();

/**
 * Sends the data to the connected device.
 * This method will return FALSE in the following cases:
 *   data pointer is null, len is 0 or len is bigger than ADB_MAX_PAYLOAD.
 *   device got detached (In that case, calling is_connected() right after this
 *     method would return FALSE.
 *   sending data over USB couldn't complete of a different reason.
 */
BOOL send_data(BYTE* data, DWORD len);
/**
 * Recieve data from the connected device.
 * data pointer MUST be allocated and big enough to store the whole response.
 * This method will return FALSE in the following cases:
 *   data pointer is null.
 *   the message recieved does not correspond to ADB protocol.
 *   device got detached (In that case, calling is_connected() right after this
 *     method would return FALSE.
 *   recieving data from USB couldn't complete of a different reason.
 */
BOOL recv_data(BYTE* data, DWORD* len);

/**
 * Runs the refresh sequance of the USB state.
 * This method refreshes the state of is_connected() and responds similarly.
 */
BOOL refresh_device();

#endif  // __ADB_H__
