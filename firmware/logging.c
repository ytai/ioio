#include "logging.h"

#ifdef DEBUG_MODE

char char_buf[256];

void print_message(const void* buf, int size) {
  const BYTE* byte_buf = (const BYTE*) buf;
  while (size-- > 0) {
    UART2PutHex(*byte_buf++);
    UART2PutChar(' ');
  }
  UART2PutChar('\r');
  UART2PutChar('\n');
}

#endif  // DEBUG_MODE
