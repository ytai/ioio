#ifndef _HARDWAREPROFILE_H_
#define _HARDWAREPROFILE_H_

#if defined (__C30__)
  // Various clock values
  #define GetSystemClock()            32000000UL
  #define GetPeripheralClock()        (GetSystemClock())
  #define GetInstructionClock()       (GetSystemClock() / 2)
#elif defined( __PIC32MX__)
  #define GetSystemClock()            80000000UL
  #define GetPeripheralClock()        80000000UL  // Will be divided down
  #define GetInstructionClock()       GetSystemClock()
#endif

// Define the baud rate constants
#define BAUDRATE2       38400
#define BRG_DIV2        4
#define BRGH2           1

#if defined(__PIC24F__) || defined(__PIC24H__)
  #include <p24fxxxx.h>
  #include <uart2.h>
  #include "pps.h"
#else
  #include <p32xxxx.h>
  #include <plib.h>
  #include <uart2.h>
#endif


/** TRIS ***********************************************************/
#define INPUT_PIN           1
#define OUTPUT_PIN          0


#if defined (__C30__)
  #if defined(__PIC24FJ256DA206__) || defined(__PIC24FJ128DA106__)
    #define mInitAllLEDs()  {TRISFbits.TRISF3 = 0; LATFbits.LATF3 = 1;}
    #define mInitAllSwitches()
    
    #define mLED_0              LATFbits.LATF3
  #else
    #error unknown platform
  #endif
  
  #define mLED_0_On()         mLED_0  = 0;
  #define mLED_0_Off()        mLED_0  = 1;
  #define mLED_0_Toggle()     mLED_0  = !mLED_0;
  #define mLED_1_On()
  #define mLED_1_Off()
  #define mLED_1_Toggle()
    
#elif defined(__PIC32MX__)
  #define mInitAllLEDs()  {TRISE &= ~0x0F; LATE &= 0x0F;}
  
  #define mLED_0              LATEbits.LATE0
  #define mLED_1              LATEbits.LATE1
  
  #define mLED_0_On()         mLED_0  = 1;
  #define mLED_1_On()         mLED_1  = 1;
  
  #define mLED_0_Off()        mLED_0  = 0;
  #define mLED_1_Off()        mLED_1  = 0;
  
  #define mLED_0_Toggle()     mLED_0  = !mLED_0;
  #define mLED_1_Toggle()     mLED_1  = !mLED_1;
#endif

#endif  

