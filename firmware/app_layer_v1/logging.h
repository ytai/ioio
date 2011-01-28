#ifndef __LOGGING_H__
#define __LOGGING_H__


#ifdef ENABLE_LOGGING
  #include <stdio.h>
  #include "uart2.h"
  #include "GenericTypeDefs.h"

  extern char char_buf[];

  #define STRINGIFY(x) #x
  #define TOSTRING(x) STRINGIFY(x)
  
  void log_print_buf(const void* buf, int size);
  #define log_print_0(x) do { UART2PrintString("["__FILE__": "TOSTRING(__LINE__)"] "); UART2PrintString(x); UART2PrintString("\r\n"); } while (0)
  #define log_printf(x,...) do { sprintf(char_buf, x, __VA_ARGS__); log_print_0(char_buf); } while(0)
#else
  #define log_print_buf(b,s)
  #define log_print_0(x)
  #define log_printf(x,...)
#endif


#endif  // __LOGGING_H__
