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

// Here's the deal:
// We want to measure the time between two edges and report it. We do not want
// our reports to happen more often than 200 times a second.
// We set timer 5 to fire an interrupt 200 times a second and from within this
// interrupt we start a capture on any modules that should be enabled and are
// not currently capturing.
//
// When measuring pulses (rising-to-falling or falling-to-rising):
// Once the first edge is detected, an incap interrupt will fire. We set this
// interrupt priority to 6, so that we can quickly prepare to the next edge
// before we miss it. Then we can reduce the priority back to 1. When the next
// interrupt comes, we have two time values in the incap FIFO, which we can
// subtract to obtain the desired delta, and we send a report to the client.
// Then, we can turn off the module and re-arm it to be triggered on the next
// timer interrupt.
//
// When measuring period (rising-to-rising):
// This is simpler: we set the module to fire after two captures and we set the
// interrupt priority directly to 1. Then we handle the interrupt the same as
// we handle the trailing edge of the first case.

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

static INCAP_REG* incap_regs = (INCAP_REG *) & IC1CON1;

typedef enum {
  LEADING = 0,
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

// A bit mask, in which if bit k is set, module number k needs to be turned on
// upon the next timer interrupt.
static unsigned armed = 0;

static void InCapConfigInternal(int incap_num, int double_prec, int mode,
                                int clock, int external);

void InCapInit() {
  log_printf("InCapInit()");
  int i;
  _T5IE = 0; // Make sure we don't trigger new captures.
  for (i = 0; i < NUM_INCAP_MODULES; ++i) {
    InCapConfigInternal(i, 0, 0, 0, 0);
  }
  // Now we're safe - all modules are off and all interrupts are clear.
  armed = 0;

  PR5 = 312;  // 5ms period = 200Hz
  TMR5 = 0x0000;
  _T5IF = 0;
  _T5IP = 1;
  _T5IE = 1; // Now our timer will start firing 200 times a second.
}

static inline void InCapArm(int incap_num, int double_prec) {
  _T5IE = 0;
  if (double_prec) {
    armed |= 3 << incap_num;
  } else {
    armed |= 1 << incap_num;
  }
  _T5IE = 1;
}

static inline void InCapDisarm(int incap_num, int double_prec) {
  _T5IE = 0;
  if (double_prec) {
    armed &= ~(3 << incap_num);
  } else {
    armed &= ~(1 << incap_num);
  }
  _T5IE = 1;
}

static void InCapConfigInternal(int incap_num, int double_prec, int mode,
                                int clock, int external) {
  INCAP_REG * const reg = incap_regs + incap_num;
  INCAP_REG * const reg2 = reg + 1;
  OUTGOING_MESSAGE msg;
  msg.type = INCAP_STATUS;
  msg.args.incap_status.incap_num = incap_num;

  InCapDisarm(incap_num, double_prec);
  Set_ICIE[incap_num](0); // Disable interrupts

  // We're safe here - nobody will touch the variables we're modifying.

  // Turn off the module.
  reg->con1 = 0x0000;
  if (double_prec) {
    reg2->con1 = 0x0000;
  }

  if (mode) {
    // Whether to flip, indexed by (mode - 1)
    static const unsigned FLIPS[] = {1, 1, 0, 0, 0};
    // The ICM and ICI bits values to use, indexed by (mode - 1)
    static const unsigned int ICM_ICI[] = {3, 2, 3 | (1 << 5), 4 | (1 << 5), 5 | (1 << 5)};
    // The ICTSEL (clock select) bits values to use, indexed by clock
    static const unsigned int ICTSEL[] = {7 << 10, 0 << 10, 2 << 10, 3 << 10};

    if (external) {
      msg.args.incap_status.enabled = 1;
      AppProtocolSendMessage(&msg);
    }
    flip[incap_num] = FLIPS[mode - 1];
    // We move straight to the trailing edge if we don't need to flip.
    edge_states[incap_num] = flip[incap_num] ? LEADING : TRAILING;
    if (double_prec) {
      // Enable 32-bit mode
      reg2->con2 = (1 << 8);
      reg->con2 = (1 << 8);
    } else {
      reg->con2 = 0;
    }
    // Prepare the values required to turn on the module in the right mode in
    // con1_vals, to be picked up by the timer interrupt.
    con1_vals[incap_num] = ICTSEL[clock] | ICM_ICI[mode - 1];
    if (double_prec) {
      con1_vals[incap_num + 1] = con1_vals[incap_num];
    }

    Set_ICIF[incap_num](0); // Clear interrupts
    Set_ICIP[incap_num](edge_states[incap_num] == LEADING ? 6 : 1);  // First edge is high-priority.
    Set_ICIE[incap_num](1); // Enable interrupts

    InCapArm(incap_num, double_prec);
    // The next T5 interrupt will enable the module.
  } else {
    if (external) {
      msg.args.incap_status.enabled = 0;
      AppProtocolSendMessage(&msg);
    }
  }
}

void InCapConfig(int incap_num, int double_prec, int mode, int clock) {
  log_printf("InCapConfig(%d, %d, %d, %d)", incap_num, double_prec, mode,
             clock);
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
  INCAP_REG * const reg = incap_regs + incap_num;
  INCAP_REG * const reg2 = reg + 1;
  int size;
  DWORD_VAL delta_time;
  OUTGOING_MESSAGE msg;
  msg.type = INCAP_REPORT;
  msg.args.incap_report.incap_num = incap_num;
  if (double_prec) {
    assert(reg->con1 & (1 << 3));  // Buffer not empty.
    assert(reg2->con1 & (1 << 3));  // Buffer not empty.
    DWORD_VAL base = {.word.HW = reg2->buf,
      .word.LW = reg->buf};
    // 32-bit mode
    assert(reg->con1 & (1 << 3));  // Buffer not empty.
    assert(reg2->con1 & (1 << 3));  // Buffer not empty.
    delta_time.word.HW = reg2->buf;
    delta_time.word.LW = reg->buf;
    delta_time.Val -= base.Val;
    log_printf("%lu", delta_time.Val);
    size = NumBytes32(delta_time.Val);
  } else {
    // 16-bit mode
    assert(reg->con1 & (1 << 3));  // Buffer not empty.
    const WORD base = reg->buf;
    assert(reg->con1 & (1 << 3));  // Buffer not empty.
    delta_time.word.LW = reg->buf - base;
    log_printf("%u", delta_time.word.LW);  // TEMP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    size = NumBytes16(delta_time.word.LW);
  }
  msg.args.incap_report.size = size;
  AppProtocolSendMessageWithVarArg(&msg, &delta_time, size);
}

static void ICInterrupt(int incap_num) {
  Set_ICIF[incap_num](0); // Clear fast - don't want to miss any edge!

  INCAP_REG * const reg = incap_regs + incap_num;
  INCAP_REG * const reg2 = reg + 1;
  const int double_prec = reg->con2 & (1 << 8);

  // Toggle bit 0 of con1 to invert edge polarity if we need to.
  const unsigned f = flip[incap_num];
  reg->con1 ^= f;
  if (double_prec) {
    reg2->con1 ^= f;
  }

  if (edge_states[incap_num] == LEADING) {
    // We're on the first edge of a pulse. Should never get here on rising-to-
    // rising or falling-to-falling measurements.
    edge_states[incap_num] = TRAILING;
    Set_ICIP[incap_num](1);
  } else {
    // We're on the second edge.
    ReportCapture(incap_num, double_prec);
    // Disable the module. T5 will enable it.
    reg->con1 = 0x0000;
    if (double_prec) {
      reg2->con1 = 0x0000;
    }
    // Clear again - we might have gotten another interrupt by now. Module is
    // off now, won't get another one.
    Set_ICIF[incap_num](0);

    // For non-flipping modes, we're always on the trailing edge, and we get
    // an interrupt every two captures.
    if (f) {
      edge_states[incap_num] = LEADING;
      Set_ICIP[incap_num](6);
    }

    InCapArm(incap_num, double_prec); // Ready to go again.
  }
}

void __attribute__((__interrupt__, auto_psv)) _T5Interrupt() {
  // Trigger all the armed modules by copying the value from con1_vals to their
  // con1 register.
  // It is important that we do this in reverse order, since in cascade (32-bit)
  // mode, the higher module needs to be started first.
  // At the end of the process - everything is un-armed.
#define MASK (1 << (NUM_INCAP_MODULES - 1))
  int i = NUM_INCAP_MODULES;
  while (armed) {
    --i;
    if (armed & MASK) {
      incap_regs[i].con1 = con1_vals[i];
      armed &= ~MASK;
    }
    armed <<= 1;
  }
  _T5IF = 0; // clear
#undef MASK
}

#define DEFINE_INTERRUPT(num, unused) \
void __attribute__((__interrupt__, auto_psv)) _IC##num##Interrupt() { \
  ICInterrupt(num - 1); \
}

REPEAT_1B(DEFINE_INTERRUPT, NUM_INCAP_MODULES)
