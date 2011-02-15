// Capabilities and features of specific board versions.
//
// Provides:
// NUM_PINS - The number of physical pins on the board, including the on-board
//            LED.
// NUM_PWMS - The number of available PWM modules.
// NUM_UARTS - The number of available UART modules.

#ifndef __BOARD_H__
#define __BOARD_H__

// TODO: move to a project flag.
#define IOIO_V10

#if defined(IOIO_V10)
  #define NUM_PINS 50
#elif defined(IOIO_V11) || defined(IOIO_V12)
  #define NUM_PINS 49
#else
  #error Unknown board
#endif

#if defined(__PIC24FJ256DA206__) || defined(__PIC24FJ128DA106__)
  #define NUM_PWMS 9
  #define NUM_UARTS 4
#else
  #error Unknown MCU
#endif


#endif  // __BOARD_H__
