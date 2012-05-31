/*
********************************************************************************
                                                                                
Software License Agreement                                                      
                                                                                
Copyright © 2007-2008 Microchip Technology Inc. and its licensors.  All         
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

#include "Compiler.h"

#define _USB_CONFIG_VERSION_MAJOR 0
#define _USB_CONFIG_VERSION_MINOR 0
#define _USB_CONFIG_VERSION_DOT   12
#define _USB_CONFIG_VERSION_BUILD 0

#define USB_SUPPORT_HOST
#ifdef ENABLE_OTG
#define USB_MICRO_AB_OTG_CABLE
#define USB_SUPPORT_OTG
#else
#define DEFAULT_ROLE ROLE_HOST
#endif

#ifdef USB_SUPPORT_DEVICE
#define USB_MAX_EP_NUMBER           2  // TODO: check
#define USB_MAX_NUM_INT             2  // TODO: check
#define USB_EP0_BUFF_SIZE           8
#define USB_POLLING
#define USB_PULLUP_OPTION           USB_PULLUP_ENABLE
#define USB_TRANSCEIVER_OPTION      USB_INTERNAL_TRANSCEIVER
#define USB_SPEED_OPTION            USB_FULL_SPEED
#define USB_NUM_STRING_DESCRIPTORS  3
#define self_power                  1

#define USB_USE_CDC
#define CDC_COMM_INTF_ID        0x00
#define CDC_COMM_EP             1
#define CDC_COMM_IN_EP_SIZE     10

#define CDC_DATA_INTF_ID        0x01
#define CDC_DATA_EP             2
#define CDC_DATA_OUT_EP_SIZE    64  // TODO: check
#define CDC_DATA_IN_EP_SIZE     64  // TODO: check

// TODO: check
//#define USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D2 //Send_Break command
#define USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D1 //Set_Line_Coding, Set_Control_Line_State, Get_Line_Coding, and Serial_State commands
#else
#define CDC_COMM_IN_EP_SIZE     0  // required for dummy to compile.
#endif

#define USB_PING_PONG_MODE  USB_PING_PONG__FULL_PING_PONG

// number of TPL entries occupied by the bluetooth driver
#ifdef DISABLE_BLUETOOTH
#define NUM_TPL_BLUETOOTH 0
#else
#define NUM_TPL_BLUETOOTH 1
#endif

// number of TPL entries occupied by the android accessory driver
#ifdef DISABLE_ACCESSORY
#define NUM_TPL_ACCESSORY 0
#else
#define NUM_TPL_ACCESSORY 2
#endif


#define NUM_TPL_ENTRIES (1 + NUM_TPL_BLUETOOTH + NUM_TPL_ACCESSORY)

// #define USB_ENABLE_TRANSFER_EVENT

#define USB_MAX_GENERIC_DEVICES 1
#define USB_NUM_CONTROL_NAKS 450
#define USB_SUPPORT_INTERRUPT_TRANSFERS
#define USB_SUPPORT_BULK_TRANSFERS
#define USB_NUM_INTERRUPT_NAKS 3
#define USB_INITIAL_VBUS_CURRENT (100/2)
#define USB_INSERT_TIME (250+1)
#define USB_HOST_APP_EVENT_HANDLER USB_ApplicationEventHandler
//#define USB_BLUETOOTH_INTERRUPT_HANDLER USBBluetoothEventHandler
