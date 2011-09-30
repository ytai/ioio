#include "config.h"

#include "usb_host_bluetooth.h"
#include "logging.h"

#include "debug.h"
#include "hci.h"
#include "hci_transport.h"
#include "hci_dump.h"

// prototypes
static void dummy_handler(uint8_t packet_type, uint8_t *packet, uint16_t size); 

// single instance
static hci_transport_t hci_transport_mchpusb;

static  void (*packet_handler)(uint8_t packet_type, uint8_t *packet, uint16_t size) = dummy_handler;

static int usb_open(void *transport_config){
    return 0;
}
static int usb_close(){
    return 0;
}

static int usb_send_cmd_packet(uint8_t *packet, int size){
    return USB_SUCCESS == USBHostBluetoothWriteControl(packet, size) ? 0 : -1;
}

static int usb_send_acl_packet(uint8_t *packet, int size){
    return USB_SUCCESS == USBHostBluetoothWriteBulk(packet, size) ? 0 : -1;
}

static int usb_send_packet(uint8_t packet_type, uint8_t * packet, int size){
    switch (packet_type){
        case HCI_COMMAND_DATA_PACKET:
            return usb_send_cmd_packet(packet, size);
        case HCI_ACL_DATA_PACKET:
            return usb_send_acl_packet(packet, size);
        default:
            return -1;
    }
}

static int usb_can_send_packet(uint8_t packet_type){
    switch (packet_type){
        case HCI_COMMAND_DATA_PACKET:
            return !USBHostBlueToothBulkOutBusy();
        case HCI_ACL_DATA_PACKET:
            return !USBHostBlueToothControlOutBusy();
        default:
            return -1;
    }
}

static void usb_register_packet_handler(void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size)){
    log_info("registering packet handler\n");
    packet_handler = handler;
}

static const char * usb_get_transport_name(){
    return "USB";
}

static void dummy_handler(uint8_t packet_type, uint8_t *packet, uint16_t size){
}

// get usb singleton
hci_transport_t * hci_transport_mchpusb_instance() {
        hci_transport_mchpusb.open                          = usb_open;
        hci_transport_mchpusb.close                         = usb_close;
        hci_transport_mchpusb.send_packet                   = usb_send_packet;
        hci_transport_mchpusb.register_packet_handler       = usb_register_packet_handler;
        hci_transport_mchpusb.get_transport_name            = usb_get_transport_name;
        hci_transport_mchpusb.set_baudrate                  = NULL;
        hci_transport_mchpusb.can_send_packet_now           = usb_can_send_packet;
    return &hci_transport_mchpusb;
}

BOOL USBHostBluetoothCallback(BLUETOOTH_EVENT event,
                              USB_EVENT status,
                              void *data,
                              DWORD size) {
  switch (event) {
    case BLUETOOTH_EVENT_WRITE_BULK_DONE:
    case BLUETOOTH_EVENT_WRITE_CONTROL_DONE:
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
