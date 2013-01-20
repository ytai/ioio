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

#include "rgb_led_matrix.h"

#include <string.h>
#include <p24Fxxxx.h>

// Connections:
//
// (r1 g1 b1 r2 g2 b2) (24 .. 19)
// clk                 25
// (a b c)             (7 10 11)
// lat                 27
// oe                  28

// Ordering:
//
// Pixels sent from left to right.
// MSB is upper half.

#define DATA_CLK_PORT LATE
#define CLK_PIN _LATE6
#define ADDR_PORT LATD
#define LAT_PIN _LATG6
#define OE_PIN _LATG7

#define SUB_FRAMES_PER_FRAME 3
#define ROWS_PER_SUB_FRAME 8

typedef uint8_t bin_row_t[64];
typedef bin_row_t bin_frame_t[ROWS_PER_SUB_FRAME];
typedef bin_frame_t frame_t[SUB_FRAMES_PER_FRAME];

static frame_t frames[2] __attribute__((far));
static const uint8_t *data;
static int sub_frame;
static int displayed_frame;
static int back_frame_ready;
static int shifter_repeat;
static int address = 0;

//static const frame_t DEFAULT_FRAME = {
//#include "default_frame.inl"
//};

void RgbLedMatrixEnable(int shifter_len_32) {
  _T4IE = 0;

  if (shifter_len_32) {
    LATE = 0;
    address = 7;
    LATG = (1 << 7);

    ODCE &= ~0x7F;
    ODCD &= ~0x7;
    ODCG &= ~0xC0;

    TRISE &= ~0x7F;
    TRISD &= ~0x7;
    TRISG &= ~0xC0;

    memset(frames[0], 0, sizeof(frames[0]));
    // memcpy(frames[0], DEFAULT_FRAME, sizeof(frames[0]));
    data = (const uint8_t *) frames[0];
    sub_frame = SUB_FRAMES_PER_FRAME;
    displayed_frame = 0;
    back_frame_ready = 0;
    shifter_repeat = shifter_len_32;

    // timer 4 is sysclk / 64 = 250KHz
    PR4 = 1;
    TMR4 = 0x0000;
    _T4IP = 7;
    _T4IE = 1;
  } else {
    TRISE |= 0x7F;
    TRISD |= 0x7;
    TRISG |= 0xC0;
  }
}

void RgbLedMatrixFrame(const uint8_t frame[]) {
  back_frame_ready = 0;
  memcpy(frames[displayed_frame ^ 1], frame, 768 * shifter_repeat);
  back_frame_ready = 1;
}

static void draw_row() {
  int i;
  if (++address == ROWS_PER_SUB_FRAME) {
    // sub-frame done
    address = 0;
    if (++sub_frame == SUB_FRAMES_PER_FRAME + 1) {
      // frame done
      sub_frame = 0;

      if (back_frame_ready) {
        displayed_frame = displayed_frame ^ 1;
        back_frame_ready = 0;
      }
      data = (const uint8_t *) frames[displayed_frame];
    }
  }

  if (sub_frame == SUB_FRAMES_PER_FRAME) {
    OE_PIN = 1; // black
    return;
  }

  for (i = shifter_repeat; i > 0; --i) {
    // push 32 bytes
  #define dump DATA_CLK_PORT = *data++; CLK_PIN = 1;
    dump dump dump dump dump dump dump dump
    dump dump dump dump dump dump dump dump
    dump dump dump dump dump dump dump dump
    dump dump dump dump dump dump dump dump
  #undef dump
  }

  OE_PIN = 1; // black
  // latch
  LAT_PIN = 1;
  LAT_PIN = 0;
  ADDR_PORT = address;

  OE_PIN = 0; // enable output
}

int RgbLedMatrixFrameSize() {
  return shifter_repeat * 32 * ROWS_PER_SUB_FRAME * SUB_FRAMES_PER_FRAME;
}

static unsigned int times[] = {
  6, 12, 36, 250
};

void __attribute__((__interrupt__, auto_psv)) _T4Interrupt() {
  draw_row();
  PR4 = times[sub_frame];
  TMR4 = 0;
  _T4IF = 0; // clear
}

