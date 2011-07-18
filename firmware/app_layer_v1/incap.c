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

#include "incap.h"

#include <assert.h>

#include "Compiler.h"
#include "platform.h"
#include "logging.h"
#include "pp_util.h"
#include "sync.h"
#include "protocol_defs.h"
#include "protocol.h"

DEFINE_REG_SETTERS_1B(NUM_INCAP_MODULES, _IC, IF)
DEFINE_REG_SETTERS_1B(NUM_INCAP_MODULES, _IC, IE)
DEFINE_REG_SETTERS_1B(NUM_INCAP_MODULES, _IC, IP)

typedef struct {
  unsigned int con1;
  unsigned int con2;
  unsigned int buf;
  unsigned int tmr;
} INCAP_REG;

static volatile INCAP_REG* incap_regs = (volatile INCAP_REG *) &IC1CON1;

typedef enum {
  LEADING,
  TRAILING,
  FREQ,
  FREQ_FIRST
} EDGE_STATE;

static EDGE_STATE edge_states[NUM_INCAP_MODULES];
static unsigned int timer_base[NUM_INCAP_MODULES];
static unsigned int ready_to_send;

void InCapInit() {
  log_printf("InCapInit()");
  int i;
  for (i = 0; i < NUM_INCAP_MODULES; ++i) {
    InCapConfig(i, 0, 0);
  }
  PR5 = 0x0138;  // 5ms period = 200Hz
  TMR5 = 0x0000;
  _T5IE = 1;
}

void InCapConfig(int incap_num, int mode, int clock) {
  volatile INCAP_REG* reg = incap_regs + incap_num;
  OUTGOING_MESSAGE msg;
  msg.type = INCAP_STATUS;
  msg.args.incap_status.incap_num = incap_num;
  log_printf("InCapConfig(%d, %d, %d)", incap_num, mode, clock);
  Set_ICIE[incap_num](0);  // disable interrupts
  reg->con1 = 0x0000;      // disable module
  reg->con2 = 0x0000;
  Set_ICIF[incap_num](0);  // clear interrupts

  // the ICM bits values to use, indexed by (mode - 1)
  static const unsigned int icm[] = { 3, 2, 3, 4, 5 };
  // the initial edge state to use, indexed by (mode - 1)
  static const EDGE_STATE initial_edge[] = { LEADING, LEADING, FREQ_FIRST, FREQ_FIRST, FREQ_FIRST };
  // the ICTSEL (clock select) bits values to use, indexed by clock
  static const unsigned int ictsel[] = { 7 << 10, 0 << 10, 2 << 10, 3 << 10};

  if (mode) {
    msg.args.incap_status.enabled = 1;
    AppProtocolSendMessage(&msg);
    ready_to_send |= (1 << incap_num);
    EDGE_STATE edge = initial_edge[mode - 1];
    edge_states[incap_num] = edge;
    Set_ICIP[incap_num](edge == LEADING ? 5 : 1);
    Set_ICIE[incap_num](1); // enable interrupts
    reg->con1 = ictsel[clock] | icm[mode - 1];
  } else {
    msg.args.incap_status.enabled = 0;
    AppProtocolSendMessage(&msg);
  }
}

static void ICInterrupt(int incap_num) {
  volatile INCAP_REG* reg = incap_regs + incap_num;

  while (reg->con1 & 0x0008) {  // buffer not empty
    EDGE_STATE edge = edge_states[incap_num];
    unsigned int timer_val = reg->buf;

    switch (edge) {
      case LEADING:
        reg->con1 ^= 1;  // toggle bit 0 of con1: inverts edge polarity
        edge_states[incap_num] = TRAILING;
        Set_ICIP[incap_num](1);
        break;

      case TRAILING:
        reg->con1 ^= 1;  // toggle bit 0 of con1: inverts edge polarity
        edge_states[incap_num] = LEADING;
        SyncInterruptLevel(5);  // mask level 5 interrupts or otherwise will get
                                // this interrupt nesting,
        Set_ICIP[incap_num](5);
        // fall-through on purpose
      case FREQ:
        if (ready_to_send & (1 << incap_num)) {
          OUTGOING_MESSAGE msg;
          msg.type = INCAP_REPORT;
          msg.args.incap_report.incap_num = incap_num;
          msg.args.incap_report.delta_time = timer_val - timer_base[incap_num];
          AppProtocolSendMessage(&msg);
          ready_to_send &= ~(1 << incap_num);
        }
        break;

      case FREQ_FIRST:
        edge_states[incap_num] = FREQ;
        break;
    }
    timer_base[incap_num] = timer_val;
  }
  Set_ICIF[incap_num](0);
}

void __attribute__((__interrupt__, auto_psv)) _T5Interrupt() {
  ready_to_send = 0xFFFF;
  _T5IF = 0;  // clear
}

#define DEFINE_INTERRUPT(num, unused) \
void __attribute__((__interrupt__, auto_psv)) _IC##num##Interrupt() { \
  ICInterrupt(num - 1); \
}

REPEAT_1B(DEFINE_INTERRUPT, NUM_INCAP_MODULES)
