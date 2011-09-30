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
#include "usb_host_bluetooth.h"
#include "adb_private.h"
#include "adb_file_private.h"

#include "hci.h"
#include "l2cap.h"
#include "rfcomm.h"
#include "sdp.h"
#include "timer.h"

static uint8_t bulk_in[64];
static uint8_t int_in[64];

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

static uint8_t   rfcomm_channel_nr = 1;
static uint16_t  rfcomm_channel_id;
static uint8_t   spp_service_buffer[128];
static uint8_t   rfcomm_send_credit = 0;

static void packet_handler(void * connection, uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
  bd_addr_t event_addr;
  uint8_t rfcomm_channel_nr;
  uint16_t mtu;

  switch (packet_type) {
    case HCI_EVENT_PACKET:
      switch (packet[0]) {

        case BTSTACK_EVENT_STATE:
          // bt stack activated, get started - set local name
          if (packet[2] == HCI_STATE_WORKING) {
            hci_send_cmd(&hci_write_local_name, "IOIO");
          }
          break;

        case HCI_EVENT_COMMAND_COMPLETE:
          if (COMMAND_COMPLETE_EVENT(packet, hci_read_bd_addr)) {
            bt_flip_addr(event_addr, &packet[6]);
            log_printf("BD-ADDR: %s\n\r", bd_addr_to_str(event_addr));
            break;
          }
          if (COMMAND_COMPLETE_EVENT(packet, hci_write_local_name)) {
            hci_discoverable_control(1);
            break;
          }
          break;

        case HCI_EVENT_LINK_KEY_REQUEST:
          // deny link key request
          log_printf("Link key request\n\r");
          bt_flip_addr(event_addr, &packet[2]);
          hci_send_cmd(&hci_link_key_request_negative_reply, &event_addr);
          break;

        case HCI_EVENT_PIN_CODE_REQUEST:
          // inform about pin code request
          log_printf("Pin code request - using '0000'\n\r");
          bt_flip_addr(event_addr, &packet[2]);
          hci_send_cmd(&hci_pin_code_request_reply, &event_addr, 4, "0000");
          break;

        case RFCOMM_EVENT_INCOMING_CONNECTION:
          // data: event (8), len(8), address(48), channel (8), rfcomm_cid (16)
          bt_flip_addr(event_addr, &packet[2]);
          rfcomm_channel_nr = packet[8];
          rfcomm_channel_id = READ_BT_16(packet, 9);
          log_printf("RFCOMM channel %u requested for %s\n\r", rfcomm_channel_nr, bd_addr_to_str(event_addr));
          rfcomm_accept_connection_internal(rfcomm_channel_id);
          break;

        case RFCOMM_EVENT_OPEN_CHANNEL_COMPLETE:
          // data: event(8), len(8), status (8), address (48), server channel(8), rfcomm_cid(16), max frame size(16)
          if (packet[2]) {
            log_printf("RFCOMM channel open failed, status %u\n\r", packet[2]);
          } else {
            rfcomm_channel_id = READ_BT_16(packet, 12);
            rfcomm_send_credit = 1;
            mtu = READ_BT_16(packet, 14);
            log_printf("\n\rRFCOMM channel open succeeded. New RFCOMM Channel ID %u, max frame size %u\n\r", rfcomm_channel_id, mtu);
          }
          break;

        case RFCOMM_EVENT_CHANNEL_CLOSED:
          rfcomm_channel_id = 0;
          break;

        default:
          break;
      }
      break;

    case RFCOMM_DATA_PACKET:
      // hack: truncate data (we know that the packet is at least on byte bigger
      //packet[size] = 0;
      //puts((const char *) packet);
      rfcomm_send_credit = 1;

    default:
      break;
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
        btstack_memory_init();

        // init HCI
        hci_transport_t * transport = hci_transport_mchpusb_instance();
        bt_control_t * control = NULL;
        hci_uart_config_t * config = NULL;
        remote_device_db_t * remote_db = NULL;
        hci_init(transport, config, control, remote_db);
        hci_power_control(HCI_POWER_ON);

        // init L2CAP
        l2cap_init();
        l2cap_register_packet_handler(packet_handler);

        // init RFCOMM
        rfcomm_init();
        rfcomm_register_packet_handler(packet_handler);
        rfcomm_register_service_internal(NULL, rfcomm_channel_nr, 100); // reserved channel, mtu=100

        // init SDP, create record for SPP and register with SDP
        sdp_init();
        memset(spp_service_buffer, 0, sizeof (spp_service_buffer));
        service_record_item_t * service_record_item = (service_record_item_t *) spp_service_buffer;
        sdp_create_spp_service((uint8_t*) & service_record_item->service_record, 1, "SPP Counter");
        log_printf("SDP service buffer size: %u\n\r", (uint16_t) (sizeof (service_record_item_t) + de_get_len((uint8_t*) & service_record_item->service_record)));
        sdp_register_service_internal(NULL, service_record_item);

        bt_state = STATE_BT_INITIALIZED;
      }
      break;

    case STATE_BT_INITIALIZED:
#ifndef USB_ENABLE_TRANSFER_EVENT
      USBHostBluetoothTasks();
#endif
      if (!USBHostBlueToothIntInBusy()) {
        USBHostBluetoothReadInt(int_in, sizeof int_in);
      }
      if (!USBHostBlueToothBulkInBusy()) {
        USBHostBluetoothReadBulk(bulk_in, sizeof bulk_in);
      }
      hci_run();
      l2cap_run();

      if (rfcomm_channel_id && rfcomm_send_credit) {
        rfcomm_grant_credits(rfcomm_channel_id, 1);
        rfcomm_send_credit = 0;
      }
      
      if (!USBHostBlueToothControlOutBusy()) {
//        pbuf_cmd_out = phybusif_next_command();
//        if (pbuf_cmd_out) {
//          USBHostBluetoothWriteControl(pbuf_cmd_out->payload, pbuf_cmd_out->len);
          // DelayMs(10);
//        }
      }
      if (!USBHostBlueToothBulkOutBusy()) {
//        while ((pbuf_acl_out = phybusif_next_acl())) {
//          if (pbuf_acl_out->len) {
//            USBHostBluetoothWriteBulk(pbuf_acl_out->payload, pbuf_acl_out->len);
            // DelayMs(10);
//            break;
//          }
//        }
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
