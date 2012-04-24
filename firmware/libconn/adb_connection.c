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

#include "adb_connection.h"

#include <limits.h>

#include "adb.h"
#include "adb_private.h"
#include "usb_host_android.h"

static int adb_connected = 0;

static void ADBConInit(void *buf, int size) {
  return ADBInit();
}

static int ADBConIsAvailable() {
  return USBHostAndroidIsInterfaceAttached(ANDROID_INTERFACE_ADB);
}

static int ADBConIsReadyToOpen() {
  return adb_connected;
}

static void ADBConTasks() {
  adb_connected = (ADBTasks() == 1);
}

static int ADBConOpenChannel(ChannelCallback cb, int_or_ptr_t open_arg,
                             int_or_ptr_t cb_args) {
  return ADBOpen((const char *) open_arg.p, cb, cb_args);
}

static void ADBConCloseChannel(int h) {
  ADBClose(h);
}

static void ADBConSend(int h, const void *data, int size) {
  ADBWrite(h, data, size);
}

static int ADBConCanSend(int h) {
  return ADBChannelReady(h);
}

static int ADBConMaxPacketSize(int h) {
  return INT_MAX; // unlimited
}

const CONNECTION_FACTORY adb_connection_factory = {
  ADBConInit,
  ADBConTasks,
  ADBConIsAvailable,
  ADBConIsReadyToOpen,
  ADBConOpenChannel,
  ADBConCloseChannel,
  ADBConSend,
  ADBConCanSend,
  ADBConMaxPacketSize
};