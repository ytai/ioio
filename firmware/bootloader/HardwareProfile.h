#ifndef _HARDWAREPROFILE_H_
#define _HARDWAREPROFILE_H_

#if defined(__PIC24FJ256DA206__) || defined(__PIC24FJ128DA106__)
  #include <p24Fxxxx.h>
  // Various clock values
  #define GetSystemClock()            32000000UL
  #define GetPeripheralClock()        (GetSystemClock())
  #define GetInstructionClock()       (GetSystemClock() / 2)

  // Define the baud rate constants
  #define BAUDRATE2       38400
  #define BRG_DIV2        4
  #define BRGH2           1

  // LEDS
  #define mInitAllLEDs()  {TRISFbits.TRISF3 = 0; LATFbits.LATF3 = 1;}
  #define mInitAllSwitches()
  #define mLED_0              LATFbits.LATF3
#else
  #error Unsupported target
#endif

#define mLED_0_On()         mLED_0  = 0;
#define mLED_0_Off()        mLED_0  = 1;
#define mLED_0_Toggle()     mLED_0  = !mLED_0;
    

#endif  

