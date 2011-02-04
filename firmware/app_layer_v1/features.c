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
static int analog_scan_num_channels;

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
static inline void Timer2Init() {
  _T2IE = 0;       // disable interrupt
  _T2IF = 0;       // clear interrupts
  T2CON = 0x0000;  // timer off
  TMR2  = 0x0000;  // reset counter
  PR2   = 0x0001;  // period of 1 (shortest possible)
  _T2IP = 1;       // interrupt priority 1
  T2CON = 0x8000;  // timer on, 16-bit, instruction clock with no prescaling
}

static inline void Timer2Trigger() {
  _T2IE = 1;       // enable interrupt
}

// timer 3 is clocked @16MHz
// we set its period to 16000 so that a match occurs @1KHz
// used for ADC
static inline void Timer3Init() {
  _T3IE = 0;       // disable interrupt
  _T3IF = 0;       // clear interrupt
  T3CON = 0x0000;  // timer off
  PR3   = 0x3E7F;  // period is 16000 clocks
  _T3IP = 1;       // interrupt priority 1 (this interrupt may write to outgoing channel)
}

static inline void Timer3Start() {
  TMR3  = 0x0000;  // reset counter
  _T3IE = 1;       // enable interrupt
  T3CON = 0x8030;  // timer on, 16-bit, instruction clock with no prescaling  TODO: 0x8000
}

static inline void Timer3Stop() {
  _T3IE = 0;       // disable interrupt
  _T3IF = 0;       // clear interrupt
  T3CON = 0x0000;  // timer off
}

static inline void ADCInit() {
  _AD1IE = 0;        // disable interrupt
  _AD1IF = 0;        // clear interrupt
  AD1CON1 = 0x0000;  // ADC off
  AD1CON2 = 0x0400;  // Avdd Avss ref, scan inputs, single buffer, interrupt on every sample 
  AD1CON3 = 0x0A01;  // system clock, 10 Tad acquisition time, ADC clock @8MHz
  AD1CHS  = 0x0000;  // Sample AN0 against negative reference.

  _SMPI = 0;         // interrupt every sample
  _AD1IF = 0;
  _AD1IP = 7;        // high priority to stop automatic sampling
  _AD1IE = 1;

  Timer2Init();  // when started generates an immediate interrupt to read ADC buffer
  Timer3Init();  // runs when ADC is used to periodically trigger sampling

  analog_scan_bitmask = 0x0001;
  analog_scan_num_channels = 0x0000;
}

static inline void ReportAnalogInStatus() {
  log_printf("value");
}

static inline void ReportAnalogInFormat() {
  log_printf("change in format");
}

static inline void ADCTrigger() {
  if (AD1CSSL != analog_scan_bitmask) {
    ReportAnalogInFormat();
    AD1CSSL = analog_scan_bitmask;
  }
  _AD1IE  = 1;       // enable interrupt
  AD1CON1 = 0x80E6;  // start ADC
}


static inline void ADCStart() {
  Timer3Start();
}

static inline void ADCStop() {
  Timer3Stop();
}

static inline void ADCSetScan(int pin) {
  int mask = 1 << PinToAnalogChannel(pin);
  // ignore if channel is already being scanned
  if (!(mask & analog_scan_bitmask)) {
    if (analog_scan_num_channels) {
      // already running, just add the new channel
      _T3IE = 0;
      ++analog_scan_num_channels;
      analog_scan_bitmask |= mask;
      _T3IE = 1;
    } else {
      // first channel, start running
      analog_scan_num_channels = 1;
      analog_scan_bitmask = mask;
      ADCStart();
    }
  }
}

static inline void ADCClrScan(int pin) {
  int mask = 1 << PinToAnalogChannel(pin);
  // ignore if channel is already not scanned
  if (mask & analog_scan_bitmask) {
    if (analog_scan_num_channels == 1) {
      // last channel, stop running
      ADCStop();
      analog_scan_num_channels = 0;
      analog_scan_bitmask = 0x0000;
      ReportAnalogInFormat();
    } else {
      // already running, just remove the new channel
      _T3IE = 0;
      --analog_scan_num_channels;
      analog_scan_bitmask &= ~mask;
      _T3IE = 1;
    }
  }
}

void __attribute__((__interrupt__, auto_psv)) _T3Interrupt() {
  // TODO: check that the previous sample is done?
  ADCTrigger();
  _T3IF = 0;  // clear
}

void __attribute__((__interrupt__, auto_psv)) _T2Interrupt() {
  _T2IE = 0;  // disable myself
  ReportAnalogInStatus();
  _T2IF = 0;  // clear
}

void __attribute__((__interrupt__, auto_psv)) _ADC1Interrupt() {
  AD1CON1 = 0x0000;  // ADC off
  Timer2Trigger();
  _AD1IF = 0;  // clear
}

////////////////////////////////////////////////////////////////////////////////
// Digital I/O
////////////////////////////////////////////////////////////////////////////////

void SetDigitalOutLevel(int pin, int value) {
  log_printf("SetDigitalOutLevel(%d, %d)", pin, value);
  SAVE_PIN4_FOR_LOG();
  PinSetLat(pin, value);
}

void SetChangeNotify(int pin, int changeNotify) {
  log_printf("SetChangeNotify(%d, %d)", pin, changeNotify);
  SAVE_PIN4_FOR_LOG();
  PinSetCnen(pin, changeNotify);
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
// Pin modes
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
  ADCClrScan(pin);
  PinSetAnsel(pin, 0);
  PinSetRpor(pin, 0);
  PinSetCnen(pin, 0);
  PinSetCnpu(pin, 0);
  PinSetCnpd(pin, 0);
  PinSetLat(pin, value);
  PinSetOdc(pin, open_drain);
  PinSetTris(pin, 0);
}

void SetPinDigitalIn(int pin, int pull) {
  log_printf("SetPinDigitalIn(%d, %d)", pin, pull);
  SAVE_PIN4_FOR_LOG();
  ADCClrScan(pin);
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

void SetPinPwm(int pin, int pwmNum) {
  log_printf("SetPinPwm(%d, %d)", pin, pwmNum);
  SAVE_PIN4_FOR_LOG();
  PinSetRpor(pin, pwmNum == 0 ? 0 : (pwmNum == 9 ? 35 : 17 + pwmNum));
}

void SetPinAnalogIn(int pin) {
  PinSetRpor(pin, 0);
  PinSetCnen(pin, 0);
  PinSetCnpu(pin, 0);
  PinSetCnpd(pin, 0);
  PinSetAnsel(pin, 1);
  PinSetTris(pin, 0);
  ADCSetScan(pin);
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
  // ADCInit();
  // TODO: reset all peripherals!
}

// BOOKMARK(add_feature): Add feature implementation.
