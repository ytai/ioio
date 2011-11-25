#include "accessory.h"
#include "usb_host_android.h"
#include "logging.h"
#include "USB/usb_common.h"

static void DummyCallback (int h, const void *data, UINT32 size) {}

static AccessoryCallback callback = &DummyCallback;
static void *rx_buf;
static int rx_buf_size;


void AccessoryInit(void *buf, int size) {
  log_printf("AccessroyInit(%p, %d)", buf, size);
  rx_buf = buf;
  rx_buf_size = size;
  USBHostAndroidRead(rx_buf, rx_buf_size, ANDROID_INTERFACE_ACC);
}

void AccessoryShutdown() {
  log_printf("AccessoryShutdown()");
  callback = &DummyCallback;
}

void AccessoryTasks() {
  DWORD size;
  BYTE err;
  if (USBHostAndroidRxIsComplete(&err, &size, ANDROID_INTERFACE_ACC)) {
    if (err != USB_SUCCESS) {
      log_printf("Read failed with error code %d", err);
      USBHostAndroidReset();
      return;
    }
    if (size) {
      callback(0, rx_buf, size);
    }
    USBHostAndroidRead(rx_buf, rx_buf_size, ANDROID_INTERFACE_ACC);
  }
}

void AccessorySetCallback(AccessoryCallback cb) {
  callback = cb;
}

void AccessoryWrite(const void *data, int size) {
  USBHostAndroidWrite(data, size, ANDROID_INTERFACE_ACC);
}

int AccessoryCanWrite() {
  BYTE err;
  int res = USBHostAndroidTxIsComplete(&err, ANDROID_INTERFACE_ACC);
  if (res && err != USB_SUCCESS) {
    log_printf("Write failed with error code %d", err);
    USBHostAndroidReset();
  }
  return res;
}
