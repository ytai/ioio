#define GetSystemClock()            32000000UL
#define GetPeripheralClock()        (GetSystemClock())
#define GetInstructionClock()       (GetSystemClock() / 2)

#include "Compiler.h"
#include "timer.h"

int main() {
  LATFbits.LATF3 = 0;
  while (1) {
    TRISFbits.TRISF3 = 0;
    DelayMs(500);
    TRISFbits.TRISF3 = 1;
    DelayMs(500);
  }
  return 0;
}
