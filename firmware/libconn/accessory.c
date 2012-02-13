#include "accessory.h"

#include <assert.h>
#include <stdint.h>

#include "usb_host_android.h"
#include "logging.h"
#include "USB/usb_common.h"

typedef enum {
  CHANNEL_INIT,
  CHANNEL_WAIT_OPEN,
  CHANNEL_OPEN,
  CHANNEL_WAIT_CLOSED
} CHANNEL_STATE;

static void DummyCallback (int h, const void *data, UINT32 size) {}

static ChannelCallback callback = &DummyCallback;
static void *rx_buf;
static int rx_buf_size;
static CHANNEL_STATE channel_state;
static uint8_t is_channel_open;


void AccessoryInit(void *buf, int size) {
  rx_buf = buf;
  rx_buf_size = size;
  is_channel_open = 0;
  channel_state = CHANNEL_INIT;
}

void AccessoryShutdown() {
  log_printf("AccessoryShutdown()");
  callback = &DummyCallback;
}

int AccessoryTasks() {
  DWORD size;
  BYTE err;

  switch (channel_state) {
    case CHANNEL_INIT:
      USBHostAndroidRead(rx_buf, 1, ANDROID_INTERFACE_ACC);
      channel_state = CHANNEL_WAIT_OPEN;
      break;

    case CHANNEL_WAIT_OPEN:
      if (USBHostAndroidRxIsComplete(&err, &size, ANDROID_INTERFACE_ACC)) {
        if (err != USB_SUCCESS) {
          log_printf("Read failed with error code %d", err);
          return -1;
        }
        USBHostAndroidWrite(&is_channel_open, 1, ANDROID_INTERFACE_ACC);
        if (is_channel_open) {
          USBHostAndroidRead(rx_buf, rx_buf_size, ANDROID_INTERFACE_ACC);
          channel_state = CHANNEL_OPEN;
        } else {
          channel_state = CHANNEL_WAIT_CLOSED;
        }
      }
      break;

    case CHANNEL_OPEN:
      if (USBHostAndroidRxIsComplete(&err, &size, ANDROID_INTERFACE_ACC)) {
        if (err != USB_SUCCESS) {
          log_printf("Read failed with error code %d", err);
          return -1;
        }
        if (size) {
          callback(0, rx_buf, size);
        }
        // Channel might have been closed from within the callback.
        if (channel_state == CHANNEL_OPEN) {
          USBHostAndroidRead(rx_buf, rx_buf_size, ANDROID_INTERFACE_ACC);
        }
      }
      break;

    case CHANNEL_WAIT_CLOSED:
      if (USBHostAndroidTxIsComplete(&err, ANDROID_INTERFACE_ACC)) {
        if (err != USB_SUCCESS) {
          log_printf("Write failed with error code %d", err);
          return -1;
        }
        callback(0, NULL, 0);
        channel_state = CHANNEL_INIT;
      }
      break;
  }
  return channel_state == CHANNEL_INIT;
}

CHANNEL_HANDLE AccessoryOpenChannel(ChannelCallback cb) {
  assert(channel_state < CHANNEL_OPEN);

  callback = cb;
  is_channel_open = 1;
  return 0;
}

void AccessoryCloseChannel(CHANNEL_HANDLE h) {
  assert(h == 0);
  assert(channel_state == CHANNEL_OPEN);
  is_channel_open = 0;
  channel_state = CHANNEL_WAIT_CLOSED;
}

void AccessoryWrite(CHANNEL_HANDLE h, const void *data, int size) {
  assert(h == 0);
  assert(channel_state == CHANNEL_OPEN);
  USBHostAndroidWrite(data, size, ANDROID_INTERFACE_ACC);
}

int AccessoryCanWrite(CHANNEL_HANDLE h) {
  BYTE err;
  assert(h == 0);
  assert(channel_state <= CHANNEL_OPEN);
  if (channel_state != CHANNEL_OPEN) return 0;
  int res = USBHostAndroidTxIsComplete(&err, ANDROID_INTERFACE_ACC);
  if (res && err != USB_SUCCESS) {
    log_printf("Write failed with error code %d", err);
    USBHostAndroidReset();
  }
  return res;
}
