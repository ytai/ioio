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

#include "dumpsys.h"

#include <assert.h>

#include "bootloader_defs.h"

#define MAX_PATH 64

typedef enum {
  STATE_IN_PREFIX,
  STATE_SKIP_LINE,
  STATE_IN_PATH
} STATE;

static const char prefix[] FORCEROM = "    dataDir=";
static int cursor;
static char result[MAX_PATH];
static STATE state;

void DumpsysInit() {
  cursor = 0;
  state = STATE_IN_PREFIX;
}

const char* DumpsysProcess(const char* data, int size) {
  assert(data);

  while (size--) {
    char c = *data++;
    switch (state) {
      case STATE_IN_PREFIX:
        if (c == prefix[cursor++]) {
          if (cursor == sizeof(prefix) - 1) {
            state = STATE_IN_PATH;
            cursor = 0;
          }
        } else {
          state = STATE_SKIP_LINE;
        }
        break;

      case STATE_SKIP_LINE:
        if (c == '\n') {
          state = STATE_IN_PREFIX;
          cursor = 0;
        }
        break;

      case STATE_IN_PATH:
        if (c == '\r') {
          result[cursor] = '\0';
          return result;
        } else {
          if (cursor < MAX_PATH - 1) {
            result[cursor++] = c;
          } else {
            return DUMPSYS_ERROR;
          }
        break;
      }
    }
  }
  return DUMPSYS_BUSY;
}
