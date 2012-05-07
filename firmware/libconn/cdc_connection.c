#include "cdc_connection.h"

#include <assert.h>
#include <stdint.h>

#include "usb_function_cdc.h"
#include "usb_device.h"
#include "logging.h"
#include "USB/usb_common.h"

typedef enum {
  CHANNEL_DETACHED,
  CHANNEL_WAIT_OPEN,
  CHANNEL_OPEN,
  CHANNEL_WAIT_CLOSED
} CHANNEL_STATE;

static void DummyCallback(const void *data, UINT32 size, int_or_ptr_t arg) {
}

static ChannelCallback callback = &DummyCallback;
static int_or_ptr_t callback_arg;
static void *rx_buf;
static int rx_buf_size;
static CHANNEL_STATE channel_state;
static uint8_t is_channel_open;

static void CDCInit(void *buf, int size) {
  rx_buf = buf;
  rx_buf_size = size;
  is_channel_open = 0;
  channel_state = CHANNEL_DETACHED;
}

static void CDCTasks() {
  DWORD size;
  BYTE err;

  // handle detach
  if (channel_state > CHANNEL_DETACHED
      && USBGetDeviceState() == DETACHED_STATE) {
    if (is_channel_open) {
      callback(NULL, 1, callback_arg);
      is_channel_open = 0;
    }
    channel_state = CHANNEL_DETACHED;
  }

  switch (channel_state) {
    case CHANNEL_DETACHED:
      if (USBGetDeviceState() == CONFIGURED_STATE) {
        channel_state = CHANNEL_WAIT_OPEN;
      }
      break;

    case CHANNEL_WAIT_OPEN:
      if (1 == getsUSBUSART(rx_buf, 1)) {
        log_printf("Remote end requested open channel.");
        putUSBUSART(&is_channel_open, 1);
        if (is_channel_open) {
          channel_state = CHANNEL_OPEN;
        } else {
          channel_state = CHANNEL_WAIT_CLOSED;
        }
      }
      break;

    case CHANNEL_OPEN:
      size = getsUSBUSART(rx_buf, rx_buf_size);
      if (size) {
        callback(rx_buf, size, callback_arg);
      }
      break;

    case CHANNEL_WAIT_CLOSED:
      if (1 == getsUSBUSART(rx_buf, 1)) {
        callback(NULL, 0, callback_arg);
        channel_state = CHANNEL_INIT;
      }
      break;
  }
}

static int CDCOpenChannel(ChannelCallback cb, int_or_ptr_t open_arg,
                          int_or_ptr_t cb_args) {
  assert(channel_state < CHANNEL_OPEN);

  callback = cb;
  callback_arg = cb_args;
  is_channel_open = 1;
  return 0;
}

static void CDCCloseChannel(int h) {
  assert(h == 0);
  assert(channel_state == CHANNEL_OPEN);
  is_channel_open = 0;
  channel_state = CHANNEL_WAIT_CLOSED;
}

static void CDCSend(int h, const void *data, int size) {
  assert(h == 0);
  assert(channel_state == CHANNEL_OPEN);
  putUSBUSART(data, size);
}

static int CDCCanSend(int h) {
  assert(h == 0);
  assert(channel_state <= CHANNEL_OPEN);
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
  return INT_MAX; // unlimited
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

