#include "dumpsys.h"

#include <assert.h>

#include "bootloader_defs.h"

#define MAX_PATH 64

typedef enum {
  STATE_IN_PREFIX,
  STATE_SKIP_LINE,
  STATE_IN_PATH
} STATE;

static const char BOOTCNST prefix[] = "    dataDir=";
static int BOOTDATA cursor;
static char BOOTDATA result[MAX_PATH];
static STATE BOOTDATA state;

void DumpsysInit() {
  cursor = 0;
  state = STATE_IN_PREFIX;
}

const char* DumpsysProcess(const char* data, int size) {
  assert(data);

  while (size--) {
    char c = *data++;
    switch (state) {
      case STATE_IN_PREFIX:
        if (c == prefix[cursor++]) {
          if (cursor == sizeof(prefix) - 1) {
            state = STATE_IN_PATH;
            cursor = 0;
          }
        } else {
          state = STATE_SKIP_LINE;
        }
        break;

      case STATE_SKIP_LINE:
        if (c == '\n') {
          state = STATE_IN_PREFIX;
          cursor = 0;
        }
        break;

      case STATE_IN_PATH:
        if (c == '\r') {
          result[cursor] = '\0';
          return result;
        } else {
          if (cursor < MAX_PATH - 1) {
            result[cursor++] = c;
          } else {
            return DUMPSYS_ERROR;
          }
        break;
      }
    }
  }
  return DUMPSYS_BUSY;
}
