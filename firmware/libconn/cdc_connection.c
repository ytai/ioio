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

#include "cdc_connection.h"

#include <assert.h>
#include <stdint.h>

#include "logging.h"
#define USB_SUPPORT_DEVICE
#include "USB/usb.h"
#include "USB/usb_function_cdc.h"

#define __MIN(a,b) ((a) < (b) ? (a) : (b))

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
  rx_buf_size = __MIN(size, 0xFF);
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

