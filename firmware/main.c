/******************************************************************************
            USB Custom Demo, Host

This file provides the main entry point to the Microchip USB Custom
Host demo.  This demo shows how a PIC24F system could be used to
act as the host, controlling a USB device running the Microchip Custom
Device demo.

******************************************************************************/

/******************************************************************************
* Filename:        main.c
* Dependancies:    USB Host Driver with Generic Client Driver
* Processor:       PIC24F256GB1xx
* Hardware:        Explorer 16 with USB PICtail Plus
* Compiler:        C30 v2.01/C32 v0.00.18
* Company:         Microchip Technology, Inc.

Software License Agreement

The software supplied herewith by Microchip Technology Incorporated
(the “Company”) for its PICmicro® Microcontroller is intended and
supplied to you, the Company’s customer, for use solely and
exclusively on Microchip PICmicro Microcontroller products. The
software is owned by the Company and/or its supplier, and is
protected under applicable copyright laws. All rights are reserved.
Any use in violation of the foregoing restrictions may subject the
user to criminal sanctions under applicable laws, as well as to
civil liability for the breach of the terms and conditions of this
license.

THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.


*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "timer.h"
#include "GenericTypeDefs.h"
#include "adb_packet.h"
#include "USB/usb.h"
#include "HardwareProfile.h"
#include "usb_config.h"
#include "usb_host_android.h"
#include "logging.h"

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

// Application States
typedef enum {
    DEMO_INITIALIZE = 0,                // Initialize the app when a device is attached
    DEMO_STATE_IDLE,                    // Inactive State
    DEMO_STATE_WAIT_SEND_CNXN,
    DEMO_STATE_WAIT_RECV_CNXN,
    DEMO_STATE_CONNECTED,
    DEMO_STATE_ERROR                    // An error has occured
} DEMO_STATE;

BOOL        DeviceAttached;
DEMO_STATE  DemoState;      // Current state of the demo application





BOOL InitializeSystem(void) {
#if defined(__PIC24FJ256DA206__)
  iPPSInput(IN_FN_PPS_U2RX,IN_PIN_PPS_RP2);       //Assign U2RX to pin RP2 (42)
  iPPSOutput(OUT_PIN_PPS_RP4,OUT_FN_PPS_U2TX);    //Assign U2TX to pin RP4 (43)
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
    UART2Init();

    DemoState = DEMO_INITIALIZE;
    DeviceAttached = FALSE;
    ADBPacketReset();

    return TRUE;
}  // InitializeSystem


BOOL CheckForNewAttach(void) {
  if (!DeviceAttached  && USBHostAndroidIsDeviceAttached()) {
    DeviceAttached = TRUE;
    ANDROID_DEVICE_ID DevID;
    USBHostAndroidGetDeviceId(&DevID);
    UART2PrintString("Generic demo device attached - polled, deviceAddress=");
    UART2PutDec(DevID.deviceAddress);
    UART2PrintString("\r\n");
    return TRUE;
  }
  return FALSE;
}  // CheckForNewAttach


void BlinkStatus(void) {
  static unsigned int led_count = 0;

  if(led_count-- == 0) led_count = 300;

  if(DemoState <= DEMO_STATE_IDLE) {
      mLED_0_Off();
      mLED_1_Off();
  } else if (DemoState < DEMO_STATE_CONNECTED) {
      mLED_0_On();
      mLED_1_On();
  } else if(DemoState < DEMO_STATE_ERROR) {
    if (led_count == 0) {
          mLED_0_Toggle();
#ifdef mLED_1
          mLED_1 = !mLED_0;       // Alternate blink
#endif
    }
  } else {
    if (led_count == 0) {
      mLED_0_Toggle();
#ifdef mLED_1
      mLED_1 = mLED_0;        // Both blink at the same time
#endif
      led_count = 50;
    }
      DelayMs(1); // 1ms delay
  }
}  // BlinkStatus


typedef struct amessage {
  UINT32 command;       /* command identifier constant      */
  UINT32 arg0;          /* first argument                   */
  UINT32 arg1;          /* second argument                  */
  UINT32 data_length;   /* length of payload (0 is allowed) */
  UINT32 data_check;    /* checksum of data payload         */
  UINT32 magic;         /* command ^ 0xffffffff             */
} amessage;


#define MAX_PAYLOAD 4096
#define A_SYNC 0x434e5953
#define A_CNXN 0x4e584e43
#define A_OPEN 0x4e45504f
#define A_OKAY 0x59414b4f
#define A_CLSE 0x45534c43
#define A_WRTE 0x45545257
#define A_VERSION 0x01000000        // ADB protocol version
char desc[] = "host::";

void ManageDemoState(void) {
  ADB_RESULT adb_res;
  UINT32 cmd, arg0, arg1, data_len;
  void* recv_data;
  BlinkStatus();

  switch (DemoState) {
   case DEMO_INITIALIZE:
    DemoState = DEMO_STATE_IDLE;
    break;

   case DEMO_STATE_IDLE:
    if (CheckForNewAttach()) {
      UART2PrintString("Sending message... ");
      ADBPacketSend(A_CNXN, A_VERSION, ADB_PACKET_MAX_RECV_DATA_BYTES, desc, sizeof desc);
      DemoState = DEMO_STATE_WAIT_SEND_CNXN;
    }
    break;

   case DEMO_STATE_WAIT_SEND_CNXN:
    if ((adb_res = ADBPacketSendStatus()) != ADB_RESULT_BUSY) {
      if (adb_res == ADB_RESULT_OK) {
        ADBPacketRecv();
        DemoState = DEMO_STATE_WAIT_RECV_CNXN;
      } else {
          print0("Tx error");
          DemoState = DEMO_STATE_ERROR;
      }
    } else {
      UART2PutChar('.');
    }
    break;

   case DEMO_STATE_WAIT_RECV_CNXN:
    if ((adb_res = ADBPacketRecvStatus(&cmd, &arg0, &arg1, &recv_data, &data_len)) != ADB_RESULT_BUSY) {
      if (adb_res == ADB_RESULT_OK && cmd == A_CNXN) {
        print1("Got response from %s", (const char*) recv_data);
        DemoState = DEMO_STATE_CONNECTED;
      } else {
          print0("Rx failed");
      }
    }
    break;

   case DEMO_STATE_ERROR:
    break;

   case DEMO_STATE_CONNECTED:
    break;

   default:
    DemoState = DEMO_INITIALIZE;
    break;
  }

  DelayMs(1); // 1ms delay
}  // ManageDemoState


BOOL USB_ApplicationEventHandler(BYTE address, USB_EVENT event, void *data, DWORD size) {
  // Handle specific events.
  switch (event) {
   case EVENT_VBUS_REQUEST_POWER:
    // We'll let anything attach.
    return TRUE;

   case EVENT_VBUS_RELEASE_POWER:
    // We aren't keeping track of power.
    return TRUE;

   case EVENT_HUB_ATTACH:
    UART2PrintString("\r\n***** USB Error - hubs are not supported *****\r\n");
    return TRUE;

   case EVENT_UNSUPPORTED_DEVICE:
    UART2PrintString("\r\n***** USB Error - device is not supported *****\r\n");
    return TRUE;

   case EVENT_CANNOT_ENUMERATE:
    UART2PrintString("\r\n***** USB Error - cannot enumerate device *****\r\n");
    return TRUE;

   case EVENT_CLIENT_INIT_ERROR:
    UART2PrintString("\r\n***** USB Error - client driver initialization error *****\r\n");
    return TRUE;

   case EVENT_OUT_OF_MEMORY:
    UART2PrintString("\r\n***** USB Error - out of heap memory *****\r\n");
    return TRUE;

   case EVENT_UNSPECIFIED_ERROR:   // This should never be generated.
    UART2PrintString("\r\n***** USB Error - unspecified *****\r\n");
    return TRUE;

   case EVENT_DETACH:
    DemoState = DEMO_INITIALIZE;
    DeviceAttached = FALSE;
    return TRUE;

   case EVENT_SUSPEND:
   case EVENT_RESUME:
    return TRUE;

   default:
    return FALSE;
  }
}  // USB_ApplicationEventHandler


int main(void) {
  // Initialize the processor and peripherals.
  if (!InitializeSystem()) {
    UART2PrintString("\r\n\r\nCould not initialize USB Custom Demo App - system.  Halting.\r\n\r\n");
    while (1);
  }
  if (USBHostInit(0)) {
      UART2PrintString("\r\n\r\n***** USB Custom Demo App Initialized *****\r\n\r\n");
  } else {
    UART2PrintString("\r\n\r\nCould not initialize USB Custom Demo App - USB.  Halting.\r\n\r\n");
    while (1);
  }

  // Main Processing Loop
  while (1) {
    // This demo does not check for overcurrent conditions.  See the
    // USB Host - Data Logger for an example of overcurrent detection
    // with the PIC24F and the USB PICtail Plus.

    // Maintain USB Host State
    USBHostTasks();
    ADBPacketTasks();

#ifndef USB_ENABLE_TRANSFER_EVENT
    USBHostAndroidTasks();
#endif

    // Maintain Demo Application State
    ManageDemoState();
  }

  return 0;
}  // main
