#include "Compiler.h"
#include "blapi/adb.h"
#include "blapi/bootloader.h"
#include "features.h"
#include "protocol.h"
#include "logging.h"

typedef enum {
  STATE_INIT,
  STATE_WAIT_CONNECTION,
  STATE_WAIT_CHANNEL_OPEN,
  STATE_CONNECTED,
  STATE_ERROR
} STATE;

STATE state = STATE_INIT;

void ChannelCallback(ADB_CHANNEL_HANDLE h, const void* data, UINT32 data_len) {
  if (data) {
    if (!AppProtocolHandleIncoming(data, data_len)) {
      // got corrupt input. need to close the connection and soft reset.
      state = STATE_ERROR;
    }
  } else {
    // connection closed, soft reset and re-establish
    state = STATE_INIT;
  }
}

int main() {
  ADB_CHANNEL_HANDLE h;

  log_init();
  log_printf("***** Hello from app-layer! *******\r\n");

  while (1) {
    BOOL adb_connected = BootloaderTasks();
    if (!adb_connected && state > STATE_WAIT_CONNECTION) {
      // just got disconnected
      state = STATE_INIT;
    }
    switch (state) {
      case STATE_INIT:
        SoftReset();
        h = ADB_INVALID_CHANNEL_HANDLE;
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

      case STATE_ERROR:
        ADBClose(h);
        state = STATE_INIT;
        break;
    }
  }
  return 0;
}
