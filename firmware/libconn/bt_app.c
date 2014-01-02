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

// Application-layer Bluetooth logic.

#include "bt_app.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "logging.h"
#include "hci.h"
#include "l2cap.h"
#include "rfcomm.h"
#include "sdp.h"
#include "btstack_memory.h"
#include "hci_transport.h"
#include "btstack/sdp_util.h"

static void DummyCallback(int h, const void *data, UINT32 size) {
}

static uint8_t    rfcomm_channel_nr = 1;
static uint16_t   rfcomm_channel_id;
static uint8_t    spp_service_buffer[128] __attribute__((aligned(__alignof(service_record_item_t))));
static uint8_t    rfcomm_send_credit = 0;
static BTCallback client_callback;
static char       local_name[] = "PIXEL (00:00)";  // the digits will be replaced by the MSB of the BD-ADDR

static void PacketHandler(void * connection, uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
  bd_addr_t event_addr;
  uint8_t rfcomm_channel_nr;
  uint16_t mtu;

  switch (packet_type) {
    case HCI_EVENT_PACKET:
      switch (packet[0]) {
        case BTSTACK_EVENT_STATE:
          // bt stack activated, get started - set local name
          if (packet[2] == HCI_STATE_WORKING) {
            hci_send_cmd(&hci_write_local_name, local_name);
          }
          break;

        case HCI_EVENT_COMMAND_COMPLETE:
          if (COMMAND_COMPLETE_EVENT(packet, hci_read_bd_addr)) {
            bt_flip_addr(event_addr, &packet[6]);
            log_printf("BD-ADDR: %s\n\r", bd_addr_to_str(event_addr));
            sprintf(local_name, "PIXEL (%02X:%02X)", event_addr[4], event_addr[5]);
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
          log_printf("RFCOMM channel closed.");
          client_callback(rfcomm_channel_id, NULL, 0);
          rfcomm_channel_id = 0;
          break;

        default:
          break;
      }
      break;

    case RFCOMM_DATA_PACKET:
      client_callback(rfcomm_channel_id, packet, size);
      rfcomm_send_credit = 1;

    default:
      break;
  }
}

void BTInit(void *buf, int size) {
  btstack_memory_init();

  // init HCI
  hci_transport_t * transport = hci_transport_mchpusb_instance(buf, size);
  bt_control_t * control = NULL;
  hci_uart_config_t * config = NULL;
  const remote_device_db_t * remote_db = &remote_device_db_memory;
  hci_init(transport, config, control, remote_db);

  // init L2CAP
  l2cap_init();
  l2cap_register_packet_handler(PacketHandler);

  // init RFCOMM
  rfcomm_init();
  rfcomm_register_packet_handler(PacketHandler);
  rfcomm_register_service_internal(NULL, rfcomm_channel_nr, 100); // reserved channel, mtu=100

  // init SDP, create record for SPP and register with SDP
  sdp_init();
  memset(spp_service_buffer, 0, sizeof (spp_service_buffer));
  service_record_item_t * service_record_item = (service_record_item_t *) spp_service_buffer;
  sdp_create_spp_service((uint8_t*) & service_record_item->service_record, 1, "IOIO-App");
  log_printf("SDP service buffer size: %u\n\r", (uint16_t) (sizeof (service_record_item_t) + de_get_len((uint8_t*) & service_record_item->service_record)));
  sdp_register_service_internal(NULL, service_record_item);

  hci_power_control(HCI_POWER_ON);

  client_callback = DummyCallback;
}

void BTShutdown() {
  hci_close();
}

void BTTasks() {
  hci_transport_mchpusb_tasks();

  if (rfcomm_channel_id && rfcomm_send_credit) {
    rfcomm_grant_credits(rfcomm_channel_id, 1);
    rfcomm_send_credit = 0;
  }
}

int BTAccepting() {
  return rfcomm_channel_id != 0;
}

void BTSetCallback(BTCallback cb) {
  client_callback = cb;
}

void BTWrite(const void *data, int size) {
  assert(!(size >> 16));
  rfcomm_send_internal(rfcomm_channel_id, (uint8_t *) data, size & 0xFFFF);
}

int BTCanWrite() {
  return rfcomm_can_send(rfcomm_channel_id);
}

void BTClose() {
  rfcomm_disconnect_internal(rfcomm_channel_id);
}
