#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Compiler.h"

_CONFIG1(FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
_CONFIG2(POSCMOD_NONE & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL96MHZ_ON & PLLDIV_NODIV & IESO_OFF)
_CONFIG3(WPDIS_WPDIS & SOSCSEL_EC)

int main() {
  uint32_t i;
  _TRISF3 = 0; // F3 <- output
  _RP16R = 19; // RP16 (LED) <- OC2
  _RP7R = 19;  // RP7 (pin 38) <- OC2

  uint32_t period = 32000000ul - 1;
  uint32_t pulse = 16000000ul - 1;
  OC1R = pulse & 0xFFFF;
  OC2R = pulse >> 16;
  OC1RS= period & 0xFFFF;
  OC2RS = period >> 16;
  
  OC1CON1bits.OCTSEL = 0x07;
  OC2CON1bits.OCTSEL = 0x07;
  OC1CON2bits.SYNCSEL = 0x1F;
  OC2CON2bits.SYNCSEL = 0x1F;
  OC1CON2bits.OCTRIS = 1;
  OC2CON2bits.OC32 = 1;
  OC1CON2bits.OC32 = 1;
  OC2CON1bits.OCM = 6;
  OC1CON1bits.OCM = 6;

  while (1);
  return EXIT_SUCCESS;
}

