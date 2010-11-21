#ifndef __LOGGING_H__
#define __LOGGING_H__


#ifdef DEBUG_MODE
  #include "uart2.h"
  #include "GenericTypeDefs.h"

  extern char char_buf[];
  
  void print_message(const void* buf, int size);
  #define print0(x) do { UART2PrintString(x); UART2PrintString("\r\n"); } while (0)
  #define print1(x,a) do { sprintf(char_buf, x, a); print0(char_buf); } while(0)
  #define print2(x,a, b) do { sprintf(char_buf, x, a, b); print0(char_buf); } while(0)
#else
  #define print_message(buf, size)
  #define print0(x)
  #define print1(x,a)
  #define print2(x,a, b)
#endif

#define ADB_CHANGE_STATE(var, state) \
  do { var = state; print2("ADBP: %s changed to %s", #var, #state); } while(0)


#endif  // __LOGGING_H__
