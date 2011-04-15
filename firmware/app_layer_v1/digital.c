#include "digital.h"

#include "Compiler.h"
#include "logging.h"
#include "pins.h"
#include "protocol.h"
#include "sync.h"

void SetDigitalOutLevel(int pin, int value) {
  log_printf("SetDigitalOutLevel(%d, %d)", pin, value);
  SAVE_PIN_FOR_LOG(pin);
  BYTE prev = SyncInterruptLevel(4);
  PinSetLat(pin, value);
  SyncInterruptLevel(prev);
}

void SetChangeNotify(int pin, int changeNotify) {
  log_printf("SetChangeNotify(%d, %d)", pin, changeNotify);
  SAVE_PIN_FOR_LOG(pin);
  PinSetCnforce(pin);
  PinSetCnen(pin, changeNotify);
  if (changeNotify) {
    _CNIF = 1;  // force a status message on the new pin
  }
}

static void SendDigitalInStatusMessage(int pin, int value) {
  log_printf("SendDigitalInStatusMessage(%d, %d)", pin, value);
  SAVE_PIN_FOR_LOG(pin);
  OUTGOING_MESSAGE msg;
  msg.type = REPORT_DIGITAL_IN_STATUS;
  msg.args.report_digital_in_status.pin = pin;
  msg.args.report_digital_in_status.level = PinGetPort(pin);
  AppProtocolSendMessage(&msg);
}

#define CHECK_PORT_CHANGE(name)                                        \
  do {                                                                 \
    unsigned int i = 0;                                                \
    unsigned int port = PORT##name;                                    \
    unsigned int changed = (CNFORCE##name | (port ^ CNBACKUP##name))   \
                           & CNEN##name;                               \
    CNBACKUP##name = port;                                             \
    CNFORCE##name = 0x0000;                                            \
    while (changed) {                                                  \
      if (changed & 1) {                                               \
        SendDigitalInStatusMessage(PinFromPort##name(i), (port & 1));  \
      }                                                                \
      ++i;                                                             \
      port >>= 1;                                                      \
      changed >>= 1;                                                   \
    }                                                                  \
  } while (0)


void __attribute__((__interrupt__, auto_psv)) _CNInterrupt() {
  _CNIF = 0;
  log_printf("_CNInterrupt()");

  CHECK_PORT_CHANGE(B);
  CHECK_PORT_CHANGE(C);
  CHECK_PORT_CHANGE(D);
  CHECK_PORT_CHANGE(E);
  CHECK_PORT_CHANGE(F);
  CHECK_PORT_CHANGE(G);
}
