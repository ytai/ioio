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

// This file implements a simple ADB packet transfer mechanism on top of the
// USB layer.
// It enables sending / receiving packets to/from an ADB device. At any given
// time there can be up to one pending transmit and one pending receive.
// API is asynchronous: client issues a request and can then poll its status.
// Client should periodically call ADBPacketTasks() in order to provide context
// for this layer.
//
// Buffer ownership and lifetime:
// ------------------------------
// When writing, client is responsible to keep the data buffer (if there is one)
// alive until the write operation completes.
// When reading, the returned data buffer is not owned by the client, and is
// guaranteed to remain available until the next read operation is issued.
//
// Error handling:
// ---------------
// A failed read / write will eventually return a failure status code. This
// should include the case when the USB connection drops unexpectedly. However,
// in such circustances the client typically know about this directly from the
// USB layer and thus abort any ongoing operations.
// Once the connection is restored, this layer can be restored to its initial
// state by calling ADBPacketReset().
//
// Important note:
// ---------------
// The USB layer does not handle transmision of ROM buffers. Do not pass ROM
// buffers as packet data. When defining an array as const, the compiler
// typically stores it in ROM, so a possible workaround is to define it as non-
// const, even if it does not change.
//
// Send usage example:
// -------------------
// char hostname[] = "host::";
// ADBPacketReset();
// ...
// ADBPacketSend(ADB_CNXN, ADB_VERSION, ADB_PACKET_MAX_RECV_DATA_BYTES, hostname, sizeof hostname);
// do {
//   ADBPacketTasks();
// } while (ADBPacketSendStatus() == ADB_RESULT_BUSY);
//
// Receive usage example:
// ----------------------
// UINT32 cmd, arg0, arg1, data_len;
// void* data;
// ADBPacketReset();
// ...
// ADBPacketRecv();
// do {
//   ADBPacketTasks();
// } while (ADBPacketRecvStatus(&cmd, &arg0, &arg1, &data, &data_len) == ADB_RESULT_BUSY);
// ... do something with the command ...

#ifndef __ADBPACKET_H__
#define __ADBPACKET_H__

#include "GenericTypeDefs.h"
#include "adb_types.h"

////////////////////////////////////////////////////////////////////////////////
// The following code is the public API of this layer - should be used by the
// client.
////////////////////////////////////////////////////////////////////////////////

// This is the maximum data size, in bytes, that we can receive.
#define ADB_PACKET_MAX_RECV_DATA_BYTES 4096

// Issue a packet send.
// Send channel must not be busy: previous sends must have been completed.
// data must be kept valid by the client until the completion of the send.
// Client can check for completion and result code by calling ADBPacketSendStatus().
void ADBPacketSend(UINT32 cmd, UINT32 arg0, UINT32 arg1, const void* data, UINT32 data_len);

// Check the status of a previously issued send request.
// Will return either success, failure or busy codes.
ADB_RESULT ADBPacketSendStatus();

// Request that a packet is read. This operation will pend until a packet is sent
// by the remote side (or until the connection drops).
// Recieve channel must not be busy: previous receive requests must have been
// completed.
// Client can check for completion and result code by calling ADBPacketRecvStatus().
void ADBPacketRecv();

// Check the status of a previously issued receive request.
// Will return either success, failure or busy codes, as well as the actual data
// received in case of success.
// The returned data buffer (if exists) will remain valid until the next call to
// ADBPacketRecv().
ADB_RESULT ADBPacketRecvStatus(UINT32* cmd, UINT32* arg0, UINT32* arg1, void** data, UINT32* data_len);

// Reset the state of this module. To be called once after every establishment
// of a USB layer attachment.
void ADBPacketReset();

// Call this function periodically to provide context to this module.
// It does not block for long.
void ADBPacketTasks();


#endif  // __ADBPACKET_H__
