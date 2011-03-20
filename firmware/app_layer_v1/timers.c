#include "timers.h"

#include "Compiler.h"
#include "logging.h"

void TimersInit() {
  log_printf("TimersInit()");
  // timer 1 is sysclk / 256 = 62.5KHz
  T1CON = 0x0000;  // Timer off
  TMR1 = 0x0000;
  PR1 = 0xFFFF;
  T1CON = 0x8030;

  // timer 3 is sysclk / 64 = 256KHz
  T3CON = 0x0000;  // Timer off
  TMR3 = 0x0000;
  PR3 = 0xFFFF;
  T3CON = 0x8020;
}
