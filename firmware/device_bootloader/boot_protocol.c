/*
 * Copyright 2012 Ytai Ben-Tsvi. All rights reserved.
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

#include "boot_protocol.h"

#include <assert.h>
#include <libpic30.h>
#include <stdint.h>

#include "board.h"
#include "boot_features.h"
#include "boot_protocol_defs.h"
#include "byte_queue.h"
#include "ioio_file.h"
#include "logging.h"
#include "version.h"
#include "USB/usb_function_cdc.h"

// platform ID:
// the platform ID is what maintains compatibility between the application
// firmware image and the board / bootloader.
// any application firmware image is given a platform ID for which it was built,
// similarly, the bootloader holds a platform ID uniquely designating its own
// binary interface and the underlying hardware.
// The application talking to the bootloader must check the platform ID reported
// by the bootloader and insure that only a matching application image gets
// installed.
// boards that are completely electrically equivalent and have the same pin
// numbering scheme and the same bootloader interface, will have identical
// platform IDs.
//
// note that when BLAPI changes, this list will need to be completely rebuilt
// with new numbers per hardware version.
#if BOARD_VER == BOARD_SPRK0020 || BOARD_VER == BOARD_PIXL0020
  #define PLATFORM_ID "IOIO0030"
#else
  #error Unknown board version - cannot determine platform ID
#endif

#define CHECK(cond) do { if (!(cond)) { log_printf("Check failed: %s", #cond); return FALSE; }} while(0)

const BYTE incoming_arg_size[MESSAGE_TYPE_LIMIT] = {
  sizeof(HARD_RESET_ARGS),
  sizeof(CHECK_INTERFACE_ARGS),
  sizeof(READ_FINGERPRINT_ARGS),
  sizeof(WRITE_FINGERPRINT_ARGS),
  sizeof(WRITE_IMAGE_ARGS)
  // BOOKMARK(add_feature): Add sizeof (argument for incoming message).
  // Array is indexed by message type enum.
};

const BYTE outgoing_arg_size[MESSAGE_TYPE_LIMIT] = {
  sizeof(ESTABLISH_CONNECTION_ARGS),
  sizeof(CHECK_INTERFACE_RESPONSE_ARGS),
  sizeof(FINGERPRINT_ARGS),
  sizeof(RESERVED_ARGS),
  sizeof(CHECKSUM_ARGS)
  // BOOKMARK(add_feature): Add sizeof (argument for outgoing message).
  // Array is indexed by message type enum.
};

DEFINE_STATIC_BYTE_QUEUE(tx_queue, 128);
static int bytes_out;
static int max_packet;

typedef enum {
  WAIT_TYPE,
  WAIT_ARGS,
  WAIT_FILE
} RX_MESSAGE_STATE;

static INCOMING_MESSAGE rx_msg;
static int rx_buffer_cursor;
static DWORD rx_message_remaining;
static RX_MESSAGE_STATE rx_message_state;
static uint16_t file_checksum;

static inline BYTE OutgoingMessageLength(const OUTGOING_MESSAGE* msg) {
  return 1 + outgoing_arg_size[msg->type];
}

void BootProtocolSendMessage(const OUTGOING_MESSAGE* msg) {
  ByteQueuePushBuffer(&tx_queue, (const BYTE*) msg, OutgoingMessageLength(msg));
}

void BootProtocolInit() {
  _prog_addressT p;
  bytes_out = 0;
  rx_buffer_cursor = 0;
  rx_message_remaining = 1;
  rx_message_state = WAIT_TYPE;
  ByteQueueClear(&tx_queue);
  max_packet = 64;

  OUTGOING_MESSAGE msg;
  msg.type = ESTABLISH_CONNECTION;
  msg.args.establish_connection.magic = BOOT_MAGIC;

  _init_prog_address(p, hardware_version);
  _memcpy_p2d16(msg.args.establish_connection.hw_impl_ver, p, 8);
  _init_prog_address(p, bootloader_version);
  _memcpy_p2d16(msg.args.establish_connection.bl_impl_ver, p, 8);

  memcpy(msg.args.establish_connection.plat_ver, PLATFORM_ID, 8);

  BootProtocolSendMessage(&msg);
}


void BootProtocolTasks() {
  if (USBUSARTIsTxTrfReady()) {
    const BYTE* data;
    if (bytes_out) {
      ByteQueuePull(&tx_queue, bytes_out);
      bytes_out = 0;
    }
    ByteQueuePeek(&tx_queue, &data, &bytes_out);
    if (bytes_out > 0) {
      if (bytes_out > max_packet) bytes_out = max_packet;
      putUSBUSART((char *) data, bytes_out);
    }
  }
}

static bool MessageDone() {
  // TODO: check pin capabilities
  switch (rx_msg.type) {
    case HARD_RESET:
      CHECK(rx_msg.args.hard_reset.magic == IOIO_MAGIC);
      HardReset();
      break;

    case CHECK_INTERFACE:
      CheckInterface(rx_msg.args.check_interface.interface_id);
      break;

    case READ_FINGERPRINT:
      if (!ReadFingerprint()) {
        log_printf("Failed to read fingerprint");
        return false;
      }
      break;

    case WRITE_FINGERPRINT:
      if (!WriteFingerprint(rx_msg.args.write_fingerprint.fingerprint)) {
        log_printf("Failed to write fingerprint");
        return false;
      }
      break;

    case WRITE_IMAGE:
      rx_message_state = WAIT_FILE;
      rx_message_remaining = rx_msg.args.write_image.size;
      if (!EraseFingerprint()) {
        log_printf("Failed to erase fingerprint");
        return false;
      }
      file_checksum = 0;
      log_printf("Starting to receive image file");
      IOIOFileInit();
      break;

    // BOOKMARK(add_feature): Add incoming message handling to switch clause.
    default:
      log_printf("Unexpected message type: 0x%x", rx_msg.type);
      return false;
  }
  return true;
}

bool BootProtocolProcess(const void* data, size_t data_len) {
  assert(!data_len || data);

  while (data_len > 0) {
    if (data_len >= rx_message_remaining) {
      if (rx_message_state == WAIT_FILE) {
        int i;
        for (i = 0; i < rx_message_remaining; ++i)
          file_checksum += ((const uint8_t *) data)[i];
        if (!IOIOFileHandleBuffer(data, rx_message_remaining)) return false;
      } else {
        // copy a chunk of data to rx_msg
        memcpy(((BYTE *) &rx_msg) + rx_buffer_cursor, data, rx_message_remaining);
        rx_buffer_cursor += rx_message_remaining;
      }
      data += rx_message_remaining;
      data_len -= rx_message_remaining;
      rx_message_remaining = 0;
    } else {
      if (rx_message_state == WAIT_FILE) {
        int i;
        for (i = 0; i < data_len; ++i)
          file_checksum += ((const uint8_t *) data)[i];
        if (!IOIOFileHandleBuffer(data, data_len)) return false;
      } else {
        // copy a chunk of data to rx_msg
        memcpy(((BYTE *) &rx_msg) + rx_buffer_cursor, data, data_len);
        rx_buffer_cursor += data_len;
      }
      rx_message_remaining -= data_len;
      data_len = 0;
    }

    // change state
    if (rx_message_remaining == 0) {
      switch (rx_message_state) {
        case WAIT_TYPE:
          rx_message_state = WAIT_ARGS;
          if (rx_msg.type >= MESSAGE_TYPE_LIMIT) {
            log_printf("Unexpected message type: 0x%x", rx_msg.type);
            return false;
          }
          rx_message_remaining = incoming_arg_size[rx_msg.type];
          if (rx_message_remaining) break;
          // fall-through on purpose

        case WAIT_ARGS:
          rx_message_state = WAIT_TYPE;
          rx_message_remaining = 1;
          rx_buffer_cursor = 0;
          if (!MessageDone()) return false;
          if (rx_message_remaining) break;
          // fall-through on purpose

        case WAIT_FILE:
          if (!IOIOFileDone()) {
            log_printf("Unexpected end of file");
            return false;
          } else {
            log_printf("Image done successfully");
          }
          SendChecksum(file_checksum);
          rx_message_state = WAIT_TYPE;
          rx_message_remaining = 1;
          break;
      }
    }
  }
  return true;
}
