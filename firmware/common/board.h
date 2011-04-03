// Capabilities and features of specific board versions.
//
// Provides:
// NUM_PINS         - The number of physical pins on the board, including the
//                    on-board LED.
// NUM_PWM_MODULES  - The number of available PWM modules.
// NUM_UART_MODULES - The number of available UART modules.
// NUM_SPI_MODULES  - The number of available SPI modules.
// NUM_I2C_MODULES  - The number of available I2C modules.

#ifndef __BOARD_H__
#define __BOARD_H__

#define _STRINGIFY(x) #x

// TODO: move to a project flag.
#define IOIO_VER 12

// number of pins of each board
#if IOIO_VER == 10
  #define NUM_PINS 50
#elif IOIO_VER >= 11 && IOIO_VER <= 14
  #define NUM_PINS 49
#else
  #error Unknown board
#endif

// assert MCU
#if IOIO_VER >= 10 && IOIO_VER <= 13
  #ifndef __PIC24FJ128DA106__
    #error Board and MCU mismatch - expecting PIC24FJ128DA106
  #endif
#else
  #error Unknown board
#endif

// board variants:
// the board variant is what maintains compatibility between the application
// firmware image and the board. when an application firmware has the same
// designator as the board, it means that this firmware is intended to run on
// this board version.
// boards that are completely electrically equivalent and have the same pin
// numbering scheme will have identical firmware variant designators.
#if IOIO_VER == 10
  #define BOARD_VARIANT 0
#elif IOIO_VER >= 10 && IOIO_VER <= 14
  #define BOARD_VARIANT 1
#endif

#define BOARD_VARIANT_STRING _STRINGIFY(BOARD_VARIANT)

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
