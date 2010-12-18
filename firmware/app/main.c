#include "Compiler.h"

int main() {
  TRISFbits.TRISF3 = 0;
  while (1) {
    long i = 0;
    LATFbits.LATF3 = 0;
    while (++i);
    LATFbits.LATF3 = 1;
    while (++i);
  }
}
