// IOIO application layer protocol implementation.
//
// Typical usage:
// ADB_CHANNEL h = ...
// AppProtocolInit(h);
// while (1) {
//   if (HasIncomingMessage()) {
//     AppProtocolHandleIncoming(msg_data, msg_size);
//   }
//   AppProtocolTasks(h);
// }
//
// TODO: decouple this module from ADB via callback functions for sending data.

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "blapi/adb.h"
#include "protocol_defs.h"

// Initialize this module.
// This function completely resets the module's state and can be called even
// after the module has been initialized in order to reset it.
// h is a channel handle, which will be used for sending outgoing protocol
// messages.
void AppProtocolInit(ADB_CHANNEL_HANDLE h);

// Call this function peridically for providing context to this module.
// h is a channel handle, which will be used for sending outgoing protocol
// messages.
void AppProtocolTasks(ADB_CHANNEL_HANDLE h);

// Process incoming protocol data.
// data may not be NULL.
BOOL AppProtocolHandleIncoming(const BYTE* data, UINT32 data_len);

// Send a protocol message.
// This is not intended for usage of the bootstrap code that glues the protocol
// to the underlying serial connection layer, but rather for use of modules
// implementing specific features of the protocol which involve producing output
// data.
void AppProtocolSendMessage(const OUTGOING_MESSAGE* msg);

// Send a protocol message with an attachment of variable-size arguments.
// As specified in the protocol specification, some message types may have a
// variable-size argument. This function is only intended for those message
// types.
void AppProtocolSendMessageWithVarArg(const OUTGOING_MESSAGE* msg,
                                      const void* data, int size);

#endif  // __PROTOCOL_H__
