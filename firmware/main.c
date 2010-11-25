//
// Sample usage and test of the ADB layer.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "timer.h"
#include "GenericTypeDefs.h"
#include "adb.h"
#include "HardwareProfile.h"
#include "logging.h"
#include "ports.h"

// *****************************************************************************
// *****************************************************************************
// Configuration Bits
// *****************************************************************************
// *****************************************************************************

#ifdef __C30__
  #if defined(__PIC24FJ256DA206__)
      _CONFIG1(FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
      _CONFIG2(POSCMOD_NONE & IOL1WAY_ON & OSCIOFNC_OFF & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL96MHZ_ON & PLLDIV_NODIV & IESO_OFF)
      _CONFIG3(0xFFFF)
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
#if defined(__PIC24FJ256DA206__)
  #ifdef DEBUG_MODE
    iPPSInput(IN_FN_PPS_U2RX,IN_PIN_PPS_RP2);       //Assign U2RX to pin RP2 (42)
    iPPSOutput(OUT_PIN_PPS_RP4,OUT_FN_PPS_U2TX);    //Assign U2TX to pin RP4 (43)
    UART2Init();
  #endif
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
    TRISDbits.TRISD10 = 0;
    EnablePullUpCN59;

    return TRUE;
}  // InitializeSystem


void BlinkStatus(void) {
//  static unsigned int led_count = 0;
//
//  if(led_count-- == 0) led_count = 300;
//
//  if(DemoState <= DEMO_STATE_IDLE) {
//      mLED_0_Off();
//      mLED_1_Off();
//  } else if (DemoState < DEMO_STATE_CONNECTED) {
//      mLED_0_On();
//      mLED_1_On();
//  } else if(DemoState < DEMO_STATE_ERROR) {
//    if (led_count == 0) {
//          mLED_0_Toggle();
//#ifdef mLED_1
//          mLED_1 = !mLED_0;       // Alternate blink
//#endif
//    }
//  } else {
//    if (led_count == 0) {
//      mLED_0_Toggle();
//#ifdef mLED_1
//      mLED_1 = mLED_0;        // Both blink at the same time
//#endif
//      led_count = 50;
//    }
//      DelayMs(1); // 1ms delay
//  }
}  // BlinkStatus

typedef enum {
  MAIN_STATE_WAIT_CONNECT,
  MAIN_STATE_WAIT_READY,
  MAIN_STATE_RUN
} MAIN_STATE;

static MAIN_STATE state = MAIN_STATE_WAIT_CONNECT;

void ChannelRecv(ADB_CHANNEL_HANDLE h, const void* data, UINT32 data_len) {
  UINT32 i;
  UART2PrintString("******** ");
  for (i = 0; i < data_len; ++i) {
    BYTE b = ((const BYTE*) data)[i];
    UART2PutChar(b);
    if (b == 'n') {
      PORTDbits.RD10 = 1;
    } else if (b == 'f') {
      PORTDbits.RD10 = 0;
    }
  }
  UART2PrintString("\r\n");
}

int main(void) {
  ADB_CHANNEL_HANDLE h;
  BOOL prev_button = 1;
  // Initialize the processor and peripherals.
  if (!InitializeSystem()) {
    UART2PrintString("\r\n\r\nCould not initialize USB Custom Demo App - system.  Halting.\r\n\r\n");
    while (1);
  }
  ADBInit();

  while (1) {
    BOOL button;
    BYTE c;
    BOOL connected = ADBTasks();
    if (!connected) {
      state = MAIN_STATE_WAIT_CONNECT;
    }
    
    switch(state) {
     case MAIN_STATE_WAIT_CONNECT:
      if (connected) {
        print0("ADB connected!");
        h = ADBOpen("tcp:4356", &ChannelRecv);
        state = MAIN_STATE_WAIT_READY;
      }
      break;

     case MAIN_STATE_WAIT_READY:
      if (ADBChannelReady(h)) {
        state = MAIN_STATE_RUN;
      }
      break;

     case MAIN_STATE_RUN:
      button = PORTEbits.RE1;
      if (button != prev_button) {
        c = button ? 'f' : 'n';
        ADBWrite(h, &c, 1);
        prev_button = button;
        state = MAIN_STATE_WAIT_READY;
      }
      break;
    }
    //DelayMs(100);
  }

  return 0;
}  // main
