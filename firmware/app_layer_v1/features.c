#include "features.h"

#include "Compiler.h"
#include "pins.h"
#include "logging.h"
#include "protocol.h"

// timer 1 works at Fosc / 2 = 16MHz
// we use a 256x presclaer to achieve 62.5KHz
void InitTimer1() {
  PR1 = 0x0000;
  TMR1 = 0x0000;
  PR1 = 0xFFFF;
  T1CON = 0x8030;
}

void SetPinDigitalOut(int pin, int value, int open_drain) {
  log_printf("SetPinDigitalOut(%d, %d, %d)", pin, value, open_drain);
  PinSetAnsel(pin, 0);
  PinSetCnen(pin, 0);
  PinSetCnpu(pin, 0);
  PinSetCnpd(pin, 0);
  PinSetLat(pin, value);
  PinSetOdc(pin, open_drain);
  PinSetTris(pin, 0);
}

void SetDigitalOutLevel(int pin, int value) {
  log_printf("SetDigitalOutLevel(%d, %d)", pin, value);
  PinSetLat(pin, value);
}

void SetPinDigitalIn(int pin, int pull) {
  log_printf("SetPinDigitalIn(%d, %d)", pin, pull);
  PinSetAnsel(pin, 0);
  PinSetCnen(pin, 0);
  switch (pull) {
    case 1:
      PinSetCnpd(pin, 0);
      PinSetCnpu(pin, 1);
      break;

    case 2:
      PinSetCnpu(pin, 0);
      PinSetCnpd(pin, 1);
      break;

    default:
      PinSetCnpu(pin, 0);
      PinSetCnpd(pin, 0);
  }
  PinSetTris(pin, 1);
}

void SetChangeNotify(int pin, int changeNotify) {
  log_printf("SetChangeNotify(%d, %d)", pin, changeNotify);
  PinSetCnen(pin, changeNotify);
}

void ReportDigitalInStatus(int pin) {
  log_printf("ReportDigitalInStatus(%d)", pin);
  OUTGOING_MESSAGE msg;
  msg.type = REPORT_DIGITAL_IN_STATUS;
  msg.args.report_digital_in_status.pin = pin;
  msg.args.report_digital_in_status.level = PinGetPort(pin);
  AppProtocolSendMessage(&msg);
}

void HardReset(DWORD magic) {
  log_printf("HardReset(%lx)", magic);
  if (magic == IOIO_MAGIC) {
    log_printf("Rebooting...");
    Reset();
  } else {
    log_printf("No magic!");
  }
}

void SoftReset() {
  log_printf("SoftReset()");
  int i;
  // reset pin states
  SetPinDigitalOut(0, 1, 1);  // LED pin: output, open-drain, high (off)
  for (i = 1; i < NUM_PINS; ++i) {
    SetPinDigitalIn(i, 0);    // all other pins: input, no-pull
  }
  // clear and enable global CN interrupts
  _CNIF = 0;
  _CNIE = 1;
  // initialize timer 1
  InitTimer1();
  // TODO: reset all peripherals!
}

#define CHECK_PORT_CHANGE(name)                       \
  do {                                                \
    port = PORT##name;                                \
    changed = (port ^ CNBACKUP##name) & CNEN##name;   \
    for (i = 0; i < 16; ++i) {                        \
      if (changed & 0x0001) {                         \
        ReportDigitalInStatus(PinFromPort##name(i));  \
      }                                               \
      changed >>= 1;                                  \
    }                                                 \
    CNBACKUP##name = port;                            \
  } while (0)


void __attribute__((__interrupt__, auto_psv)) _CNInterrupt() {
  unsigned int port;
  unsigned int changed;
  int i;
  log_printf("_CNInterrupt()");

  CHECK_PORT_CHANGE(B);
  CHECK_PORT_CHANGE(C);
  CHECK_PORT_CHANGE(D);
  CHECK_PORT_CHANGE(E);
  CHECK_PORT_CHANGE(F);
  CHECK_PORT_CHANGE(G);

  _CNIF = 0;
}

////////////////////////////////////////////////////////////////////////////////
// PWM
////////////////////////////////////////////////////////////////////////////////

typedef struct {
  unsigned int con1;
  unsigned int con2;
  unsigned int rs;
  unsigned int r;
  unsigned int tmr;
} OC_REGS;

#define OC_REG(num) (((OC_REGS *) 0x186) + num)

void SetPinPwm(int pin, int pwmNum) {
  log_printf("SetPinPwm(%d, %d)", pin, pwmNum);
  PinSetRpor(pin, pwmNum == 0 ? 0 : (pwmNum == 9 ? 35 : 17 + pwmNum));
}

void SetPwmDutyCycle(int pwmNum, int dc_lsb, int dc_msb) {
  volatile OC_REGS* regs;
  log_printf("SetPwmDutyCycle(%d, %d, %d)", pwmNum, dc_lsb, dc_msb);
  regs = OC_REG(pwmNum);
  regs->con2 &= ~0x0600;
  regs->con2 |= dc_lsb << 9;
  regs->r = dc_msb;
}

void SetPwmPeriod(int pwmNum, int period, int scale256) {
  volatile OC_REGS* regs;
  log_printf("SetPwmPeriod(%d, %d, %d)", pwmNum, period, scale256);
  regs = OC_REG(pwmNum);
  regs->con1 = 0x0000;
  if (period) {
    regs->r = 0;
    regs->rs = period;
    regs->con2 = 0x001F;
    regs->con1 = 0x0006 | (scale256 ? 0x1000 : 0x1C00);
  }
}

// BOOKMARK(add_feature): Add feature implementation.
