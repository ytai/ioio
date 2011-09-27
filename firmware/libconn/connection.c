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

#include "lwbt/phybusif.h"


#include "connection.h"

#include <string.h>
#include <assert.h>

#include "logging.h"
#include "usb_config.h"
#include "usb/usb.h"
#include "usb/usb_host.h"
#include "usb_host_android.h"
#include "usb_host_bluetooth.h"
#include "adb_private.h"
#include "adb_file_private.h"
#include "lwbt/hci.h"
#include "lwbt/phybusif.h"

typedef enum {
  STATE_BT_DISCONNECTED,
  STATE_BT_INITIALIZED,
} BT_STATE;

typedef enum {
  STATE_ADB_DISCONNECTED,
  STATE_ADB_INTIALIZED
} ADB_STATE;

static BT_STATE bt_state;
static ADB_STATE adb_state;

struct pbuf *pbuf_cmd_in = NULL, *pbuf_acl_in = NULL,
            *pbuf_cmd_out = NULL, *pbuf_acl_out = NULL;

BOOL USBHostBluetoothCallback(BLUETOOTH_EVENT event,
        USB_EVENT status,
        void *data,
        DWORD size) {
  switch (event) {
    case BLUETOOTH_EVENT_WRITE_BULK_DONE:
      if (pbuf_acl_out->flags) {
        pbuf_header(pbuf_acl_out, 1);
        pbuf_acl_out->flags = 0;
      }
      pbuf_free(pbuf_acl_out);
      pbuf_acl_out = NULL;
      return TRUE;

    case BLUETOOTH_EVENT_WRITE_CONTROL_DONE:
      if (pbuf_cmd_out->flags) {
        pbuf_header(pbuf_cmd_out, 1);
        pbuf_cmd_out->flags = 0;
      }
      pbuf_free(pbuf_cmd_out);
      pbuf_cmd_out = NULL;
      return TRUE;

    case BLUETOOTH_EVENT_ATTACHED:
      return TRUE;

    case BLUETOOTH_EVENT_DETACHED:
      log_printf("detach event");
      if (pbuf_cmd_out) pbuf_free(pbuf_cmd_out);
      if (pbuf_acl_out) pbuf_free(pbuf_acl_out);
      pbuf_cmd_out = pbuf_acl_out = NULL;
      return TRUE;

    case BLUETOOTH_EVENT_READ_BULK_DONE:
      if (size) {
        pbuf_acl_in->len = pbuf_acl_in->tot_len = size;
        pbuf_header(pbuf_acl_in, -HCI_ACL_HDR_LEN);
        hci_acl_input(pbuf_acl_in);
        // For some strange reason, hci_acl_input takes ownership of the buffer.
        // so don't free here.
      } else {
        pbuf_free(pbuf_acl_in);
      }
      return TRUE;

    case BLUETOOTH_EVENT_READ_INTERRUPT_DONE:
      if (size) {
        pbuf_header(pbuf_cmd_in, -HCI_EVENT_HDR_LEN);
        hci_event_input(pbuf_cmd_in);
      }
      pbuf_free(pbuf_cmd_in);
      return TRUE;

    default:
      return FALSE;
  }
}

static void ConnBTTasks() {
  static unsigned int tcount;

  if (!USBHostBluetoothIsDeviceAttached()) {
    bt_state = STATE_BT_DISCONNECTED;
    return;
  }

  switch (bt_state) {
    case STATE_BT_DISCONNECTED:
      if (USBHostBluetoothIsDeviceAttached()) {
        bt_init();
        tcount = -1;
        bt_state = STATE_BT_INITIALIZED;
      }
      break;

    case STATE_BT_INITIALIZED:
#ifndef USB_ENABLE_TRANSFER_EVENT
      USBHostBluetoothTasks();
#endif
      if (!USBHostBlueToothIntInBusy()) {
        pbuf_cmd_in = pbuf_alloc(PBUF_RAW, 64, PBUF_POOL);
        if (!pbuf_cmd_in) {
          log_printf("OUT OF MEM");
        }
        USBHostBluetoothReadInt(pbuf_cmd_in->payload, 64);
      }
      if (!USBHostBlueToothBulkInBusy()) {
        pbuf_acl_in = pbuf_alloc(PBUF_RAW, 64, PBUF_POOL);
        if (!pbuf_acl_in) {
          log_printf("OUT OF MEM");
        }
        USBHostBluetoothReadBulk(pbuf_acl_in->payload, 64);
      }
      if (!USBHostBlueToothControlOutBusy()) {
        pbuf_cmd_out = phybusif_next_command();
        if (pbuf_cmd_out) {
          USBHostBluetoothWriteControl(pbuf_cmd_out->payload, pbuf_cmd_out->len);
          // DelayMs(10);
        }
      }
      if (!USBHostBlueToothBulkOutBusy()) {
        while ((pbuf_acl_out = phybusif_next_acl())) {
          if (pbuf_acl_out->len) {
            USBHostBluetoothWriteBulk(pbuf_acl_out->payload, pbuf_acl_out->len);
            // DelayMs(10);
            break;
          } else {
            pbuf_free(pbuf_acl_out);
          }
        }
      }

      if (tcount-- == 0) {
        l2cap_tmr();
        rfcomm_tmr();
        bt_spp_tmr();
        tcount = -1;
      }
      break;
  }
}

static void ConnADBTasks() {
  int res;

  if (!USBHostAndroidIsDeviceAttached()) {
    adb_state = STATE_ADB_DISCONNECTED;
    return;
  }

  switch (adb_state) {
    case STATE_ADB_DISCONNECTED:
      if (USBHostAndroidIsInterfaceAttached(ANDROID_INTERFACE_ADB)) {
        ADBInit();
        ADBFileInit();
        adb_state = STATE_ADB_INTIALIZED;
      }
      break;

    case STATE_ADB_INTIALIZED:
#ifndef USB_ENABLE_TRANSFER_EVENT
      USBHostAndroidTasks();
#endif
      res = ADBTasks();
      if (res == 1) {
        ADBFileTasks();
      } else if (res == -1) {
        log_printf("Error occured. Resetting Android USB.");
        USBHostAndroidReset();
      }
      break;
  }
}

void ConnectionInit() {
  BOOL res = USBHostInit(0);
  assert(res);
  bt_state = STATE_BT_DISCONNECTED;
  adb_state = STATE_ADB_DISCONNECTED;
}

BOOL ConnectionTasks() {
  USBHostTasks();
  ConnBTTasks();
  ConnADBTasks();

  return adb_state != STATE_ADB_DISCONNECTED || bt_state != STATE_BT_DISCONNECTED;
}

void ConnectionResetUSB() {
  USBHostShutdown();
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
