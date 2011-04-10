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
  #define log_printf(...) do { sprintf(char_buf, __VA_ARGS__); log_print_0(char_buf); } while(0)
  void log_init();

  #define SAVE_PIN_FOR_LOG(pin) if (pin == 32) return
  #define SAVE_UART_FOR_LOG(uart) if (uart == 1) return
#else
  #define log_print_buf(b,s)
  #define log_print_0(x)
  #define log_printf(...)
  #define SAVE_PIN_FOR_LOG(pin)
  #define SAVE_UART_FOR_LOG(uart)
  #define log_init()
#endif


#endif  // __LOGGING_H__
