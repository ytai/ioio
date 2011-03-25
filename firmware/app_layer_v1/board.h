// Capabilities and features of specific board versions.
//
// Provides:
// NUM_PIN_MODULES - The number of physical pins on the board, including the on-
//                   board LED.
// NUM_PWM_MODULES - The number of available PWM modules.
// NUM_UART_MODULES - The number of available UART modules.

#ifndef __BOARD_H__
#define __BOARD_H__

// TODO: move to a project flag.
#define IOIO_V12

#if defined(IOIO_V10)
  #define NUM_PINS 50
#elif defined(IOIO_V11) || defined(IOIO_V12)
  #define NUM_PINS 49
#else
  #error Unknown board
#endif

#if defined(__PIC24FJ256DA206__) || defined(__PIC24FJ128DA106__)
  #define NUM_PWM_MODULES 9
  #define NUM_UART_MODULES 4
  #define NUM_SPI_MODULES 3
  #define NUM_I2C_MODULES 3
  #define NUM_INCAP_MODULES 9
#else
  #error Unknown MCU
#endif


#endif  // __BOARD_H__
