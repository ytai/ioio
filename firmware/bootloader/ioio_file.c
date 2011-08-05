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

#include <string.h>

#include "logging.h"
#include "flash.h"
#include "ioio_file.h"
#include "bootloader_defs.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

typedef enum {
  IOIO_FILE_STATE_WAIT_HEADER,
  IOIO_FILE_STATE_WAIT_BLOCK
} IOIO_FILE_STATE;

static BYTE ioio_file_buf[196];
static BYTE ioio_file_buf_pos;
static BYTE ioio_file_field_remaining;
static DWORD ioio_file_last_page;
static IOIO_FILE_STATE ioio_file_state;
static const BYTE IOIO_FILE_HEADER[8] = { 'I', 'O', 'I', 'O', '\1', '\0', '\0', '\0' };

static BOOL IOIOFileBlockDone() {
  DWORD address, page_address;
  switch (ioio_file_state) {
    case IOIO_FILE_STATE_WAIT_HEADER:
      if (memcmp(ioio_file_buf, IOIO_FILE_HEADER, 8) != 0) {
        log_printf("Unexpected IOIO file header or version");
        return FALSE;
      }
      ioio_file_buf_pos = 0;
      ioio_file_field_remaining = 196;
      ioio_file_state = IOIO_FILE_STATE_WAIT_BLOCK;
      break;

    case IOIO_FILE_STATE_WAIT_BLOCK:
      address = *((const DWORD *) ioio_file_buf);
      if (address & 0x7F) {
        log_printf("Misaligned block: 0x%lx", address);
        return FALSE;
      }
      if (address < BOOTLOADER_MIN_APP_ADDRESS || address >= BOOTLOADER_MAX_APP_ADDRESS) {
        log_printf("Adderess outside of permitted range: 0x%lx", address);
        return FALSE;
      }
      if (ioio_file_last_page != BOOTLOADER_INVALID_ADDRESS && address < ioio_file_last_page) {
        log_printf("Out-of-order address: 0x%lx", address);
        return FALSE;
      }
      page_address = address & 0xFFFFFFC00ull;
      if (page_address != ioio_file_last_page) {
        log_printf("Erasing Flash page: 0x%lx", address);
        if (!FlashErasePage(page_address)) return FALSE;
        ioio_file_last_page = page_address;
      }
      log_printf("Writing Flash block: 0x%lx", address);
      if (!FlashWriteBlock(address, ioio_file_buf + 4)) return FALSE;
      ioio_file_buf_pos = 0;
      ioio_file_field_remaining = 196;
      break;
  }
  return TRUE;
}

void IOIOFileInit() {
  ioio_file_buf_pos = 0;
  ioio_file_field_remaining = 8;
  ioio_file_last_page = BOOTLOADER_INVALID_ADDRESS;
  ioio_file_state = IOIO_FILE_STATE_WAIT_HEADER;
}

BOOL IOIOFileHandleBuffer(const void * buffer, size_t size) {
  while (size) {
    size_t bytes_to_copy = MIN(ioio_file_field_remaining, size);
    memcpy(ioio_file_buf + ioio_file_buf_pos, buffer, bytes_to_copy);
    ioio_file_buf_pos += bytes_to_copy;
    ioio_file_field_remaining -= bytes_to_copy;
    buffer = ((const BYTE *) buffer) + bytes_to_copy;
    size -= bytes_to_copy;
    if (ioio_file_field_remaining == 0) {
      if (!IOIOFileBlockDone()) {
        return FALSE;
      }
    }
  }
  return TRUE;
}

BOOL IOIOFileDone() {
  if (ioio_file_state == IOIO_FILE_STATE_WAIT_BLOCK
      && ioio_file_field_remaining == 196) return TRUE;
  log_printf("Unexpected EOF");
  return FALSE;
}
