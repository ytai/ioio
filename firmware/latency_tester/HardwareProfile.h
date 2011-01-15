#ifndef __HARDWAREPROFILE_H__
#define __HARDWAREPROFILE_H__

// Various clock values
#define GetSystemClock()            32000000UL
#define GetPeripheralClock()        (GetSystemClock())
#define GetInstructionClock()       (GetSystemClock() / 2)

// Define the baud rate constants
#define BAUDRATE2       38400
#define BRG_DIV2        4
#define BRGH2           1


#endif  // __HARDWAREPROFILE_H__
