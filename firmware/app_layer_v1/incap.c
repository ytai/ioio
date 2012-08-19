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
#include "uart2.h"

DEFINE_REG_SETTERS_1B(NUM_INCAP_MODULES, _IC, IF)
DEFINE_REG_SETTERS_1B(NUM_INCAP_MODULES, _IC, IE)
DEFINE_REG_SETTERS_1B(NUM_INCAP_MODULES, _IC, IP)

typedef struct {
  volatile unsigned int con1;
  volatile unsigned int con2;
  volatile unsigned int buf;
  volatile unsigned int tmr;
} INCAP_REG;

static INCAP_REG* incap_regs = (INCAP_REG *) &IC1CON1;

typedef enum {
  LEADING  = 0,
  TRAILING = 1
} EDGE_STATE;

// Records for each module whether the next interrupt is a leading edge or a
// traling edge.
static EDGE_STATE edge_states[NUM_INCAP_MODULES];

// For each module: the value that needs to be written to con1 in order to
// enable the module in the right mode.
static WORD con1_vals[NUM_INCAP_MODULES];

// For each module, if true, edge type will be flipped on every interrupt.
// This is used for measuring pulses, as opposed to period.
static unsigned flip[NUM_INCAP_MODULES];

static void InCapConfigInternal(int incap_num, int double_prec, int mode,
                                int clock, int external);

void InCapInit() {
  log_printf("InCapInit()");
  int i;
  for (i = 0; i < NUM_INCAP_MODULES; ++i) {
    InCapConfigInternal(i, 0, 0, 0, 0);
  }
  PR5 = 0x0138;  // 5ms period = 200Hz
//  PR5 = 0x0138 * 200;  // 5ms period = 200Hz
  TMR5 = 0x0000;
  _T5IP = 1;
  _T5IE = 1;
}

static void InCapConfigInternal(int incap_num, int double_prec, int mode,
                                int clock, int external) {
  INCAP_REG* const reg = incap_regs + incap_num;
  INCAP_REG* const reg2 = reg + 1;
  OUTGOING_MESSAGE msg;
  msg.type = INCAP_STATUS;
  msg.args.incap_status.incap_num = incap_num;
  if (external) {
    log_printf("InCapConfig(%d, %d, %d, %d)", incap_num, double_prec, mode,
               clock);
  }
  
  Set_ICIE[incap_num](0);  // disable interrupts

  unsigned t5ie = _T5IE;
  _T5IE = 0;
  con1_vals[incap_num] = 0x0000;
  reg->con1 = 0x0000;      // disable module
  reg->con2 = 0x0000;
  if (double_prec) {
    con1_vals[incap_num + 1] = 0x0000;
    reg2->con1 = 0x0000;
    reg2->con2 = 0x0000;
  }
  _T5IE = t5ie;
  Set_ICIF[incap_num](0);  // clear interrupts

  // Whether to flip, indexed by (mode - 1)
  static const unsigned flips[] = { 1, 1, 0, 0, 0 };
  // The ICM and ICI bits values to use, indexed by (mode - 1)
  static const unsigned int icm_ici[] = { 3, 2 , 3 | (1 << 5), 4 | (1 << 5), 5 | (1 << 5)};
  // The ICTSEL (clock select) bits values to use, indexed by clock
  static const unsigned int ictsel[] = { 7 << 10, 0 << 10, 2 << 10, 3 << 10};

  if (mode) {
    if (external) {
      msg.args.incap_status.enabled = 1;
      AppProtocolSendMessage(&msg);
    }
    edge_states[incap_num] = LEADING;
    flip[incap_num] = flips[mode - 1];
    Set_ICIP[incap_num](6);
    Set_ICIE[incap_num](1); // enable interrupts
    if (double_prec) {
      reg2->con2 = (1 << 8);
      reg->con2 = (1 << 8);
    }
    unsigned t5ie = _T5IE;
    _T5IE = 0;
    if (double_prec) {
      con1_vals[incap_num + 1] = ictsel[clock] | icm_ici[mode - 1];
    }
    con1_vals[incap_num] = ictsel[clock] | icm_ici[mode - 1];
    _T5IE = t5ie;
    // The next T5 interrupt will enable the module.
  } else {
    if (external) {
      msg.args.incap_status.enabled = 0;
      AppProtocolSendMessage(&msg);
    }
  }
}

void InCapConfig(int incap_num, int double_prec, int mode, int clock) {
  InCapConfigInternal(incap_num, double_prec, mode, clock, 1);
}

inline static int NumBytes16(WORD val) {
  return val > 0xFF ? 2 : 1;
}

inline static int NumBytes32(DWORD val) {
  if (((DWORD_VAL) val).word.HW) {
    return 2 + NumBytes16(((DWORD_VAL) val).word.HW);
  } else {
    return NumBytes16(((DWORD_VAL) val).word.LW);
  }
}

inline static void ReportCapture(int incap_num, int double_prec) {
  INCAP_REG* const reg = incap_regs + incap_num;
  INCAP_REG* const reg2 = reg + 1;
  int size;
  DWORD_VAL delta_time;
  OUTGOING_MESSAGE msg;
  msg.type = INCAP_REPORT;
  msg.args.incap_report.incap_num = incap_num;
  if (double_prec) {
    DWORD_VAL base = { .word.HW = reg2->buf,
                       .word.LW = reg->buf };
    // 32-bit mode
    delta_time.word.HW = reg2->buf;
    delta_time.word.LW = reg->buf;
    delta_time.Val -= base.Val;
    size = NumBytes32(delta_time.Val);
  } else {
    // 16-bit mode
    WORD base = reg->buf;
    delta_time.word.LW =  reg->buf - base;
    size = NumBytes16(delta_time.word.LW);
  }
  msg.args.incap_report.size = size;
  AppProtocolSendMessageWithVarArg(&msg, &delta_time, size);
}

inline static void FlipEdge(int incap_num, int double_prec) {
  // Toggle bit 0 of con1: inverts edge polarity.
  const unsigned f = flip[incap_num];
  con1_vals[incap_num] ^= f;
  if (double_prec) {
    con1_vals[incap_num + 1] ^= f;
  }
}

static void ICInterrupt(int incap_num) {
  INCAP_REG* const reg = incap_regs + incap_num;
  INCAP_REG* const reg2 = reg + 1;
  const int double_prec = reg->con2 & (1 << 8);

  FlipEdge(incap_num, double_prec);
  if (edge_states[incap_num] == LEADING) {
    reg->con1 = con1_vals[incap_num];
    if (double_prec) {
      reg2->con1 = con1_vals[incap_num + 1];
    }
    edge_states[incap_num] = TRAILING;
  } else {
    ReportCapture(incap_num, double_prec);
    // Disable the module. T5 will enable it.
    reg->con1 = 0x0000;
    if (double_prec) {
      reg2->con1 = 0x0000;
    }
    edge_states[incap_num] = LEADING;
  }
  Set_ICIF[incap_num](0);
  Set_ICIP[incap_num](edge_states[incap_num] == LEADING ? 6 : 1);
}

void __attribute__((__interrupt__, auto_psv)) _T5Interrupt() {
  int i = NUM_INCAP_MODULES;
  while (i-- > 0) {
    incap_regs[i].con1 = con1_vals[i];
  }
  _T5IF = 0;  // clear
}

#define DEFINE_INTERRUPT(num, unused) \
void __attribute__((__interrupt__, auto_psv)) _IC##num##Interrupt() { \
  ICInterrupt(num - 1); \
}

REPEAT_1B(DEFINE_INTERRUPT, NUM_INCAP_MODULES)
