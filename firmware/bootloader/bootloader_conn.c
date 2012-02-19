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

#include "bootloader_conn.h"

#include <string.h>
#include <assert.h>

#include "logging.h"
#include "libusb/usb_config.h"
#include "libusb/usb_host_android.h"
#include "adb_private.h"
#include "adb_file_private.h"

typedef enum {
  STATE_ADB_DISCONNECTED,
  STATE_ADB_INITIALIZING,
  STATE_ADB_INITIALIZED
} ADB_STATE;

static ADB_STATE adb_state;
static BOOL unknown_device_attached;

static void ConnADBTasks() {
  int res;

  if (!ADBAttached()) {
    adb_state = STATE_ADB_DISCONNECTED;
    return;
  }

  switch (adb_state) {
    case STATE_ADB_DISCONNECTED:
      if (ADBAttached()) {
        ADBInit();
        ADBFileInit();
        adb_state = STATE_ADB_INITIALIZED;
      }
      break;

    case STATE_ADB_INITIALIZING:
    case STATE_ADB_INITIALIZED:
      res = ADBTasks();
      if (res == -1) {
        log_printf("Error occured. Resetting Android USB.");
        USBHostAndroidReset();
        break;
      }
#ifndef USB_ENABLE_TRANSFER_EVENT
      USBHostAndroidTasks();
#endif
      if (res == 1) {
        ADBFileTasks();
        adb_state = STATE_ADB_INITIALIZED;
      } else if (res == 0) {
        adb_state = STATE_ADB_INITIALIZING;
      }
      break;
  }
}

void BootloaderConnInit() {
  BOOL res = USBHostInit(0);
  assert(res);
  adb_state = STATE_ADB_DISCONNECTED;
  unknown_device_attached = FALSE;
}

BOOL BootloaderConnTasks() {
  USBHostTasks();
  ConnADBTasks();

  return USBHostAndroidIsInterfaceAttached(ANDROID_INTERFACE_ADB) || unknown_device_attached;
}

void BootloaderConnResetUSB() {
  USBHostShutdown();
}

BOOL USB_ApplicationEventHandler(BYTE address, USB_EVENT event, void *data, DWORD size) {
  // Handle specific events.
  switch (event) {
   case EVENT_VBUS_REQUEST_POWER:
    // We'll let anything attach.
    return TRUE;

   case EVENT_VBUS_RELEASE_POWER:
    unknown_device_attached = FALSE;
    return TRUE;

   case EVENT_HUB_ATTACH:
    log_printf("***** USB Error - hubs are not supported *****");
    return TRUE;

   case EVENT_UNSUPPORTED_DEVICE:
    log_printf("***** USB Error - device is not supported *****");
    unknown_device_attached = TRUE;
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
