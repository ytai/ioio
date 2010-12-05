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
  #define log_print_1(x,a) do { sprintf(char_buf, x, a); log_print_0(char_buf); } while(0)
  #define log_print_2(x,a,b) do { sprintf(char_buf, x, a, b); log_print_0(char_buf); } while(0)
  #define log_print_3(x,a,b,c) do { sprintf(char_buf, x, a, b, c); log_print_0(char_buf); } while(0)
  #define log_print_4(x,a,b,c,d) do { sprintf(char_buf, x, a, b, c, d); log_print_0(char_buf); } while(0)
  #define log_print_5(x,a,b,c,d,e) do { sprintf(char_buf, x, a, b, c, d, e); log_print_0(char_buf); } while(0)
  #define log_print_6(x,a,b,c,d,e,f) do { sprintf(char_buf, x, a, b, c, d, e, f); log_print_0(char_buf); } while(0)
#else
  #define log_print_buf(b,s)
  #define log_print_0(x)
  #define log_print_1(x,a)
  #define log_print_2(x,a,b)
  #define log_print_3(x,a,b,c)
  #define log_print_4(x,a,b,c,d)
  #define log_print_5(x,a,b,c,d,e)
  #define log_print_6(x,a,b,c,d,e,f)
#endif

#define LOG_CHANGE_STATE(var, state) \
  do { var = state; log_print_2("%s changed to %s", #var, #state); } while(0)


#endif  // __LOGGING_H__
