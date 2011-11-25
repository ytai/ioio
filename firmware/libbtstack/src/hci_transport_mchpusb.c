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

// Implementation of btstack hci transport layer over IOIO's bluetooth dongle
// driver within the MCHPUSB host framework.

#include <assert.h>
#include "config.h"

#include "usb_host_bluetooth.h"
#include "logging.h"

#include "debug.h"
#include "hci.h"
#include "hci_transport.h"
#include "hci_dump.h"

static uint8_t *bulk_in;
static int bulk_in_size;
static uint8_t *int_in;
#define INT_IN_SIZE 64

void hci_transport_mchpusb_tasks() {
  if (!USBHostBlueToothIntInBusy()) {
    USBHostBluetoothReadInt(int_in, INT_IN_SIZE);
  }
  if (!USBHostBlueToothBulkInBusy()) {
    USBHostBluetoothReadBulk(bulk_in, bulk_in_size);
  }
}

// single instance
static hci_transport_t hci_transport_mchpusb;
static void (*packet_handler)(uint8_t packet_type, uint8_t *packet, uint16_t size) = NULL;

static int usb_open(void *transport_config) {
  return 0;
}

static int usb_close() {
  return 0;
}

static int usb_send_cmd_packet(uint8_t *packet, int size) {
  return USB_SUCCESS == USBHostBluetoothWriteControl(packet, size) ? 0 : -1;
}

static int usb_send_acl_packet(uint8_t *packet, int size) {
  return USB_SUCCESS == USBHostBluetoothWriteBulk(packet, size) ? 0 : -1;
}

static int usb_send_packet(uint8_t packet_type, uint8_t * packet, int size) {
  switch (packet_type) {
    case HCI_COMMAND_DATA_PACKET:
      return usb_send_cmd_packet(packet, size);
    case HCI_ACL_DATA_PACKET:
      return usb_send_acl_packet(packet, size);
    default:
      return -1;
  }
}

static int usb_can_send_packet(uint8_t packet_type) {
  switch (packet_type) {
    case HCI_COMMAND_DATA_PACKET:
      return !USBHostBlueToothControlOutBusy();
    case HCI_ACL_DATA_PACKET:
      return !USBHostBlueToothBulkOutBusy();
    default:
      return -1;
  }
}

static void usb_register_packet_handler(void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size)) {
  log_info("registering packet handler\n");
  packet_handler = handler;
}

static const char * usb_get_transport_name() {
  return "USB";
}

// get usb singleton

hci_transport_t * hci_transport_mchpusb_instance(void *buf, int size) {
  assert(size > INT_IN_SIZE + 256);
  int_in = buf;
  bulk_in = int_in + INT_IN_SIZE;
  bulk_in_size = size - INT_IN_SIZE;
  hci_transport_mchpusb.open = usb_open;
  hci_transport_mchpusb.close = usb_close;
  hci_transport_mchpusb.send_packet = usb_send_packet;
  hci_transport_mchpusb.register_packet_handler = usb_register_packet_handler;
  hci_transport_mchpusb.get_transport_name = usb_get_transport_name;
  hci_transport_mchpusb.set_baudrate = NULL;
  hci_transport_mchpusb.can_send_packet_now = usb_can_send_packet;
  return &hci_transport_mchpusb;
}

BOOL USBHostBluetoothCallback(BLUETOOTH_EVENT event,
        USB_EVENT status,
        void *data,
        DWORD size) {
  uint8_t e;
  switch (event) {
    case BLUETOOTH_EVENT_WRITE_BULK_DONE:
    case BLUETOOTH_EVENT_WRITE_CONTROL_DONE:
      e = DAEMON_EVENT_HCI_PACKET_SENT;
      packet_handler(HCI_EVENT_PACKET, &e, 1);
      return TRUE;

    case BLUETOOTH_EVENT_ATTACHED:
    case BLUETOOTH_EVENT_DETACHED:
      return TRUE;

    case BLUETOOTH_EVENT_READ_BULK_DONE:
      if (status == USB_SUCCESS) {
        if (size) {
          if (packet_handler) {
            packet_handler(HCI_ACL_DATA_PACKET, data, size);
          }
        }
      } else {
        log_printf("Read bulk failure");
      }
      return TRUE;

    case BLUETOOTH_EVENT_READ_INTERRUPT_DONE:
      if (status == USB_SUCCESS) {
        if (size) {
          if (packet_handler) {
            packet_handler(HCI_EVENT_PACKET, data, size);
          }
        }
      } else {
        log_printf("Read bulk failure");
      }
      return TRUE;

    default:
      return FALSE;
  }
}
