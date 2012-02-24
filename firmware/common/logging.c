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

#include "logging.h"

#include <uart2.h>
#include <PPS.h>

#ifdef ENABLE_LOGGING

char char_buf[256];

void log_print_buf(const void* buf, int size) {
  const BYTE* byte_buf = (const BYTE*) buf;
  int s = size;
  while (size-- > 0) {
    UART2PutHex(*byte_buf++);
    UART2PutChar(' ');
  }
  UART2PutChar('\r');
  UART2PutChar('\n');

  byte_buf -= s;
  while (s-- > 0) {
    UART2PutChar(*byte_buf++);
  }
  UART2PutChar('\r');
  UART2PutChar('\n');
}

void log_init() {
  iPPSOutput(OUT_PIN_PPS_RP28,OUT_FN_PPS_U2TX);  // U2TX to pin 32
  UART2Init();
}

int write(int handle, void *buffer, unsigned int len) {
  if (handle == 1 || handle == 2) {
    const char *p = buffer;
    int i;
    for (i = 0; i < len; ++i) {
      UART2PutChar(*p++);
    }
    return len;
  } else {
    return -1;
  }
}


#endif  // DEBUG_MODE
