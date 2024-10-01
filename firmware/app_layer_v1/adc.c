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

#include "adc.h"

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <libpic30.h>


#include "Compiler.h"
#include "logging.h"
#include "protocol.h"
#include "pins.h"
#include "sync.h"

#define FAST_INT_PRIORITY 6
#define SLOW_INT_PRIORITY 1

static volatile uint16_t analog_scan_bitmask;
static volatile int analog_scan_num_channels;
// Bit k is set iff channel k is marked for capsense reporting.
static volatile uint16_t capsense_bitmask;
// Bit k is set iff the status of channel k has changed wrt capsense sampling
// and we have not yet reported back this fact. This bit is set whenever the
// status changes and cleared as soon as we report.
static volatile uint16_t capsense_dirty_bitmask;
// Current channel to capsense.
static uint16_t capsense_current = 15;
// Set to true before triggering a sample to designate this is a cap-sense
// sample.
static bool capsense_sample = false;
// Used to decide whether or not to enable T3 interrupt. When 0, interrupt
// should be enabled, otherwise, disabled.
static volatile int t3_int_counter;


// timer 3 is clocked @2MHz
// we set its period to 2000 so that a match occurs @1KHz
// used for ADC
static inline void Timer3Init() {
  PR3   = 1999;  // period is 2000 clocks = 1KHz
  _T3IP = SLOW_INT_PRIORITY; // interrupt priority 1 (this interrupt may write to outgoing channel)
}

static inline void T3IntBlock() {
  PRIORITY(1) {
    _T3IE = 0;
    ++t3_int_counter;
  }
}

static inline void T3IntUnblock() {
  PRIORITY(1) {
    if (--t3_int_counter == 0) {
      _T3IE = 1;
    }
  }
}

static inline void ADCStart() {
  // Clear any possibly remaining interrupts before enabling them.
  _AD1IF = 0;

  _AD1IE = 1;  // We can enable interrupts now, they won't fire.

  t3_int_counter = 0;
  // Reset counter and start triggering.
  TMR3  = 0x0000;  // reset counter
  _T3IF = 0;
  _T3IE = 1;  // We're ready to handle trigger interrupts.
}

// IMPORTANT:
// A post-condition of this function is that no interrupts (related to this
// module) will fire.
static inline void ADCStop() {
  // Disable interrupts. T3 must comes last, as the handlers of the others may
  // enable it.
  _AD1IE = 0;
  _T3IE = 0;
  // No more interrupts at this point.
  _CTMUEN = 0;  // CTMU off.
  _ADON = 0;    // ADC off
}

void ADCInit() {
  ADCStop();     // Just in case we were running. No interrupts after this point.
  Timer3Init();  // initialize the timer. stopped if already running.

  // Now nothing will interrupt us
  AD1CON1 = 0x0000;  // ADC off.
  AD1CON2 = 0x0000;  // Avdd Avss ref, single buffer, interrupt on every sample
  AD1CON3 = 0x1F01;  // system clock, 31 Tad acquisition time, ADC clock @8MHz
  AD1CHS  = 0x0000;  // Sample AN0 against negative reference.
  AD1CSSL = 0x0000;  // reset scan mask.

  _AD1IP = FAST_INT_PRIORITY;        // high priority to stop automatic sampling

  // Setup CTMU
  CTMUCON = (1 << 8)  // CTTRIG
            | (0x3 << 5) // EDG2SEL
            | (0x3 << 2) // EDG1SEL
            | (1 << 4);  // EDG1POL;
  CTMUICON = 0x7F00; //89.1uA

  analog_scan_bitmask = 0x0000;
  analog_scan_num_channels = 0;
  capsense_bitmask = 0x0000;
  capsense_dirty_bitmask = 0x0000;
}

static inline int CountOnes(unsigned int val) {
  int res = 0;
  while (val) {
    if (val & 1) ++res;
    val >>= 1;
  }
  return res;
}

static inline void ReportAnalogInStatus() {
  volatile unsigned int* buf = &ADC1BUF0;
  int num_channels = CountOnes(AD1CSSL);
  int i;
  BYTE var_arg[16 / 4 * 5];
  int var_arg_pos = 0;
  int group_header_pos;
  int pos_in_group;
  int value;
  OUTGOING_MESSAGE msg;
  msg.type = REPORT_ANALOG_IN_STATUS;
  for (i = 0; i < num_channels; i++) {
    pos_in_group = i & 3;
    if (pos_in_group == 0) {
      group_header_pos = var_arg_pos;
      var_arg[var_arg_pos++] = 0;  // reset header
    }
    value = buf[i];
    //log_printf("%d", value);
    var_arg[group_header_pos] |= (value & 3) << (pos_in_group * 2);  // two LSb to group header
    var_arg[var_arg_pos++] = value >> 2;  // eight MSb to channel byte
  }
  AppProtocolSendMessageWithVarArg(&msg, var_arg, var_arg_pos);
}

static inline void ReportCapSense() {
  OUTGOING_MESSAGE msg;
  msg.type = CAPSENSE_REPORT;
  msg.args.capsense_report.pin = PinFromAnalogChannel(capsense_current);
  msg.args.capsense_report.value = ADC1BUF0;
  AppProtocolSendMessage(&msg);
}

static inline void ReportAnalogInFormat() {
  unsigned int mask = analog_scan_bitmask;
  int channel = 0;
  BYTE var_arg[16];
  int var_arg_pos = 0;
  OUTGOING_MESSAGE msg;
  msg.type = REPORT_ANALOG_IN_FORMAT;
  msg.args.report_analog_in_format.num_pins = analog_scan_num_channels;
  while (mask) {
    if (mask & 1) {
      var_arg[var_arg_pos++] = PinFromAnalogChannel(channel);
    }
    mask >>= 1;
    ++channel;
  }
  AppProtocolSendMessageWithVarArg(&msg, var_arg, var_arg_pos);
  AD1CSSL = analog_scan_bitmask;
}


static inline void ReportModifiedCapSenseStatus() {
  int i = 0;
  OUTGOING_MESSAGE msg;
  msg.type = SET_CAPSENSE_SAMPLING;

  uint16_t mask = capsense_bitmask;  // Local copy.
  while (capsense_dirty_bitmask) {
    if (capsense_dirty_bitmask & 1) {
      msg.args.set_capsense_sampling.pin = PinFromAnalogChannel(i);
      msg.args.set_capsense_sampling.enable = mask & 1;
      AppProtocolSendMessage(&msg);
    }
    capsense_dirty_bitmask >>= 1;
    mask >>= 1;
    ++i;
  }
}

static inline void ADCTrigger() {
  assert(analog_scan_num_channels != 0);

  _SMPI = analog_scan_num_channels - 1;
  _SSRC = 7;  // auto-convert.
  _CSCNA = 1; // scan channels set in AD1CSSL
  capsense_sample = false;  // let the ISR know this is an analog scan.

  // Turn the module on and start and automatic (scan) sample.
  _ADON = 1;
  _ASAM = 1;
}

static inline void ADCCapSenseTrigger() {
  assert(capsense_bitmask);

  // Cycle...
  capsense_current = (capsense_current + 1) & 0x0F;
  
  if ((1 << capsense_current) & capsense_bitmask) {
    _CTMUEN = 1;  // CTMU on.
    _IDISSEN = 1;  // discharge ADC internal cap.
    _CH0SA = capsense_current;  // select channel to sample.
    _SMPI = 0;     // interrupt when done.
    _SSRC = 6;     // convert once CTMU current pulse is done.
    _CSCNA = 0;    // don't scan.
    capsense_sample = true;  // let the ISR know this is a capsense sample.
    // Stop discharging circuit.
    PinSetTris(PinFromAnalogChannel(capsense_current), 1);
    _IDISSEN = 0;  // Stop discharging internal cap.

    // Turn the module on and start manual sampling.
    _ADON = 1;
    _SAMP = 1;

    // Charge for 16 cycles (1us) at constant current.
    _EDG1STAT = 1; // Set edge1 - Start Charge
    __delay32(15);
    _EDG1STAT = 0; //Clear edge1 - Stop Charge - auto-triggers ADC conversion.
  } else {
    T3IntUnblock();
  }
}

void ADCSetScan(int pin, int enable) {
  log_printf("ADCSetScan(%d, %d)", pin, enable);
  int channel = PinToAnalogChannel(pin);
  int mask;
  if (channel == -1) return;
  mask = 1 << channel;
  if (!!(mask & analog_scan_bitmask) == enable) return;

  if (enable) {
    if (analog_scan_bitmask | capsense_bitmask) {
      // already running, just add the new channel
      T3IntBlock();
      // These two variables are shared with the triggering code, ran from
      // timer 3 interrupt context.
      ++analog_scan_num_channels;
      analog_scan_bitmask |= mask;
      T3IntUnblock();
    } else {
      // first channel, start running
      analog_scan_num_channels = 1;
      analog_scan_bitmask = mask;
      ADCStart();
    }
  } else {
    T3IntBlock();
    --analog_scan_num_channels;
    analog_scan_bitmask &= ~mask;
    if (analog_scan_bitmask | capsense_bitmask) {
      T3IntUnblock();
    } else {
      // This was the last channel. At this point no new samples will be
      // triggered, but we may be in the middle of a sample.
      ADCStop();
      // Now we're safe. Report the change in format.
      ReportAnalogInFormat();
    }
  }
}

void ADCSetCapSense(int pin, int enable) {
  log_printf("ADCSetCapSense(%d, %d)", pin, enable);
  int channel = PinToAnalogChannel(pin);
  int mask;
  if (channel == -1) return;
  mask = 1 << channel;
  if (!!(mask & capsense_bitmask) == enable) return;

  if (enable) {
    if (analog_scan_bitmask | capsense_bitmask) {
      // already running, just add the new channel
      T3IntBlock();
      // These two variables are shared with the triggering code, ran from
      // timer 3 interrupt context.
      capsense_bitmask |= mask;
      capsense_dirty_bitmask |= mask;
      T3IntUnblock();
    } else {
      // first channel, start running
      capsense_bitmask = mask;
      capsense_dirty_bitmask = mask;
      ADCStart();
    }
  } else {
    T3IntBlock();
    capsense_bitmask &= ~mask;
    capsense_dirty_bitmask |= mask;
    if (analog_scan_bitmask | capsense_bitmask) {
      T3IntUnblock();
    } else {
      // This was the last channel. At this point no new samples will be
      // triggered, but we may be in the middle of a sample.
      ADCStop();
      // Now we're safe. Report the change in format.
      ReportModifiedCapSenseStatus();
    }
  }
}

void __attribute__((__interrupt__, auto_psv)) _T3Interrupt() {
  // Report frame format of analog channels if changed.
  if (AD1CSSL != analog_scan_bitmask) {
    ReportAnalogInFormat();
  }
  assert(AD1CSSL == analog_scan_bitmask);

  // Report enabling / disabling of capsense channels.
  if (capsense_dirty_bitmask) {
    ReportModifiedCapSenseStatus();
  }
  assert(!capsense_dirty_bitmask);

  T3IntBlock();  // disable interrupts. will be re-enabled when sampling is done.
  // Sample!
  if (analog_scan_num_channels) {
    // Trigger ADC sequence, which will eventually trigger capsense.
    ADCTrigger();
  } else if (capsense_bitmask) {
    // Jump directly to capsense.
    ADCCapSenseTrigger();
  } else {
    assert(false);
  }
  _T3IF = 0;  // clear
}

static inline void ScanDoneBottomHalf() {
  if (capsense_sample) {
    _CTMUEN = 0; // CTMU off.
    // Discharge circuit.
    PinSetTris(PinFromAnalogChannel(capsense_current), 0);
    ReportCapSense();
    T3IntUnblock();  // ready for next trigger.
  } else {
    ReportAnalogInStatus();
    if (capsense_bitmask) {
      ADCCapSenseTrigger();
    } else {
      T3IntUnblock();  // ready for next trigger.
    }
  }
}

// we need to generate a priority 1 interrupt in order to send a message
// containing ADC-captured data.
//
// this is the reasoning:
// we need to protect the outgoing-message buffer from concurrent access. this
// is achieved by making sure it is only written to by priority 1 code.
// however, in the case of ADC, we must service the "done" interrupt quickly
// to stop the ADC before our buffer gets overwritten, so this would be a
// priority 6 interrupt. then, in order to write to the output buffer, it would
// trigger the priority 1 interrupt using GFX1, that will read the ADC data and
// write to the buffer.
//
// Towards this end, we toggle the ADC interrupt priority based on whether or
// not the ADC is running; we only clear the interrupt flag after both parts of
// the handler have run.
void __attribute__((__interrupt__, auto_psv)) _ADC1Interrupt() {
    if(_ADON) {
        _ADON = 0;  // Turn the module off to stop an overrun.
        _AD1IP = SLOW_INT_PRIORITY;
    } else {
        ScanDoneBottomHalf();
        _AD1IF = 0;  // clear
        _AD1IP = FAST_INT_PRIORITY;
    }
}
