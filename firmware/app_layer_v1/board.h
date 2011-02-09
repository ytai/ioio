#ifndef __BOARD_H__
#define __BOARD_H__

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
