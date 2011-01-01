#include "../flash.h"

#include "Compiler.h"

int main() {
  FlashErasePage(0x9000);
  FlashWriteDWORD(0x9000, 0x123456); 

  // blink LED
  TRISFbits.TRISF3 = 0;
  while (1) {
    long i = 1000L;
    LATFbits.LATF3 = 0;
    while (i--);
    i = 2000000L;
    LATFbits.LATF3 = 1;
    while (i--);
  }
}
