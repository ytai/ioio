//
// This file implements the ADB protocol (host side).
// It hides the underlying USB layer completely and exposes a simple API that
// enables the client to open several independant communication channels with
// the remote side. These communication channels can correspond to virtual TCP
// ports on the remote side, a command shell, etc. Each such communication
// channel is able to process a single buffer of outgoing data at a given moment
// as well as calls back a client-provided function as soon as incoming data
// arrives.
// The number of concurrently open channels is limited by ADB_MAX_CHANNELS.
//
// Closing of a channel:
// ---------------------
// In case the channel opening is rejected by the remote end or closed by it
// later, the client will be notified by calling the channel's callback function
// with a NULL data buffer and a length of 0.
// Client may close a channel by calling the ADBClose() function.
//
// Error handling:
// ---------------
// This module automatically attempts to establish a connection with the remote
// end as soon as USB layer is attached. When the connection drops, the
// connection will be dropped, and re-established when possible.
// The client should check the return code on every call to ADBTasks(). When the
// connection drops, all state is lost and any open channels are closed. They
// will be notified of the closure as mentioned in the above section.
// Upon detection of any sort of error or data corruption during connection,
// this module will reset the USB device and start over. Under normal operation
// this should never happen, so the client may want to reset everything and
// restart the program whenever a connection drops, but this is of course not
// required.
//
// Important note:
// ---------------
// The USB layer does not handle transmision of ROM buffers. Do not pass ROM
// buffers to ADBWrite(). When defining an array as const, the compiler
// typically stores it in ROM, so a possible workaround is to define it as non-
// const, even if it does not change.
//
// Usage example (error handling omitted):
// ---------------------------------------
// void ShellChannelCallback(ADB_CHANNEL_HANDLE h, void* data, UINT32 data_len) {
//   print(data, data_len);
// }
//
// ...
// ADBInit();  // call only once.
// while (!ADBTasks());
// h = ADBOpen("shell:", &ShellChannelCallback);
// while (!ADBChannelReady(h)) {
//   ADBTasks();
// }
// ADBWrite(h, data, sizeof data);
// while (1) {
//   ADBTasks();
// }
//

#ifndef __ADB_H__
#define __ADB_H__


#include "GenericTypeDefs.h"
#include "adb_types.h"

// A type designating a communication channel handle.
typedef int ADB_CHANNEL_HANDLE;

// An invalid channel handle - returned by an open attempt when the maximum
// number of open channels has been exceeded.
#define ADB_INVALID_CHANNEL_HANDLE (-1)

// The maximum amount of concurrent open channels allowed.
#define ADB_MAX_CHANNELS 8

// The maximum length of a channel name (include the trailing zero.
#define ADB_CHANNEL_NAME_MAX_LENGTH 16

// The signature of a channel incoming data callback.
// The h argument is useful in case the same function is used for several
// channels, but can be safely ignored otherwise.
// The data buffer is normally valid only until the callback exist.
// If the client needs to hang on to the buffer for a longer period, please see
// the ADBBufferRef() function documentation.
// When a channel is closed by the remote end (or its open is rejected), this
// function will be called with NULL data and 0 length.
typedef void (*ADBChannelRecvFunc)(ADB_CHANNEL_HANDLE h, const void* data, UINT32 data_len);

// Open a new channel to the remote end.
// The name indicates the destination on the remote end. Names such as
// "tcp:3823" or "shell:" can be used. Read the ADB spec for details.
// The callback passed will be invoked on every incoming data. See docuemntation
// of the callback signature above.
// The open request will result in either a "ready" state (ADBChannelReady()) or
// a close indication if refused by the remote end.
// An ADB_INVALID_CHANNEL_HANDLE value will be returned if the maximum number of
// concurrent channels is exceeded.
ADB_CHANNEL_HANDLE ADBOpen(const char* name, ADBChannelRecvFunc recv_func);

// Client may call this function from within the recieve callback in order to
// keep the data buffer alive after the return of the callback.
// Every call to this function must be paired with a single call to
// ADBBufferUnref() unless the connection has been lost.
// It is expected that ADBBufferUnref() will be called shortly after, since
// until it is called, no new data can be recieved on any channel.
// If the client needs to hold the received data for an extended period of
// time, a copy is recommended. 
void ADBBufferRef();

// Must be called once per every call to ADBBufferRef(). This lets the ADB
// layer no that the client no longer needs the data from the recieved buffer.
void ADBBufferUnref();

// Close a channel previously opened with ADBOpen().
// The actual close will happen shortly after calling this function, so a new
// ADBOpen() is not guaranteed to succeed if called immediately after ADBClose().
void ADBClose(ADB_CHANNEL_HANDLE handle);

// Check whether the channel is ready to transmit data.
// This also indicates a success in opening the channel.
BOOL ADBChannelReady(ADB_CHANNEL_HANDLE handle);

// Write data to the channel.
// The data must be kept valid until the channel becomes ready to send again.
// This will also indicate that the remote end acknoledged the reception of the
// message.
// Do not pass ROM buffers here, as they will silently fail.
void ADBWrite(ADB_CHANNEL_HANDLE handle, const void* data, UINT32 data_len);


#endif  // __ADB_H__
