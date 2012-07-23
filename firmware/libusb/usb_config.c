/*
********************************************************************************
                                                                                
Software License Agreement                                                      
                                                                                
Copyright � 2007-2008 Microchip Technology Inc. and its licensors.  All         
rights reserved.                                                                
                                                                                
Microchip licenses to you the right to: (1) install Software on a single        
computer and use the Software with Microchip 16-bit microcontrollers and        
16-bit digital signal controllers ("Microchip Product"); and (2) at your        
own discretion and risk, use, modify, copy and distribute the device            
driver files of the Software that are provided to you in Source Code;           
provided that such Device Drivers are only used with Microchip Products         
and that no open source or free software is incorporated into the Device        
Drivers without Microchip's prior written consent in each instance.             
                                                                                
You should refer to the license agreement accompanying this Software for        
additional information regarding your rights and obligations.                   
                                                                                
SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY         
KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY              
WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A          
PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE             
LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY,               
CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY           
DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY         
INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR         
LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY,                 
SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY         
DEFENSE THEREOF), OR OTHER SIMILAR COSTS.                                       
                                                                                
********************************************************************************
*/

// Created by the Microchip USBConfig Utility, Version 0.0.12.0, 3/28/2008, 8:58:18

#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "usb.h"
#include "usb_host_android.h"
#include "usb_host_bluetooth.h"

// *****************************************************************************
// Client Driver Function Pointer Table for the USB Embedded Host foundation
// *****************************************************************************

CLIENT_DRIVER_TABLE usbClientDrvTable[] =
{                                        
    {
        USBHostAndroidInitInterface,
        USBHostAndroidEventHandler,
        1
    },
#ifndef DISABLE_ACCESSORY
    {
        USBHostAndroidInitDevice,
        USBHostAndroidEventHandler,
        0
    },
#endif
#ifndef DISABLE_BLUETOOTH
    {
        USBHostBluetoothInit,
        USBHostBluetoothEventHandler,
        0
    }
#endif
};

// an enum to match the indices of the drivers in the table according to macros
typedef enum {
  AND_INT,
#ifndef DISABLE_ACCESSORY
  AND_DEV,
#endif
#ifndef DISABLE_BLUETOOTH
  BT_DEV
#endif
} INDEX;


// *****************************************************************************
// USB Embedded Host Targeted Peripheral List (TPL)
// *****************************************************************************

USB_TPL usbTPL[] =
{
    { INIT_CL_SC_P( 0xFFul, 0x42ul, 0x01ul ), 0, AND_INT, {TPL_CLASS_DRV | TPL_INTFC_DRV} },  // ADB
#ifndef DISABLE_BLUETOOTH
    { INIT_CL_SC_P( 0xE0ul, 0x01ul, 0x01ul ), 0, BT_DEV,  {TPL_CLASS_DRV | TPL_DEVICE_DRV} }, // Bluetooth
#endif
#ifndef DISABLE_ACCESSORY
    { INIT_VID_PID( 0x18D1ul, 0x2D00ul )    , 0, AND_DEV, {TPL_DEVICE_DRV} },                 // Accessory without ADB
    { INIT_VID_PID( 0x18D1ul, 0x2D01ul )    , 0, AND_DEV, {TPL_DEVICE_DRV} },                 // Accessory with ADB
#endif
};

