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

#include "connection.h"

#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "connection_private.h"
#include "USB/usb_common.h"
#include "logging.h"
#include "bt_connection.h"
#include "adb_connection.h"
#include "accessory_connection.h"
#include "cdc_connection.h"

#define BUF_SIZE 1024
static uint8_t buf[BUF_SIZE];  // shared between Bluetooth and Accessory, as
                               // they are mutually exclusive. ADB currently
                               // conveniently ignores this.

static const CONNECTION_FACTORY *factories[CHANNEL_TYPE_MAX] = {
  &adb_connection_factory,
  &accessory_connection_factory,
  &bt_connection_factory,
  &cdc_connection_factory
};

void ConnectionInit() {
  int i;
  USBInitialize();
  for (i = 0; i < CHANNEL_TYPE_MAX; ++i) {
    factories[i]->init(buf, BUF_SIZE);
  }
}

void ConnectionTasks() {
  int i;
  USBTasks();
  for (i = 0; i < CHANNEL_TYPE_MAX; ++i) {
    factories[i]->tasks();
  }
}

void ConnectionShutdownAll() {
  USBShutdown();
}

BOOL ConnectionTypeSupported(CHANNEL_TYPE con) {
  return factories[con]->isAvailable();
}

BOOL ConnectionCanOpenChannel(CHANNEL_TYPE con) {
  return factories[con]->isReadyToOpen();
}

static CHANNEL_HANDLE ConnectionOpenChannel(CHANNEL_TYPE t,
                                            ChannelCallback cb,
                                            int_or_ptr_t open_arg,
                                            int_or_ptr_t cb_arg) {
  int h = factories[t]->connectionOpen(cb, open_arg, cb_arg);
  assert(!(h & 0xF000));
  return (t << 12) | h;
}

CHANNEL_HANDLE ConnectionOpenChannelAdb(const char *name, ChannelCallback cb,
                                        int_or_ptr_t cb_arg) {
  int_or_ptr_t open_arg = { .p = (void *) name };
  return ConnectionOpenChannel(CHANNEL_TYPE_ADB, cb, open_arg, cb_arg);
}

CHANNEL_HANDLE ConnectionOpenChannelBtServer(ChannelCallback cb,
                                             int_or_ptr_t cb_arg) {
  int_or_ptr_t open_arg = { .i = 0 };
  return ConnectionOpenChannel(CHANNEL_TYPE_BT, cb, open_arg, cb_arg);
}

CHANNEL_HANDLE ConnectionOpenChannelAccessory(ChannelCallback cb,
                                              int_or_ptr_t cb_arg) {
  int_or_ptr_t open_arg = { .i = 0 };
  return ConnectionOpenChannel(CHANNEL_TYPE_ACC, cb, open_arg, cb_arg);
}

CHANNEL_HANDLE ConnectionOpenChannelCdc(ChannelCallback cb,
                                        int_or_ptr_t cb_arg) {
  int_or_ptr_t open_arg = { .i = 0 };
  return ConnectionOpenChannel(CHANNEL_TYPE_CDC_DEVICE, cb, open_arg, cb_arg);
}

void ConnectionSend(CHANNEL_HANDLE ch, const void *data, int size) {
  int t = ch >> 12;
  int h = ch & 0x0FFF;
  factories[t]->connectionSend(h, data, size);
}

BOOL ConnectionCanSend(CHANNEL_HANDLE ch) {
  int t = ch >> 12;
  int h = ch & 0x0FFF;
  return factories[t]->connectionCanSend(h);
}

void ConnectionCloseChannel(CHANNEL_HANDLE ch) {
  int t = ch >> 12;
  int h = ch & 0x0FFF;
  factories[t]->connectionClose(h);
}

int ConnectionGetMaxPacket(CHANNEL_HANDLE ch) {
  int t = ch >> 12;
  int h = ch & 0x0FFF;
  return factories[t]->connectionMaxPacketSize(h);
}

BOOL USB_ApplicationEventHandler(BYTE address, USB_EVENT event, void *data, DWORD size) {
  // Handle specific events.
  switch (event) {
   case EVENT_VBUS_REQUEST_POWER:
    // We'll let anything attach.
    return TRUE;

   case EVENT_VBUS_RELEASE_POWER:
    // We aren't keeping track of power.
    return TRUE;

   case EVENT_HUB_ATTACH:
    log_printf("***** USB Error - hubs are not supported *****");
    return TRUE;

   case EVENT_UNSUPPORTED_DEVICE:
    log_printf("***** USB Error - device is not supported *****");
    return TRUE;

   case EVENT_CANNOT_ENUMERATE:
    log_printf("***** USB Error - cannot enumerate device *****");
    return TRUE;

   case EVENT_CLIENT_INIT_ERROR:
    log_printf("***** USB Error - client driver initialization error *****");
    return TRUE;

   case EVENT_OUT_OF_MEMORY:
    log_printf("***** USB Error - out of heap memory *****");
    return TRUE;

   case EVENT_UNSPECIFIED_ERROR:   // This should never be generated.
    log_printf("***** USB Error - unspecified *****");
    return TRUE;

   default:
    return FALSE;
  }
}  // USB_ApplicationEventHandler
