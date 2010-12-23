#include "Compiler.h"

int main() {
  TRISFbits.TRISF3 = 0;
  while (1) {
    long i = 1000000L;
    LATFbits.LATF3 = 0;
    while (i--);
    i = 1000000L;
    LATFbits.LATF3 = 1;
    while (i--);
  }
}
