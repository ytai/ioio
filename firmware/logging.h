#ifndef __LOGGING_H__
#define __LOGGING_H__


#ifdef DEBUG_MODE
  #include "uart2.h"
  #include "GenericTypeDefs.h"

  extern char char_buf[];
  
  void print_message(const void* buf, int size);
  #define print0(x) UART2PrintString(x)
  #define print1(x,a) do { sprintf(char_buf, x, a); UART2PrintString(char_buf); } while(0)
  #define print2(x,a, b) do { sprintf(char_buf, x, a, b); UART2PrintString(char_buf); } while(0)
#else
  #define print_message(buf, size)
  #define print0(x)
  #define print1(x,a)
  #define print2(x,a, b)
#endif



#endif  // __LOGGING_H__
