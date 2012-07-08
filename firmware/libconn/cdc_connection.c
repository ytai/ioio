#include "cdc_connection.h"

#include <assert.h>
#include <stdint.h>

#define USB_SUPPORT_DEVICE
#include "USB/usb_function_cdc.h"
#include "USB/usb_device.h"
#include "logging.h"
#include "USB/usb_common.h"

typedef enum {
  CHANNEL_DETACHED,
  CHANNEL_WAIT_DTE,
  CHANNEL_WAIT_OPEN,
  CHANNEL_OPEN,
} CHANNEL_STATE;

static void DummyCallback(const void *data, UINT32 size, int_or_ptr_t arg) {
}

static ChannelCallback callback = &DummyCallback;
static int_or_ptr_t callback_arg;
static void *rx_buf;
static int rx_buf_size;
static CHANNEL_STATE channel_state;

static void CDCInit(void *buf, int size) {
  rx_buf = buf;
  rx_buf_size = min(size, 0xFF);
  channel_state = CHANNEL_DETACHED;
}

static void CDCTasks() {
  DWORD size;

  if (channel_state > CHANNEL_DETACHED
      && USBGetDeviceState() == DETACHED_STATE) {
    // handle detach
    if (channel_state >= CHANNEL_OPEN) {
      callback(NULL, 1, callback_arg);
    }
    channel_state = CHANNEL_DETACHED;
  } else if (channel_state > CHANNEL_WAIT_DTE
      && !CDCIsDtePresent()) {
    // handle close
    if (channel_state >= CHANNEL_OPEN) {
      callback(NULL, 0, callback_arg);
    }
    channel_state = CHANNEL_WAIT_DTE;
  }

  switch (channel_state) {
    case CHANNEL_DETACHED:
      if (USBGetDeviceState() == CONFIGURED_STATE) {
        channel_state = CHANNEL_WAIT_DTE;
      }
      break;

    case CHANNEL_WAIT_DTE:
      if (CDCIsDtePresent()) {
        channel_state = CHANNEL_WAIT_OPEN;
      }
      break;

    case CHANNEL_WAIT_OPEN:
      break;

    case CHANNEL_OPEN:
      size = getsUSBUSART(rx_buf, rx_buf_size);
      if (size) {
        callback(rx_buf, size, callback_arg);
      }
      break;
  }
}

static int CDCOpenChannel(ChannelCallback cb, int_or_ptr_t open_arg,
                          int_or_ptr_t cb_args) {
  assert(channel_state == CHANNEL_WAIT_OPEN);

  callback = cb;
  callback_arg = cb_args;
  channel_state = CHANNEL_OPEN;
  return 0;
}

static void CDCCloseChannel(int h) {
  // Do nothing. Host will close the channel.
}

static void CDCSend(int h, const void *data, int size) {
  assert(h == 0);
  assert(channel_state == CHANNEL_OPEN);
  putUSBUSART((char *) data, size);
}

static int CDCCanSend(int h) {
  assert(h == 0);
  if (channel_state != CHANNEL_OPEN) return 0;
  return USBUSARTIsTxTrfReady();
}

int CDCIsAvailable() {
  return USBGetDeviceState() != DETACHED_STATE;
}

int CDCIsReadyToOpen() {
  return channel_state == CHANNEL_WAIT_OPEN;
}

int CDCMaxPacketSize(int h) {
  assert(h == 0);
//  return INT_MAX; // unlimited
  return 255;
}

const CONNECTION_FACTORY cdc_connection_factory = {
  CDCInit,
  CDCTasks,
  CDCIsAvailable,
  CDCIsReadyToOpen,
  CDCOpenChannel,
  CDCCloseChannel,
  CDCSend,
  CDCCanSend,
  CDCMaxPacketSize
};

