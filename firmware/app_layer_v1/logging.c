#include "logging.h"

#ifdef ENABLE_LOGGING

char char_buf[256];

void log_print_buf(const void* buf, int size) {
  const BYTE* byte_buf = (const BYTE*) buf;
  int s = size;
  while (size-- > 0) {
    UART2PutHex(*byte_buf++);
    UART2PutChar(' ');
  }
  UART2PutChar('\r');
  UART2PutChar('\n');

  byte_buf -= s;
  while (s-- > 0) {
    UART2PutChar(*byte_buf++);
  }
  UART2PutChar('\r');
  UART2PutChar('\n');
}

#endif  // DEBUG_MODE
