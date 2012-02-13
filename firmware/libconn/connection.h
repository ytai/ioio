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

// This module provides functions for ongoing operation of the connection.
// ConnectionInit() must be called once to reset the state.
// ConnectionTasks() must be called periodically in order to provide context
// for all connection services. In particular, the ADB-related services need
// context in order to maintain communications.

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "GenericTypeDefs.h"

typedef int CHANNEL_HANDLE;

#define INVALID_CHANNEL_HANDLE (-1)

typedef enum {
  CHANNEL_TYPE_ADB,
  CHANNEL_TYPE_ACC,
  CHANNEL_TYPE_BT
} CHANNEL_TYPE;

// data != NULL -> incoming data
// data = NULL, size = 0 -> Closed normally
// data = NULL, size = 1 -> Closed as result of error
//
// client should ignore ch. it is not guaranteed to be related to the actual
// channel handle
typedef void (*ChannelCallback) (CHANNEL_HANDLE ch, const void* data,
                                 UINT32 size);

// Reset the state of all connection modules.
void ConnectionInit();

// Needs to be called by the application periodically in order to provide
// context for the service provided by the connection library.
// Returns TRUE iff a USB device is connected.
BOOL ConnectionTasks();

// Resets USB connection. Connection will be dropped and then re-established.
void ConnectionResetUSB();

BOOL ConnectionTypeSupported(CHANNEL_TYPE con);
BOOL ConnectionCanOpenChannel(CHANNEL_TYPE con);
CHANNEL_HANDLE ConnectionOpenChannelAdb(const char *name, ChannelCallback cb);
CHANNEL_HANDLE ConnectionOpenChannelBtServer(ChannelCallback cb);
CHANNEL_HANDLE ConnectionOpenChannelAccessory(ChannelCallback cb);
void ConnectionSend(CHANNEL_HANDLE ch, const void *data, int size);
BOOL ConnectionCanSend(CHANNEL_HANDLE ch);
void ConnectionCloseChannel(CHANNEL_HANDLE ch);
int ConnectionGetMaxPacket(CHANNEL_HANDLE ch);


#endif  // __CONNECTION_H__
