#include "connection.h"

#include <stdlib.h>
#include <assert.h>

#include "blapi/bootloader.h"
#include "blapi/adk.h"
#include "blapi/adb.h"
#include "logging.h"

typedef enum {
  DISCONNECTED,
  ADK_CONNECTED,
  ADB_OPENING_CHANNEL,
  ADB_CONNECTED
} STATE;

static CONNECTION_CALLBACK callback = NULL;
static STATE state;
static ADB_CHANNEL_HANDLE adb_channel;
static BYTE __attribute__((far)) incoming_buffer[1024];

static void ADBCallback (ADB_CHANNEL_HANDLE h, const void* data, UINT32 data_len) {
  if (!data) {
    // channel closed, re-open
    adb_channel = ADBOpen("tcp:4545", &ADBCallback);
    state = ADB_OPENING_CHANNEL;
    return;
  }
  if (callback) {
    callback(data, data_len);
  }
}

void ConnectionInit() {
  state = DISCONNECTED;
}

int ConnectionTasks() {
  BYTE result;
  DWORD bytes_read;
  if (BootloaderTasks()) {
    switch (state) {
      case DISCONNECTED:
        if (ADKAttached()) {
          log_printf("Connection established over ADK.");
          state = ADK_CONNECTED;
          result = ADKRead(incoming_buffer, sizeof incoming_buffer);
          if (result != 0) {
            log_printf("ADKRead failed with code 0x%x. Resetting USB.", result);
            BootloaderResetUSB();
          }
          break;
        }
        if (ADBAttached()) {
          log_printf("Opening ADB channel.");
          adb_channel = ADBOpen("tcp:4545", &ADBCallback);
          state = ADB_OPENING_CHANNEL;
          break;
        }
        log_printf("Couldn't find any comaptible connection interface.");
        return -1;

      case ADK_CONNECTED:
        if (ADKReadDone(&result, &bytes_read)) {
          if (result != 0) {
            log_printf("ADKRead failed with code 0x%x. Resetting USB.", result);
            BootloaderResetUSB();
          }
          if (callback) {
            callback(incoming_buffer, bytes_read);
          }
          result = ADKRead(incoming_buffer, sizeof incoming_buffer);
          if (result != 0) {
            log_printf("ADKRead failed with code 0x%x. Resetting USB.", result);
            BootloaderResetUSB();
          }
        }
        break;

      case ADB_OPENING_CHANNEL:
        if (ADBChannelReady(adb_channel)) {
          log_printf("Connection established over ADB.");
          state = ADB_CONNECTED;
        }
        break;

      case ADB_CONNECTED:
        break;
    }
  } else {
    state = DISCONNECTED;
  }
  return state == ADK_CONNECTED || state == ADB_CONNECTED;
}

void ConnectionSetReadCallback(CONNECTION_CALLBACK cb) {
  callback = cb;
}

void ConnectionWrite(const void *data, int size) {
  assert(data);
  if (state == ADK_CONNECTED) {
    int result = ADKWrite(data, size);
    if (result != 0) {
      log_printf("ADKRead failed with code 0x%x. Resetting USB.", result);
      BootloaderResetUSB();
    }
    return;
  }

  if (state == ADB_CONNECTED) {
    ADBWrite(adb_channel, data, size);
    return;
  }
}

int ConnectionCanWrite() {
  if (state == ADK_CONNECTED) {
    BYTE result;
    if (!ADKWriteDone(&result)) {
      return 0;
    }
    if (result != 0) {
      log_printf("ADKWrite failed with code 0x%x. Resetting USB", result);
      BootloaderResetUSB();
      return 0;
    }
    return 1;
  }

  if (state == ADB_CONNECTED) {
    return ADBChannelReady(adb_channel);
  }
  
  return 0;
}

void ConnectionReset() {
  if (state == ADK_CONNECTED) {
    BootloaderResetUSB();
    state = DISCONNECTED;
    return;
  }

  if (state == ADB_CONNECTED) {
    ADBClose(adb_channel);
    adb_channel = ADBOpen("tcp:4545", &ADBCallback);
    state = ADB_OPENING_CHANNEL;
  }
}
