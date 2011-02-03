#include "features.h"

#include "Compiler.h"
#include "pins.h"
#include "logging.h"
#include "protocol.h"

#ifdef ENABLE_LOGGING
  #define SAVE_PIN4_FOR_LOG() if (pin == 4) return
#else
  #define SAVE_PIN4_FOR_LOG()
#endif

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

// timer 1 is clocked @16MHz
// we use a 256x presclaer to achieve 62.5KHz
// used for low frequency PWM
static void Timer1Init() {
  T1CON = 0x0000;  // Timer off
  TMR1 = 0x0000;
  PR1 = 0xFFFF;
  T1CON = 0x8030;
}

#define OC_REG(num) (((OC_REGS *) 0x186) + num)

static void PWMInit() {
  int i;
  // disable PWMs
  for (i = 1; i <= NUM_PWMS; ++i) {
    SetPwmPeriod(i, 0, 0);
  }
  Timer1Init();  // constantly running, feeds low-speed PWM
}

void SetPwmDutyCycle(int pwmNum, int dc, int fraction) {
  volatile OC_REGS* regs;
  log_printf("SetPwmDutyCycle(%d, %d, %d)", pwmNum, dc, fraction);
  regs = OC_REG(pwmNum);
  regs->con2 &= ~0x0600;
  regs->con2 |= fraction << 9;
  regs->r = dc;
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

////////////////////////////////////////////////////////////////////////////////
// ADC
////////////////////////////////////////////////////////////////////////////////

static unsigned int analog_scan_bitmask;

// timer 2 is clocked @16MHz
// its period is 0, and it is used to generate a low-priority (1) interrupt as
// soon as it is enabled.
// its usage is for sending a message containing ADC-captured data.
// this is the reasononing:
// we need to protect the outgoing-message buffer from concurrent access. this
// is achieved by making sure it is only written to by priority 1 code.
// however, in the case of ADC, we must service the "done" interrupt quickly
// to stop the ADC before our buffer gets overwritten, so this would be a
// priority 7 interrupt. then, in order to write to the output buffer, it would
// trigger the priority 1 interrupt using timer 2, that will read the ADC data
// and write to the buffer.
static void Timer2Init() {
  _T2IE = 0;
  T2CON = 0x0000;  // Timer off
  TMR2 = 0x0000;
  PR2 = 0x0001;
  T2CON = 0x8000;  // Timer on
  _T2IP = 1;  // interrupt priority 1
}

// timer 3 is clocked @16MHz
// we set its period to 16000 so that a match occurs @1KHz
// used for ADC
static void Timer3Init() {
  T3CON = 0x0000;  // Timer off
  TMR3 = 0x0000;
  PR3 = 0x3E7F;
  T3CON = 0x8030; // TODO: 0x8000
  _T3IE = 1;
}

void __attribute__((__interrupt__, auto_psv)) _T3Interrupt() {
  // TODO: report pin config if changed.
  AD1CSSL = analog_scan_bitmask;
  AD1CON1 = 0x80E6;  // start ADC
  _T3IF = 0;  // clear
}

void __attribute__((__interrupt__, auto_psv)) _T2Interrupt() {
  log_printf("sample is %d", ADC1BUF0);
  _T2IE = 0;  // disable myself
  _T2IF = 0;  // clear
}

void __attribute__((__interrupt__, auto_psv)) _ADC1Interrupt() {
  AD1CON1 = 0x0000;  // ADC off
  _T2IE = 1;  // trigger the priority 1 interrupt
  _AD1IF = 0;  // clear
}

static void ADCInit() {
  log_printf("ADCInit()");
  AD1CON1 = 0x0000;  // ADC off
  AD1CON2 = 0x0400;  // Avdd Avss ref, scan inputs, single buffer, 
  AD1CON3 = 0x0A01;  // system clock, 10 Tad acquisition time, ADC clock @8MHz
  AD1CHS  = 0x0000;  // Sample AN0 against negative reference.

  _SMPI = 0;  // interrupt every sample
  _AD1IP = 7;    // high priority to stop automatic sampling
  _AD1IF = 0;
  _AD1IE = 1;

  Timer2Init();  // when started generates an immediate interrupt to read ADC buffer
  Timer3Init();  // runs when ADC is used to periodically trigger sampling

  analog_scan_bitmask = 0x0001;
}

//static void ADCTrigger() {
//  AD1CON1 = 0x8040;  // Timer3 source
//}
//
//static void ADCStart() {
//  AD1CON1 = 0x8040;  // Timer3 source 
//}
//
//static void ADCStop() {
//  AD1CON1 = 0x0000;
//}

// TODO adc:
// BUFS
// SMPI
// AD1CSSL

////////////////////////////////////////////////////////////////////////////////
// Pins
////////////////////////////////////////////////////////////////////////////////

static void PinsInit() {
  int i;
  // reset pin states
  SetPinDigitalOut(0, 1, 1);  // LED pin: output, open-drain, high (off)
  for (i = 1; i < NUM_PINS; ++i) {
    SetPinDigitalIn(i, 0);    // all other pins: input, no-pull
  }
  // clear and enable global CN interrupts
  _CNIF = 0;
  _CNIE = 1;
  _CNIP = 1;  // CN interrupt priority is 1 so it can write an outgoing message
}

void SetPinDigitalOut(int pin, int value, int open_drain) {
  log_printf("SetPinDigitalOut(%d, %d, %d)", pin, value, open_drain);
  SAVE_PIN4_FOR_LOG();
  PinSetAnsel(pin, 0);
  PinSetRpor(pin, 0);
  PinSetCnen(pin, 0);
  PinSetCnpu(pin, 0);
  PinSetCnpd(pin, 0);
  PinSetLat(pin, value);
  PinSetOdc(pin, open_drain);
  PinSetTris(pin, 0);
}

void SetDigitalOutLevel(int pin, int value) {
  log_printf("SetDigitalOutLevel(%d, %d)", pin, value);
  SAVE_PIN4_FOR_LOG();
  PinSetLat(pin, value);
}

void SetPinDigitalIn(int pin, int pull) {
  log_printf("SetPinDigitalIn(%d, %d)", pin, pull);
  SAVE_PIN4_FOR_LOG();
  PinSetAnsel(pin, 0);
  PinSetRpor(pin, 0);
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

//void SetPinAnalogIn(int pin) {
//  int an_pin_num;
//  log_printf("SetPinAnalogIn(%d)", pin);
//  PinSetRpor(pin, 0);
//  PinSetCnen(pin, 0);
//  PinSetCnpu(pin, 0);
//  PinSetCnpd(pin, 0);
//  PinSetAnsel(pin, 1);
//  PinSetTris(pin, 0);
//  an_pin_num = PinGetAnalogPinNumber(pin);
//  if (an_pin_num != -1) {
//    
//  }
//}

void SetChangeNotify(int pin, int changeNotify) {
  log_printf("SetChangeNotify(%d, %d)", pin, changeNotify);
  SAVE_PIN4_FOR_LOG();
  PinSetCnen(pin, changeNotify);
}

void SetPinPwm(int pin, int pwmNum) {
  log_printf("SetPinPwm(%d, %d)", pin, pwmNum);
  SAVE_PIN4_FOR_LOG();
  PinSetRpor(pin, pwmNum == 0 ? 0 : (pwmNum == 9 ? 35 : 17 + pwmNum));
}

void ReportDigitalInStatus(int pin) {
  log_printf("ReportDigitalInStatus(%d)", pin);
  SAVE_PIN4_FOR_LOG();
  OUTGOING_MESSAGE msg;
  msg.type = REPORT_DIGITAL_IN_STATUS;
  msg.args.report_digital_in_status.pin = pin;
  msg.args.report_digital_in_status.level = PinGetPort(pin);
  AppProtocolSendMessage(&msg);
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
// Reset
////////////////////////////////////////////////////////////////////////////////

void HardReset() {
  log_printf("HardReset()");
  log_printf("Rebooting...");
  Reset();
}

void SoftReset() {
  log_printf("SoftReset()");
  // initialize pins
  PinsInit();
  // initialize PWM
  PWMInit();
  // initialze ADC
  ADCInit();
  // TODO: reset all peripherals!
}

// BOOKMARK(add_feature): Add feature implementation.
