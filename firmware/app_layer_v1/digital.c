#include "digital.h"

#include "Compiler.h"
#include "logging.h"
#include "pins.h"
#include "protocol.h"

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
