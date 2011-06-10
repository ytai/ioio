#include "icsp.h"

#include "pins.h"
#include "logging.h"
#include "features.h"
#include "HardwareProfile.h"
#include "timer.h"

#define PGC_PIN 37
#define PGD_PIN 38
#define MCLR_PIN 36

#define PGD_OUT() PinSetTris(PGD_PIN, 0)
#define PGD_IN() PinSetTris(PGD_PIN, 1)

#define Sleep(x) do { int i = x; while(i-- > 0) { asm("nop"); }  } while (0)

static void ClockBitsOut(BYTE b, int nbits) {
  while (nbits-- > 0) {
    PinSetLat(PGD_PIN, b & 1);
    Sleep(1);
    PinSetLat(PGC_PIN, 1);
    Sleep(1);
    PinSetLat(PGC_PIN, 0);
    b >>= 1;
  }
}

static BYTE ClockBitsIn(int nbits) {
  int i;
  BYTE b = 0;
  for (i = 0; i < nbits; ++i) {
    Sleep(1);
    PinSetLat(PGC_PIN, 1);
    b |= (PinGetPort(PGD_PIN) << i);
    Sleep(1);
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

void ICSPSix(DWORD inst) {
  log_printf("ICSPSix(0x%lx)", inst);
  PGD_OUT();
  ClockBitsOut(0, 4);
  ClockByteOut(inst);
  inst >>= 8;
  ClockByteOut(inst);
  inst >>= 8;
  ClockByteOut(inst);
}

WORD ICSPRegout() {
  WORD ret;
  PGD_OUT();
  ClockBitsOut(1, 4);
  PGD_IN();
  ClockByteIn();
  ret = ClockByteIn() | (((WORD) ClockByteIn()) << 8);
  log_printf("ICSPRegout got 0x%x", ret);
  return ret;
}

static void SlaveLED(int on) {
  if (on) {
    ICSPSix(0x2FFF70ul);  // mov #FFF7, W0
  } else {
    ICSPSix(0x2FFFF0ul);  // mov #FFF7, W0
  }
  ICSPSix(0x881740ul);  // mov W0, TRISF
}

void ICSPStart() {
  log_printf("ICSPStart()");
  SetPinDigitalOut(PGC_PIN, 0, 0);
  SetPinDigitalOut(PGD_PIN, 0, 0);
  SetPinDigitalOut(MCLR_PIN, 0, 0);

  // temp
  ICSPEnter();

  ICSPSix(0x040200ul);  // goto 0x200
  ICSPSix(0x040200ul);  // goto 0x200
  ICSPSix(0x000000ul);  // nop
  ICSPSix(0x000000ul);  // nop
  ICSPSix(0x000000ul);  // nop
  ICSPSix(0x040200ul);  // goto 0x200
  ICSPSix(0x000000ul);  // nop

/*
  while (1) {
    DelayMs(250);
    SlaveLED(1);
    DelayMs(250);
    SlaveLED(0);
    ICSPSix(0x000000ul);  // nop
  }
*/

  ICSPSix(0x212340ul);  // mov.w     #0x1234, w0
  ICSPSix(0x000000ul);  // nop
  ICSPSix(0x883c20ul);  // mov.w     w0, 0x784
  ICSPSix(0x000000ul);  // nop
  ICSPRegout();
}
