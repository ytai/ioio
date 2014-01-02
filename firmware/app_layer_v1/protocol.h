/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */

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

#include "libconn/connection.h"
#include "protocol_defs.h"

// Human-readable string describing app firmware version.
#define FW_IMPL_VER "PIXL0003"

// Initialize this module.
// This function completely resets the module's state and can be called even
// after the module has been initialized in order to reset it.
// h is a channel handle, which will be used for sending outgoing protocol
// messages.
void AppProtocolInit(CHANNEL_HANDLE h);

// Call this function peridically for providing context to this module.
// h is a channel handle, which will be used for sending outgoing protocol
// messages.
void AppProtocolTasks(CHANNEL_HANDLE h);

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

// The same as AppProtocolSendMessageWithVarArg(), but with the argument split
// in two.
void AppProtocolSendMessageWithVarArgSplit(const OUTGOING_MESSAGE* msg,
                                          const void* data1, int size1,
                                          const void* data2, int size2);

#endif  // __PROTOCOL_H__
