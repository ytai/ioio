#include "usb.h"
#include "usb_device.h"
#include "usb_device_cdc.h"
#include "usb_function_cdc.h"

USB_VOLATILE USB_DEVICE_STATE USBDeviceState = DETACHED_STATE;
BYTE cdc_trf_state = CDC_TX_BUSY;

BYTE getsUSBUSART(char *buffer, BYTE len) { return 0; }
void putUSBUSART(char *data, BYTE  length) {}