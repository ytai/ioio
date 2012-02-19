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

#include "icsp.h"

#include "byte_queue.h"
#include "pins.h"
#include "logging.h"
#include "features.h"
#include "HardwareProfile.h"
#include "timer.h"
#include "protocol.h"

#define PGC_PIN 37
#define PGD_PIN 38
#define MCLR_PIN 36

#define RX_BUF_SIZE 64
//#define TX_BUF_SIZE 64

#define PGD_OUT() PinSetTris(PGD_PIN, 0)
#define PGD_IN() PinSetTris(PGD_PIN, 1)

// TODO: do not use delay

static int num_rx_since_last_report;
static BYTE_QUEUE rx_queue;
//static BYTE_QUEUE tx_queue;
static BYTE rx_buffer[RX_BUF_SIZE];
//static BYTE tx_buffer[TX_BUF_SIZE];

static void ClockBitsOut(BYTE b, int nbits) {
  while (nbits-- > 0) {
    PinSetLat(PGD_PIN, b & 1);
    PinSetLat(PGC_PIN, 1);
    PinSetLat(PGC_PIN, 0);
    b >>= 1;
  }
}

static BYTE ClockBitsIn(int nbits) {
  int i;
  BYTE b = 0;
  for (i = 0; i < nbits; ++i) {
    PinSetLat(PGC_PIN, 1);
    b |= (PinGetPort(PGD_PIN) << i);
    PinSetLat(PGC_PIN, 0);
  }
  return b;
}

static inline void ClockByteOut(BYTE b) {
  ClockBitsOut(b, 8);
}

static inline BYTE ClockByteIn() {
  return ClockBitsIn(8);
}

void ICSPEnter() {
  log_printf("ICSPEnter()");
  // pulse reset
  PinSetLat(MCLR_PIN, 1);
  Delay10us(50);
  PinSetLat(MCLR_PIN, 0);
  DelayMs(1);
  PGD_OUT();
  // enter code
  ClockByteOut(0xB2);
  ClockByteOut(0xC2);
  ClockByteOut(0x12);
  ClockByteOut(0x8A);
  // mclr high
  PinSetLat(MCLR_PIN, 1);
  DelayMs(50);
  // extra 5 bits for first SIX
  ClockBitsOut(0, 5);
}

void ICSPExit() {
  log_printf("ICSPExit()");
  PinSetLat(MCLR_PIN, 0);
  Delay10us(50);
}

void ICSPSix(DWORD inst) {
  log_printf("ICSPSix(0x%06lx)", inst);
  PGD_OUT();
  ClockBitsOut(0, 4);
  ClockByteOut(inst);
  inst >>= 8;
  ClockByteOut(inst);
  inst >>= 8;
  ClockByteOut(inst);
}

void ICSPRegout() {
  log_printf("ICSPRegout()");
  PGD_OUT();
  ClockBitsOut(1, 4);
  PGD_IN();
  ClockByteIn();  // skip 8 bits
  ByteQueuePushByte(&rx_queue, ClockByteIn());
  ByteQueuePushByte(&rx_queue, ClockByteIn());
}

void ICSPConfigure(int enable) {
  log_printf("ICSPConfigure(%d)", enable);
  if (enable) {
    SetPinDigitalOut(PGC_PIN, 0, 0);
    SetPinDigitalOut(PGD_PIN, 0, 0);
    SetPinDigitalOut(MCLR_PIN, 0, 0);
    // pull PGD up for reads.
    PinSetCnpu(PGD_PIN, 1);
    PinSetCnpd(PGD_PIN, 0);

    ByteQueueInit(&rx_queue, rx_buffer, RX_BUF_SIZE);
    num_rx_since_last_report = RX_BUF_SIZE;
  } else {
    SetPinDigitalIn(PGC_PIN, 0);
    SetPinDigitalIn(PGD_PIN, 0);
    SetPinDigitalIn(MCLR_PIN, 0);
  }
}

static void ICSPReportRxStatus() {
  int report;
  report = num_rx_since_last_report;
  num_rx_since_last_report = 0;
  OUTGOING_MESSAGE msg;
  msg.type = ICSP_REPORT_RX_STATUS;
  msg.args.icsp_report_rx_status.bytes_to_add = report;
  AppProtocolSendMessage(&msg);
}

void ICSPTasks() {
  while (ByteQueueSize(&rx_queue)) {
    OUTGOING_MESSAGE msg;
    msg.type = ICSP_RESULT;
    ByteQueuePullToBuffer(&rx_queue, &msg.args.icsp_result.reg, 2);
    num_rx_since_last_report += 2;
    log_printf("ICSP read word: 0x%04x", msg.args.icsp_result.reg);
    AppProtocolSendMessage(&msg);
  }
  if (num_rx_since_last_report > RX_BUF_SIZE / 2) {
    ICSPReportRxStatus();
  }
}


