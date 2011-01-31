#include <uart2.h>
#include <p24fxxxx.h>
#include <pps.h>

#include "Compiler.h"
#include "blapi/adb.h"
#include "blapi/bootloader.h"
#include "features.h"
#include "protocol.h"

typedef enum {
  STATE_INIT,
  STATE_WAIT_CONNECTION,
  STATE_WAIT_CHANNEL_OPEN,
  STATE_CONNECTED
} STATE;

STATE state = STATE_INIT;

void ChannelCallback(ADB_CHANNEL_HANDLE h, const void* data, UINT32 data_len) {
  if (data) {
    AppProtocolHandleIncoming(data, data_len);
  } else {
    // connection closed, re-establish
    state = STATE_INIT;
  }
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
        SoftReset();
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
          AppProtocolInit(h);
          state = STATE_CONNECTED;
        }
        break;

      case STATE_CONNECTED:
        AppProtocolTasks(h);
        break;
    }
  }
  return 0;
}
