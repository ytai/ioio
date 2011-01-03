//
// Sample usage and test of the ADB layer.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "timer.h"
#include "GenericTypeDefs.h"
#include "bootloader_private.h"
#include "blapi/adb.h"
#include "blapi/adb_file.h"
#include "HardwareProfile.h"
#include "logging.h"
#include "ioio_file.h"

// *****************************************************************************
// *****************************************************************************
// Configuration Bits
// *****************************************************************************
// *****************************************************************************

#ifdef __C30__
  #if defined(__PIC24FJ256DA206__) || defined(__PIC24FJ128DA106__)
      _CONFIG1(FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
      _CONFIG2(POSCMOD_NONE & IOL1WAY_ON & OSCIOFNC_OFF & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL96MHZ_ON & PLLDIV_NODIV & IESO_OFF)
      _CONFIG3(WPDIS_WPEN | WPFP_WPFP31 | WPCFG_WPCFGEN | WPEND_WPSTARTMEM)
  #else
      #error Procesor undefined
  #endif

#elif defined(__PIC32MX__)
  #pragma config UPLLEN   = ON            // USB PLL Enabled
  #pragma config FPLLMUL  = MUL_15        // PLL Multiplier
  #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
  #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
  #pragma config FPLLODIV = DIV_1         // PLL Output Divider
  #pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
  #pragma config FWDTEN   = OFF           // Watchdog Timer
  #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
  #pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
  #pragma config OSCIOFNC = OFF           // CLKO Enable
  #pragma config POSCMOD  = HS            // Primary Oscillator
  #pragma config IESO     = OFF           // Internal/External Switch-over
  #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable (KLO was off)
  #pragma config FNOSC    = PRIPLL        // Oscillator Selection
  #pragma config CP       = OFF           // Code Protect
  #pragma config BWP      = OFF           // Boot Flash Write Protect
  #pragma config PWP      = OFF           // Program Flash Write Protect
  #pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
  #pragma config DEBUG    = ON            // Background Debugger Enable
#else
  #error Cannot define configuration bits.
#endif


BOOL InitializeSystem(void) {
#if defined(__PIC24FJ256DA206__) || defined(__PIC24FJ128DA106__)
//    iPPSInput(IN_FN_PPS_U2RX,IN_PIN_PPS_RP2);       //Assign U2RX to pin RP2 (42)
//    iPPSOutput(OUT_PIN_PPS_RP4,OUT_FN_PPS_U2TX);    //Assign U2TX to pin RP4 (43)
//    UART2Init();
#elif defined(__PIC32MX__)
  {
    int  value = SYSTEMConfigWaitStatesAndPB(GetSystemClock());
    // Enable the cache for the best performance
    CheKseg0CacheOn();
    INTEnableSystemMultiVectoredInt();
    value = OSCCON;
    while (!(value & 0x00000020)) {
        value = OSCCON;    // Wait for PLL lock to stabilize
    }
  }
#endif

    mInitAllLEDs();

    return TRUE;
}  // InitializeSystem


typedef enum {
  MAIN_STATE_WAIT_CONNECT,
  MAIN_STATE_RECV,
  MAIN_STATE_DONE,
  MAIN_STATE_ERROR
} MAIN_STATE;

static MAIN_STATE state = MAIN_STATE_WAIT_CONNECT;


void FileRecv(ADB_FILE_HANDLE h, const void* data, UINT32 data_len) {
  if (data) {
    if (!IOIOFileHandleBuffer(data, data_len)) {
      state = MAIN_STATE_ERROR;
    }
  } else {
    if (data_len == 0 && IOIOFileDone()) {
//      UART2PrintString("\r\n\r\nSuccessfully wrote application firmware image!\r\n\r\n");
      state = MAIN_STATE_DONE;
    } else {
      state = MAIN_STATE_ERROR;
    }
  }
}

int main(void) {
  ADB_FILE_HANDLE h;
  // Initialize the processor and peripherals.
  if (!InitializeSystem()) {
//    UART2PrintString("\r\n\r\nCould not initialize USB Custom Demo App - system.  Halting.\r\n\r\n");
    while (1);
  }
  BootloaderInit();

  while (1) {
    BOOL connected = BootloaderTasks();
    mLED_0 = !connected;
    if (!connected) {
      state = MAIN_STATE_WAIT_CONNECT;
    }
    
    switch(state) {
     case MAIN_STATE_WAIT_CONNECT:
      if (connected) {
        log_print_0("ADB connected!");
        IOIOFileInit();
        h = ADBFileRead("/data/data/ioio.manager/files/image.ioio", &FileRecv);
        state = MAIN_STATE_RECV;
      }
      break;

     case MAIN_STATE_RECV:
      break;

     case MAIN_STATE_DONE:
      __asm__("goto __APP_RESET");
      break;

     case MAIN_STATE_ERROR:
      break;
    }
    //DelayMs(100);
  }

  return 0;
}  // main
