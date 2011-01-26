#include <uart2.h>
#include <p24fxxxx.h>
#include <pps.h>

#include "blapi/adb.h"
#include "blapi/bootloader.h"

#include "Compiler.h"

typedef enum {
  STATE_INIT,
  STATE_WAIT_CONNECTION,
  STATE_WAIT_CHANNEL_OPEN,
  STATE_CONNECTED
} STATE;

STATE state = STATE_INIT;

void ChannelCallback(ADB_CHANNEL_HANDLE h, const void* data, UINT32 data_len) {
  if (data) {
  } else {
    if (data_len == 0) {
    } else {
    }
    // connection closed, re-establish
    state = STATE_INIT;
  }
}

void ResetAllPeripherals() {
  // TODO: for real
  LATFbits.LATF3 = 1;  // LED off
  TRISFbits.TRISF3 = 0;  // LED pin is output
  ODCFbits.ODF3 = 1;  // LED pin is open drain
}

void AppProtocolInit() {
}

void AppProtocolTasks() {
  static unsigned count = 0;
  LATFbits.LATF3 = count++ >> 13;
}

int main() {
  ADB_CHANNEL_HANDLE h;
  iPPSInput(IN_FN_PPS_U2RX,IN_PIN_PPS_RP2);       //Assign U2RX to pin RP2 (42)
  iPPSOutput(OUT_PIN_PPS_RP4,OUT_FN_PPS_U2TX);    //Assign U2TX to pin RP4 (43)
  UART2Init();

  UART2PrintString("***** Hello from app-layer! *******\r\n");

  while (1) {
    BOOL adb_connected = BootloaderTasks();
    if (!adb_connected && state > STATE_WAIT_CONNECTION) {
      // just got disconnected
      state = STATE_INIT;
    }
    switch (state) {
      case STATE_INIT:
        ResetAllPeripherals();
        state = STATE_WAIT_CONNECTION;
        break;

      case STATE_WAIT_CONNECTION:
        if (adb_connected) {
          h = ADBOpen("tcp:4545", &ChannelCallback);
          state = STATE_WAIT_CHANNEL_OPEN;
        }
        break;

      case STATE_WAIT_CHANNEL_OPEN:
        if (ADBChannelReady(h)) {
          AppProtocolInit();
          state = STATE_CONNECTED;
        }
        break;

      case STATE_CONNECTED:
        AppProtocolTasks();
        break;
    }
  }
  return 0;
}
