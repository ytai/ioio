/******************************************************************************

    USB Host Driver Local Header

This file provides local definitions used by the hardware interface for a USB
Host application.

* File Name:       usb_host_local.h
* Dependencies:    None
* Processor:       PIC24/dsPIC30/dsPIC33/PIC32MX
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

Change History:
  Rev         Description
  ----------  ----------------------------------------------------------
  2.6a        Removed extraneous definition
  2.7         No change
  2.7a        Removed freez() macro
*******************************************************************************/

#ifndef _USB_HOST_LOCAL_
#define _USB_HOST_LOCAL_

#include "usb_hal_local.h"


// *****************************************************************************
// *****************************************************************************
// Section: Constants
//
// These constants are internal to the stack.  All constants required by the
// API are in the header file(s).
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// Section: State Machine Constants
// *****************************************************************************

#define STATE_MASK                                      0x0F00  //
#define SUBSTATE_MASK                                   0x00F0  //
#define SUBSUBSTATE_MASK                                0x000F  //

#define NEXT_STATE                                      0x0100  //
#define NEXT_SUBSTATE                                   0x0010  //
#define NEXT_SUBSUBSTATE                                0x0001  //

#define SUBSUBSTATE_ERROR                               0x000F  //

#define NO_STATE                                        0xFFFF  //

/*
*******************************************************************************
DETACHED state machine values

This state machine handles the condition when no device is attached.
*/

#define STATE_DETACHED                                  0x0000  //
#define SUBSTATE_INITIALIZE                             0x0000  //
#define SUBSTATE_WAIT_FOR_POWER                         0x0010  //
#define SUBSTATE_TURN_ON_POWER                          0x0020  //
#define SUBSTATE_WAIT_FOR_DEVICE                        0x0030  //

/*
*******************************************************************************
ATTACHED state machine values

This state machine gets the device descriptor of the remote device.  We get the
size of the device descriptor, and use that size to get the entire device
descriptor.  Then we check the VID and PID and make sure they appear in the TPL.
*/

#define STATE_ATTACHED                                  0x0100  //

#define SUBSTATE_SETTLE                                 0x0000  //
#define SUBSUBSTATE_START_SETTLING_DELAY                0x0000  //
#define SUBSUBSTATE_WAIT_FOR_SETTLING                   0x0001  //
#define SUBSUBSTATE_SETTLING_DONE                       0x0002  //

#define SUBSTATE_RESET_DEVICE                           0x0010  //
#define SUBSUBSTATE_SET_RESET                           0x0000  //
#define SUBSUBSTATE_RESET_WAIT                          0x0001  //
#define SUBSUBSTATE_RESET_RECOVERY                      0x0002  //
#define SUBSUBSTATE_RECOVERY_WAIT                       0x0003  //
#define SUBSUBSTATE_RESET_COMPLETE                      0x0004  //

#define SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE             0x0020  //
#define SUBSUBSTATE_SEND_GET_DEVICE_DESCRIPTOR_SIZE     0x0000  //
#define SUBSUBSTATE_WAIT_FOR_GET_DEVICE_DESCRIPTOR_SIZE 0x0001  //
#define SUBSUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE_COMPLETE 0x0002  //

#define SUBSTATE_GET_DEVICE_DESCRIPTOR                  0x0030  //
#define SUBSUBSTATE_SEND_GET_DEVICE_DESCRIPTOR          0x0000  //
#define SUBSUBSTATE_WAIT_FOR_GET_DEVICE_DESCRIPTOR      0x0001  //
#define SUBSUBSTATE_GET_DEVICE_DESCRIPTOR_COMPLETE      0x0002  //

#define SUBSTATE_VALIDATE_VID_PID                       0x0040  //

/*
*******************************************************************************
ADDRESSING state machine values

This state machine sets the address of the remote device.
*/

#define STATE_ADDRESSING                                0x0200  //

#define SUBSTATE_SET_DEVICE_ADDRESS                     0x0000  //
#define SUBSUBSTATE_SEND_SET_DEVICE_ADDRESS             0x0000  //
#define SUBSUBSTATE_WAIT_FOR_SET_DEVICE_ADDRESS         0x0001  //
#define SUBSUBSTATE_SET_DEVICE_ADDRESS_COMPLETE         0x0002  //

/*
*******************************************************************************
CONFIGURING state machine values

This state machine sets the configuration of the remote device, and sets up
internal variables to support the device.
*/
#define STATE_CONFIGURING                               0x0300  //

#define SUBSTATE_INIT_CONFIGURATION                     0x0000  //

#define SUBSTATE_GET_CONFIG_DESCRIPTOR_SIZE             0x0010  //
#define SUBSUBSTATE_SEND_GET_CONFIG_DESCRIPTOR_SIZE     0x0000  //
#define SUBSUBSTATE_WAIT_FOR_GET_CONFIG_DESCRIPTOR_SIZE 0x0001  //
#define SUBSUBSTATE_GET_CONFIG_DESCRIPTOR_SIZECOMPLETE  0x0002  //

#define SUBSTATE_GET_CONFIG_DESCRIPTOR                  0x0020  //
#define SUBSUBSTATE_SEND_GET_CONFIG_DESCRIPTOR          0x0000  //
#define SUBSUBSTATE_WAIT_FOR_GET_CONFIG_DESCRIPTOR      0x0001  //
#define SUBSUBSTATE_GET_CONFIG_DESCRIPTOR_COMPLETE      0x0002  //

#define SUBSTATE_SELECT_CONFIGURATION                   0x0030  //
#define SUBSUBSTATE_SELECT_CONFIGURATION                0x0000  //
#define SUBSUBSTATE_SEND_SET_OTG                        0x0001  //
#define SUBSUBSTATE_WAIT_FOR_SET_OTG_DONE               0x0002  //
#define SUBSUBSTATE_SET_OTG_COMPLETE                    0x0003  //

#define SUBSTATE_ENABLE_ACCESSORY                       0x0040  //
#define SUBSUBSTATE_SEND_GET_PROTOCOL                   0x0000  //
#define SUBSUBSTATE_WAIT_FOR_GET_PROTOCOL               0x0001  //
#define SUBSUBSTATE_SEND_ACCESSORY_STRING               0x0002  //
#define SUBSUBSTATE_WAIT_FOR_ACCESSORY_STRING           0x0003  //
#define SUBSUBSTATE_SEND_START_ACCESSORY                0x0004  //
#define SUBSUBSTATE_WAIT_FOR_START_ACCESSORY            0x0005  //

#define SUBSTATE_SET_CONFIGURATION                      0x0050  //
#define SUBSUBSTATE_SEND_SET_CONFIGURATION              0x0000  //
#define SUBSUBSTATE_WAIT_FOR_SET_CONFIGURATION          0x0001  //
#define SUBSUBSTATE_SET_CONFIGURATION_COMPLETE          0x0002  //
#define SUBSUBSTATE_INIT_CLIENT_DRIVERS                 0x0003  //

/*
*******************************************************************************
RUNNING state machine values

*/

#define STATE_RUNNING                                   0x0400  //
#define SUBSTATE_NORMAL_RUN                             0x0000  //
#define SUBSTATE_SUSPEND_AND_RESUME                     0x0010  //
#define SUBSUBSTATE_SUSPEND                             0x0000  //
#define SUBSUBSTATE_RESUME                              0x0001  //
#define SUBSUBSTATE_RESUME_WAIT                         0x0002  //
#define SUBSUBSTATE_RESUME_RECOVERY                     0x0003  //
#define SUBSUBSTATE_RESUME_RECOVERY_WAIT                0x0004  //
#define SUBSUBSTATE_RESUME_COMPLETE                     0x0005  //


/*
*******************************************************************************
HOLDING state machine values

*/

#define STATE_HOLDING                                   0x0500  //
#define SUBSTATE_HOLD_INIT                              0x0000  //
#define SUBSTATE_HOLD                                   0x0001  //


// *****************************************************************************
// Section: Token State Machine Constants
// *****************************************************************************

#define TSTATE_MASK                             0x00F0  //
#define TSUBSTATE_MASK                          0x000F  //

#define TSUBSTATE_ERROR                         0x000F  //

#define TSTATE_IDLE                             0x0000  //

#define TSTATE_CONTROL_NO_DATA                  0x0010  //
#define TSUBSTATE_CONTROL_NO_DATA_SETUP         0x0000  //
#define TSUBSTATE_CONTROL_NO_DATA_ACK           0x0001  //
#define TSUBSTATE_CONTROL_NO_DATA_COMPLETE      0x0002  //

#define TSTATE_CONTROL_READ                     0x0020  //
#define TSUBSTATE_CONTROL_READ_SETUP            0x0000  //
#define TSUBSTATE_CONTROL_READ_DATA             0x0001  //
#define TSUBSTATE_CONTROL_READ_ACK              0x0002  //
#define TSUBSTATE_CONTROL_READ_COMPLETE         0x0003  //

#define TSTATE_CONTROL_WRITE                    0x0030  //
#define TSUBSTATE_CONTROL_WRITE_SETUP           0x0000  //
#define TSUBSTATE_CONTROL_WRITE_DATA            0x0001  //
#define TSUBSTATE_CONTROL_WRITE_ACK             0x0002  //
#define TSUBSTATE_CONTROL_WRITE_COMPLETE        0x0003  //

#define TSTATE_INTERRUPT_READ                   0x0040  //
#define TSUBSTATE_INTERRUPT_READ_DATA           0x0000  //
#define TSUBSTATE_INTERRUPT_READ_COMPLETE       0x0001  //

#define TSTATE_INTERRUPT_WRITE                  0x0050  //
#define TSUBSTATE_INTERRUPT_WRITE_DATA          0x0000  //
#define TSUBSTATE_INTERRUPT_WRITE_COMPLETE      0x0001  //

#define TSTATE_ISOCHRONOUS_READ                 0x0060  //
#define TSUBSTATE_ISOCHRONOUS_READ_DATA         0x0000  //
#define TSUBSTATE_ISOCHRONOUS_READ_COMPLETE     0x0001  //

#define TSTATE_ISOCHRONOUS_WRITE                0x0070  //
#define TSUBSTATE_ISOCHRONOUS_WRITE_DATA        0x0000  //
#define TSUBSTATE_ISOCHRONOUS_WRITE_COMPLETE    0x0001  //

#define TSTATE_BULK_READ                        0x0080  //
#define TSUBSTATE_BULK_READ_DATA                0x0000  //
#define TSUBSTATE_BULK_READ_COMPLETE            0x0001  //

#define TSTATE_BULK_WRITE                       0x0090  //
#define TSUBSTATE_BULK_WRITE_DATA               0x0000  //
#define TSUBSTATE_BULK_WRITE_COMPLETE           0x0001  //

//******************************************************************************
// Section: USB Peripheral Constants
//******************************************************************************

// Section: USB Control Register Constants

// Section: U1PWRC

#define USB_SUSPEND_MODE                    0x02    // U1PWRC - Put the module in suspend mode.
#define USB_NORMAL_OPERATION                0x00    // U1PWRC - Normal USB operation
#define USB_ENABLED                         0x01    // U1PWRC - Enable the USB module.
#define USB_DISABLED                        0x00    // U1PWRC - Disable the USB module.

// Section: U1OTGCON

#define USB_DPLUS_PULLUP_ENABLE             0x80    // U1OTGCON - Enable D+ pull-up
#define USB_DMINUS_PULLUP_ENABLE            0x40    // U1OTGCON - Enable D- pull-up
#define USB_DPLUS_PULLDOWN_ENABLE           0x20    // U1OTGCON - Enable D+ pull-down
#define USB_DMINUS_PULLDOWN_ENABLE          0x10    // U1OTGCON - Enable D- pull-down
#define USB_VBUS_ON                         0x08    // U1OTGCON - Enable Vbus
#define USB_OTG_ENABLE                      0x04    // U1OTGCON - Enable OTG
#define USB_VBUS_CHARGE_ENABLE              0x02    // U1OTGCON - Vbus charge line set to 5V
#define USB_VBUS_DISCHARGE_ENABLE           0x01    // U1OTGCON - Discharge Vbus

// Section: U1OTGIE/U1OTGIR

#define USB_INTERRUPT_IDIF                  0x80    // U1OTGIR - ID state change flag
#define USB_INTERRUPT_T1MSECIF              0x40    // U1OTGIR - 1ms timer interrupt flag
#define USB_INTERRUPT_LSTATEIF              0x20    // U1OTGIR - line state stable flag
#define USB_INTERRUPT_ACTIVIF               0x10    // U1OTGIR - bus activity flag
#define USB_INTERRUPT_SESVDIF               0x08    // U1OTGIR - session valid change flag
#define USB_INTERRUPT_SESENDIF              0x04    // U1OTGIR - B-device Vbus change flag
#define USB_INTERRUPT_VBUSVDIF              0x01    // U1OTGIR - A-device Vbus change flag

// Section: U1CON

#define USB_JSTATE_DETECTED                 0x80    // U1CON - J state
#define USB_SE0_DETECTED                    0x40    // U1CON - Single ended 0 detected
#define USB_TOKEN_BUSY                      0x20    // U1CON - Token currently being processed
#define USB_ASSERT_RESET                    0x10    // U1CON - RESET signalling
#define USB_HOST_MODE_ENABLE                0x08    // U1CON - Enable host mode
#define USB_RESUME_ACTIVATED                0x04    // U1CON - RESUME signalling
#define USB_PINGPONG_RESET                  0x02    // U1CON - Reset ping-pong buffer pointer
#define USB_SOF_ENABLE                      0x01    // U1CON - Enable SOF generation
#define USB_SOF_DISABLE                     0x00    // U1CON - Disable SOF generation

// Section: U1CNFG1

#define USB_EYE_PATTERN_TEST                0x80    // U1CFG1 - Enable eye pattern test
#define USB_MONITOR_OE                      0x40    // U1CFG1 - nOE signal active
#define USB_FREEZE_IN_DEBUG_MODE            0x20    // U1CFG1 - Freeze on halt when in debug mode
#define USB_STOP_IN_IDLE_MODE               0x10    // U1CFG1 - Stop module in idle mode
#define USB_PING_PONG__ALL_BUT_EP0          0x03    // U1CFG1 - Ping-pong on all endpoints except EP0
#define USB_PING_PONG__FULL_PING_PONG       0x02    // U1CFG1 - Ping-pong on all endpoints
#define USB_PING_PONG__EP0_OUT_ONLY         0x01    // U1CFG1 - Ping-pong on EP 0 out only
#define USB_PING_PONG__NO_PING_PONG         0x00    // U1CFG1 - No ping-pong

// Section: U1CNFG2

#define USB_VBUS_PULLUP_ENABLE              0x01    // U1CNFG2 - Enable Vbus pull-up
#define USB_EXTERNAL_IIC                    0x08    // U1CNFG2 - External module controlled by I2C
#define USB_VBUS_BOOST_DISABLE              0x04    // U1CNFG2 - Disable Vbus boost
#define USB_VBUS_BOOST_ENABLE               0x00    // U1CNFG2 - Enable Vbus boost
#define USB_VBUS_COMPARE_DISABLE            0x02    // U1CNFG2 - Vbus comparator disabled
#define USB_VBUS_COMPARE_ENABLE             0x00    // U1CNFG2 - Vbus comparator enabled
#define USB_ONCHIP_DISABLE                  0x01    // U1CNFG2 - On-chip transceiver disabled
#define USB_ONCHIP_ENABLE                   0x00    // U1CNFG2 - On-chip transceiver enabled

// Section: U1IE/U1IR

#define USB_INTERRUPT_STALL                     0x80    // U1IE - Stall interrupt enable
#define USB_INTERRUPT_ATTACH                    0x40    // U1IE - Attach interrupt enable
#define USB_INTERRUPT_RESUME                    0x20    // U1IE - Resume interrupt enable
#define USB_INTERRUPT_IDLE                      0x10    // U1IE - Idle interrupt enable
#define USB_INTERRUPT_TRANSFER                  0x08    // U1IE - Transfer Done interrupt enable
#define USB_INTERRUPT_SOF                       0x04    // U1IE - Start of Frame Threshold interrupt enable
#define USB_INTERRUPT_ERROR                     0x02    // U1IE - USB Error interrupt enable
#define USB_INTERRUPT_DETACH                    0x01    // U1IE - Detach interrupt enable


//******************************************************************************
// Section: Other Constants
//******************************************************************************

#define CLIENT_DRIVER_HOST                  0xFF    // Client driver index for indicating the host driver.

#define DTS_DATA0                           0       // DTS bit - DATA0 PID
#define DTS_DATA1                           1       // DTS bit - DATA1 PID

#define UEP_DIRECT_LOW_SPEED                0x80    // UEP0 - Direct connect to low speed device enabled
#define UEP_NO_DIRECT_LOW_SPEED             0x00    // UEP0 - Direct connect to low speed device disabled
#define UEP_RETRY_NAKS                      0x40    // UEP0 - No automatic retry of NAK'd transactions
#define UEP_NO_RETRY_NAKS                   0x00    // UEP0 - Automatic retry of NAK'd transactions
#define UEP_NO_SETUP_TRANSFERS              0x10    // UEP0 - SETUP transfers not allowed
#define UEP_ALLOW_SETUP_TRANSFERS           0x00    // UEP0 - SETUP transfers allowed
#define UEP_RX_ENABLE                       0x08    // UEP0 - Endpoint can receive data
#define UEP_RX_DISABLE                      0x00    // UEP0 - Endpoint cannot receive data
#define UEP_TX_ENABLE                       0x04    // UEP0 - Endpoint can transmit data
#define UEP_TX_DISABLE                      0x00    // UEP0 - Endpoint cannot transmit data
#define UEP_HANDSHAKE_ENABLE                0x01    // UEP0 - Endpoint handshake enabled
#define UEP_HANDSHAKE_DISABLE               0x00    // UEP0 - Endpoint handshake disabled (isochronous endpoints)

#define USB_ENDPOINT_CONTROL_BULK           (UEP_NO_SETUP_TRANSFERS | UEP_RX_ENABLE | UEP_TX_ENABLE | UEP_HANDSHAKE_ENABLE) //
#define USB_ENDPOINT_CONTROL_ISOCHRONOUS    (UEP_NO_SETUP_TRANSFERS | UEP_RX_ENABLE | UEP_TX_ENABLE )                       //
#define USB_ENDPOINT_CONTROL_INTERRUPT      (UEP_NO_SETUP_TRANSFERS | UEP_RX_ENABLE | UEP_TX_ENABLE | UEP_HANDSHAKE_ENABLE) //
#define USB_ENDPOINT_CONTROL_SETUP          (UEP_RX_ENABLE | UEP_TX_ENABLE | UEP_HANDSHAKE_ENABLE)                          //

#define USB_DISABLE_ENDPOINT                0x00    // Value to disable an endpoint.

#define USB_SOF_THRESHOLD_08                0x12    // U1SOF - Threshold for a max packet size of 8
#define USB_SOF_THRESHOLD_16                0x1A    // U1SOF - Threshold for a max packet size of 16
#define USB_SOF_THRESHOLD_32                0x2A    // U1SOF - Threshold for a max packet size of 32
#define USB_SOF_THRESHOLD_64                0x4A    // U1SOF - Threshold for a max packet size of 64

#define USB_1MS_TIMER_FLAG                  0x40
#ifndef USB_INSERT_TIME
    #define USB_INSERT_TIME                 (250+1) // Insertion delay time (spec minimum is 100 ms)
#endif
#define USB_RESET_TIME                      (50+1)  // RESET signaling time - 50ms
#if defined( __C30__ )
    #define USB_RESET_RECOVERY_TIME         (10+1)  // RESET recovery time.
#elif defined( __PIC32MX__ )
    #define USB_RESET_RECOVERY_TIME         (100+1) // RESET recovery time - Changed to 100 ms from 10ms.  Some devices take longer.
#else
    #error Unknown USB_RESET_RECOVERY_TIME
#endif
#define USB_RESUME_TIME                     (20+1)  // RESUME signaling time - 20 ms
#define USB_RESUME_RECOVERY_TIME            (10+1)  // RESUME recovery time - 10 ms


// *****************************************************************************
/* USB Root Hub Information

This structure contains information about the USB root hub.
*/

typedef struct _USB_ROOT_HUB_INFO
{
    union
    {
        struct
        {
            BYTE        bPowerGoodPort0 : 1;    // Power can turned on
        };
        BYTE            val;
    }                   flags;
} USB_ROOT_HUB_INFO;


// *****************************************************************************
/* Event Data

This structure defines the data associated with any USB events (see USB_EVENT)
that can be generated by the USB ISR (see _USB1Interrupt).  These events and
their associated data are placed in an event queue used to synchronize between
the main host-tasks loop (see USBHostTasks) and the ISR.  This queue is required
only if transfer events are being used.  All other events are send directly to
the client drivers.
*/
#if defined( USB_ENABLE_TRANSFER_EVENT )
    typedef struct
    {
        USB_EVENT               event;          // Event that occured.
        union
        {
            HOST_TRANSFER_DATA  TransferData;   // Event: EVENT_TRANSFER,
                                                //        EVENT_BUS_ERROR

            // Additional items needed for new events can be added here.
        };
    } USB_EVENT_DATA;
#endif



// *****************************************************************************
/* Event Queue

This structure defines the queue of USB events that can be generated by the
ISR that need to be synchronized to the USB event tasks loop (see
USB_EVENT_DATA, above).  See "struct_queue.h" for usage and operations.
*/
#if defined( USB_ENABLE_TRANSFER_EVENT )
    #ifndef USB_EVENT_QUEUE_DEPTH
        #define USB_EVENT_QUEUE_DEPTH   4       // Default depth of 4 events
    #endif

    typedef struct _usb_event_queue
    {
        int             head;
        int             tail;
        int             count;
        USB_EVENT_DATA  buffer[USB_EVENT_QUEUE_DEPTH];

    } USB_EVENT_QUEUE;
#endif


/********************************************************************
 * USB Endpoint Control Registers
 *******************************************************************/

// See _UEP data type for EP Control Register definitions in the
// processor-specific header files.

#define UEPList (*((_UEP*)&U1EP0))


//******************************************************************************
//******************************************************************************
// Section: Macros
//
// These macros are all internal to the host layer.
//******************************************************************************
//******************************************************************************

#define _USB_InitErrorCounters()        { numCommandTries   = USB_NUM_COMMAND_TRIES; }
#define _USB_SetDATA01(x)               { pCurrentEndpoint->status.bfNextDATA01 = x; }
#define _USB_SetErrorCode(x)            { usbDeviceInfo.errorCode = x; }
#define _USB_SetHoldState()             { usbHostState = STATE_HOLDING; }
#define _USB_SetNextState()             { usbHostState = (usbHostState & STATE_MASK) + NEXT_STATE; }
#define _USB_SetNextSubState()          { usbHostState = (usbHostState & (STATE_MASK | SUBSTATE_MASK)) + NEXT_SUBSTATE; }
#define _USB_SetNextSubSubState()       { usbHostState =  usbHostState + NEXT_SUBSUBSTATE; }
#define _USB_SetNextTransferState()     { pCurrentEndpoint->transferState ++; }
#define _USB_SetPreviousSubSubState()   { usbHostState =  usbHostState - NEXT_SUBSUBSTATE; }
#define _USB_SetTransferErrorState(x)   { x->transferState = (x->transferState & TSTATE_MASK) | TSUBSTATE_ERROR; }


//******************************************************************************
//******************************************************************************
// Section: Local Prototypes
//******************************************************************************
//******************************************************************************

void                 _USB_CheckCommandAndEnumerationAttempts( void );
BOOL                 _USB_FindClassDriver( BYTE bClass, BYTE bSubClass, BYTE bProtocol, BYTE *pbClientDrv );
BOOL                 _USB_FindDeviceLevelClientDriver( void );
USB_ENDPOINT_INFO *  _USB_FindEndpoint( BYTE endpoint );
USB_INTERFACE_INFO * _USB_FindInterface ( BYTE bInterface, BYTE bAltSetting );
void                 _USB_FindNextToken( void );
BOOL                 _USB_FindServiceEndpoint( BYTE transferType );
void                 _USB_FreeConfigMemory( void );
void                 _USB_FreeMemory( void );
void                 _USB_InitControlRead( USB_ENDPOINT_INFO *pEndpoint, BYTE *pControlData, WORD controlSize,
                              BYTE *pData, WORD size );
void                 _USB_InitControlWrite( USB_ENDPOINT_INFO *pEndpoint, BYTE *pControlData, WORD controlSize,
                               BYTE *pData, WORD size );
void                 _USB_InitRead( USB_ENDPOINT_INFO *pEndpoint, BYTE *pData, WORD size );
void                 _USB_InitWrite( USB_ENDPOINT_INFO *pEndpoint, BYTE *pData, WORD size );
void                 _USB_NotifyClients( BYTE DevAddress, USB_EVENT event, void *data, unsigned int size );
BOOL                 _USB_ParseConfigurationDescriptor( void );
void                 _USB_ResetDATA0( BYTE endpoint );
void                 _USB_SendToken( BYTE endpoint, BYTE tokenType );
void                 _USB_SetBDT( BYTE  direction );
BOOL                 _USB_TransferInProgress( void );
BOOL                 _USB_IsAccessoryDevice( void );


#endif // _USB_HOST_LOCAL_


