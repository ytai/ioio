#include "accessory_connection.h"

#include <assert.h>
#include <stdint.h>

#include "usb_host_android.h"
#include "logging.h"
#include "USB/usb_common.h"

typedef enum {
  CHANNEL_DETACHED,
  CHANNEL_INIT,
  CHANNEL_WAIT_OPEN,
  CHANNEL_OPEN,
  CHANNEL_WAIT_CLOSED
} CHANNEL_STATE;

static void DummyCallback (const void *data, UINT32 size, int_or_ptr_t arg) {}

static ChannelCallback callback = &DummyCallback;
static int_or_ptr_t callback_arg;
static void *rx_buf;
static int rx_buf_size;
static CHANNEL_STATE channel_state;
static uint8_t is_channel_open;


static void AccessoryInit(void *buf, int size) {
  rx_buf = buf;
  rx_buf_size = size;
  is_channel_open = 0;
  channel_state = CHANNEL_DETACHED;
}

static void AccessoryTasks() {
  DWORD size;
  BYTE err;

  // handle detach
  if (channel_state > CHANNEL_DETACHED
      && !USBHostAndroidIsInterfaceAttached(ANDROID_INTERFACE_ACC)) {
    if (is_channel_open) {
      callback(NULL, 1, callback_arg);
      is_channel_open = 0;
    }
    channel_state = CHANNEL_DETACHED;
  }

  switch (channel_state) {
    case CHANNEL_DETACHED:
      if (USBHostAndroidIsInterfaceAttached(ANDROID_INTERFACE_ACC)) {
        channel_state = CHANNEL_INIT;
      }
      break;

    case CHANNEL_INIT:
      USBHostAndroidRead(rx_buf, 1, ANDROID_INTERFACE_ACC);
      channel_state = CHANNEL_WAIT_OPEN;
      break;

    case CHANNEL_WAIT_OPEN:
      if (USBHostAndroidRxIsComplete(&err, &size, ANDROID_INTERFACE_ACC)) {
        if (err != USB_SUCCESS) {
          log_printf("Read failed with error code %d", err);
          return;
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
          return;
        }
        if (size) {
          callback(rx_buf, size, callback_arg);
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
          return;
        }
        callback(NULL, 0, callback_arg);
        channel_state = CHANNEL_INIT;
      }
      break;
  }
}

static int AccessoryOpenChannel(ChannelCallback cb, int_or_ptr_t open_arg,
                        int_or_ptr_t cb_args) {
  assert(channel_state < CHANNEL_OPEN);

  callback = cb;
  callback_arg = cb_args;
  is_channel_open = 1;
  return 0;
}

static void AccessoryCloseChannel(int h) {
  assert(h == 0);
  assert(channel_state == CHANNEL_OPEN);
  is_channel_open = 0;
  channel_state = CHANNEL_WAIT_CLOSED;
}

static void AccessorySend(int h, const void *data, int size) {
  assert(h == 0);
  assert(channel_state == CHANNEL_OPEN);
  USBHostAndroidWrite(data, size, ANDROID_INTERFACE_ACC);
}

static int AccessoryCanSend(int h) {
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

int AccessoryIsAvailable() {
  return USBHostAndroidIsInterfaceAttached(ANDROID_INTERFACE_ACC);
}

int AccessoryIsReadyToOpen() {
  return channel_state == CHANNEL_INIT;
}

int AccessoryMaxPacketSize(int h) {
  assert(h == 0);
  return INT_MAX;  // unlimited
}

const CONNECTION_FACTORY accessory_connection_factory = {
  AccessoryInit,
  AccessoryTasks,
  AccessoryIsAvailable,
  AccessoryIsReadyToOpen,
  AccessoryOpenChannel,
  AccessoryCloseChannel,
  AccessorySend,
  AccessoryCanSend,
  AccessoryMaxPacketSize
};
