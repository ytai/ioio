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

#include "logging.h"
#include "usb_config.h"
#include "usb/usb.h"
#include "usb/usb_host.h"
#include "usb_host_android.h"
#include "adb_private.h"
#include "adb_file_private.h"

BOOL ConnectionTasks() {
  int res;
  
  USBHostTasks();
#ifndef USB_ENABLE_TRANSFER_EVENT
  USBHostAndroidTasks();
#endif

  res = ADBTasks();
  if (res == 1) {
    ADBFileTasks();
  } else if (res == -1) {
    log_printf("Error occured. Resetting USB.");
    USBHostAndroidReset();
    return FALSE;
  }
  // TODO: this function should return the state of the USB connection
  // (or not return anything). Separate functions to be used for checking
  // the connection state of a specific channel.
  return USBHostAndroidIsDeviceAttached();
}

void ConnectionResetUSB() {
  USBHostShutdown();
}

void ConnectionInit() {
  BOOL res = USBHostInit(0);
  assert(res);
  ADBInit();
  ADBFileInit();
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
