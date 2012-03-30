/*******************************************************************************

    USB OTG (Header File)

Description:
    This file provides the interface for a USB OTG
    application.

    This header file must be included after the application-specific
    usb_config.h file, as usb_config.h configures parts of this file.

Summary:
    This file provides the interface for a USB OTG
    application.

*******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************

* FileName:        usb_otg.h
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

Author          Date    Comments
--------------------------------------------------------------------------------
MR       9-04-2008 First release

*******************************************************************************/

#ifndef __USBOTG_H__
#define __USBOTG_H__
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: OTG Firmware Version
// *****************************************************************************
// *****************************************************************************

#define USB_OTG_FW_MAJOR_VER   1       // Firmware version, major release number.
#define USB_OTG_FW_MINOR_VER   0       // Firmware version, minor release number.
#define USB_OTG_FW_DOT_VER     0       // Firmware version, dot release number.

// *****************************************************************************
// *****************************************************************************
// Section: USB Constants
// *****************************************************************************
// *****************************************************************************

//OTG Events
#define OTG_EVENT_DISCONNECT                0
#define OTG_EVENT_CONNECT                      1
#define OTG_EVENT_NONE                            2
#define OTG_EVENT_SRP_DPLUS_HIGH        3
#define OTG_EVENT_SRP_DPLUS_LOW         4
#define OTG_EVENT_SRP_VBUS_HIGH          5
#define OTG_EVENT_SRP_VBUS_LOW           6
#define OTG_EVENT_SRP_CONNECT              7
#define OTG_EVENT_HNP_ABORT                 8
#define OTG_EVENT_HNP_FAILED                9
#define OTG_EVENT_SRP_FAILED                10
#define OTG_EVENT_RESUME_SIGNALING   11


//Role Defines
#define ROLE_DEVICE     0
#define ROLE_HOST       1

//Cable Defines
#define CABLE_A_SIDE    0
#define CABLE_B_SIDE    1

//Session Defines
#define START_SESSION        0
#define END_SESSION          1
#define TOGGLE_SESSION       2

//USB OTG Timing Parameter Defines
#define DELAY_TB_AIDL_BDIS        10 //100
#define DELAY_TA_BIDL_ADIS        10//150
#define DELAY_TB_ASE0_BRST       100
#define DELAY_TB_SE0_SRP           2
#define DELAY_TB_DATA_PLS         6
#define DELAY_TB_SRP_FAIL          5100
#define DELAY_TA_WAIT_VRISE     100
#define DELAY_TA_WAIT_BCON      1100
#define DELAY_TA_BDIS_ACON       1
#define DELAY_VBUS_SETTLE          500
#define DELAY_TA_AIDL_BDIS        255

// *****************************************************************************
// *****************************************************************************
// Section: HNP Event Flow
// *****************************************************************************
// *****************************************************************************
/*
// *****************************************************************************
                                (A becomes Device, B becomes Host)
        A side(Host)                                                                B side(Device)
// *****************************************************************************
Suspend-SelectRole(ROLE_DEVICE)         ------->    Idle-SelectRole(ROLE_HOST)
Detach - OTG_EVENT_DISCONNECT         <-------    D+ Pullup Disabled
D+ Pullup Enabled                                    ------->   Attach - OTG_EVENT_CONNECT

// *****************************************************************************
                                (A becomes Host, B becomes Device)
        A side(Device)                                                              B side(Host)
// *****************************************************************************
Idle-SelectRole(ROLE_HOST)                    <-------    Suspend-SelectRole(ROLE_DEVICE)
D+ Pullup Disabled                                   ------->    Detach - OTG_EVENT_DISCONNECT
USGOTGInitializeHostStack()
*/

// *****************************************************************************
// *****************************************************************************
// Section: USB OTG Function Prototypes
// *****************************************************************************
// *****************************************************************************

//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
    void InitializeHostStack()

  Description:
    This function initializes the host stack for use in an OTG application

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void USBOTGInitializeHostStack();
 //DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
    void InitializeDeviceStack()

  Description:
    This function initializes the device stack for use in an OTG application

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void USBOTGInitializeDeviceStack();
  //DOM-IGNORE-END


/****************************************************************************
  Function:
 BOOL USBOTGRoleSwitch()

  Description:
    This function returns whether a role switch occurred or not.  This is used by the main application function
    to determine when to reinitialize the system (InitializeSystem())

  Precondition:
    None

  Parameters:
    None

  Return Values:
    BOOL - TRUE or FALSE

  Remarks:
    None
  ***************************************************************************/
BOOL USBOTGRoleSwitch();


/****************************************************************************
  Function:
 void USBOTGClearRoleSwitch()

  Description:
    This function clears the RoleSwitch variable.  After the main function detects the RoleSwitch
    and re-initializes the system, this function should be called to clear the RoleSwitch flag

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void USBOTGClearRoleSwitch();


/****************************************************************************
  Function:
    void USBOTGInitialize()

  Description:
    This function initializes an OTG application and initializes a default role of Host or Device

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
  #define USB_MICRO_AB_OTG_CABLE should be commented out in usb_config.h
  if not using a micro AB OTG cable
  ***************************************************************************/
void USBOTGInitialize();


/****************************************************************************
  Function:
   void USBOTGSelectRole(BOOL role)

  Description:
    This function initiates a role switch via the Host Negotiation Protocol (HNP).
    The parameter role that is passed to this function is the desired role to switch to.

  Precondition:
    None

  Parameters:
    BOOL role - ROLE_DEVICE or ROLE_HOST

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void USBOTGSelectRole(BOOL role);


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  BOOL USBOTGHnpIsEnabled()

  Description:
    This function returns TRUE if HNP is enabled, FALSE otherwise

  Precondition:
    None

  Parameters:
    BOOL - TRUE or FALSE

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
BOOL USBOTGHnpIsEnabled();
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  BOOL USBOTGHnpIsActive()

  Description:
    This function returns TRUE if HNP is active, FALSE otherwise.

  Precondition:
    None

  Parameters:
    BOOL - TRUE or FALSE

  Return Values:
    None

  Remarks:
    HNP will become active on the host when it suspends the bus and HNP was enabled
    by an acknowledegement of the SET_FEATURE(b_hnp_enable) by the peripheral.

    HNP will become active on the peripheral side when it receives a bus idle condition
    and HNP was enabled by a SET_FEATURE(b_hnp_enable) from the host
  ***************************************************************************/
BOOL USBOTGHnpIsActive();
//DOM-IGNORE-END


/****************************************************************************
  Function:
  void USBOTGSession(BYTE Value)

  Description:
    This function starts, ends, or toggles a VBUS session.

  Precondition:
    This function assumes I/O controlling DC/DC converter has already been initialized

  Parameters:
    Value - START_SESSION, END_SESSION, TOGGLE_SESSION

  Return Values:
    TRUE - Session Started, FALSE - Session Not Started

  Remarks:
  This function should only be called by an A-side Host
  ***************************************************************************/
BOOL USBOTGSession(BYTE Value);


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  void USBOTGEnableHnp()

  Description:
    This function enables HNP

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void USBOTGEnableHnp();
  //DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  void USBOTGDisableHnp()

  Description:
    This function disables HNP

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void USBOTGDisableHnp();
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  void void USBOTGEnableAltHnp()

  Description:
    This function enables Alt HNP

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void USBOTGEnableAltHnp();
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  void USBOTGDisableAltHnp()

  Description:
    This function disables Alt HNP

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void USBOTGDisableAltHnp();
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  void USBOTGEnableSupportHnp()

  Description:
    This function enables HNP Support

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void USBOTGEnableSupportHnp();
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  void USBOTGDisableSupportHnp()

  Description:
    This function disables HNP Support

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void USBOTGDisableSupportHnp();
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  BOOL USBOTGSrpIsActive()

  Description:
    This function returns TRUE if SRP is active, FALSE otherwise

  Precondition:
    None

  Parameters:
    BOOL - TRUE or FALSE

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
BOOL USBOTGSrpIsActive();
//DOM-IGNORE-END

//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  void USBOTGDeactivateHnp()

  Description:
    This function deactivates HNP

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void USBOTGDeactivateHnp();
//DOM-IGNORE-END


/****************************************************************************
  Function:
  BYTE USBOTGCurrentRoleIs()

  Description:
    This function returns whether the current role is ROLE_HOST or ROLE_DEVICE

  Precondition:
    None

  Parameters:
    None

  Return Values:
    BYTE  - ROLE_HOST or ROLE_DEVICE

  Remarks:
    None
  ***************************************************************************/
BYTE USBOTGCurrentRoleIs();



/****************************************************************************
  Function:
  BYTE USBOTGDefaultRoleIs()

  Description:
    This function returns whether the default role is ROLE_HOST or ROLE_DEVICE

  Precondition:
    None

  Parameters:
    None

  Return Values:
    BYTE  - ROLE_HOST or ROLE_DEVICE

  Remarks:
    If using a Micro AB USB OTG Cable, the A-side plug of the cable when plugged in
    will assign a default role of ROLE_HOST.  The B-side plug of the cable when plugged in
    will assign a default role of ROLE_DEVICE.

    If using a Standard USB Cable, ROLE_HOST or ROLE_DEVICE needs to be manually configured in
    usb_config.h.

    Both of these items can be easily configured using the USB Config Tool which will automatically
    generate the apropriate information for your application
  ***************************************************************************/
BYTE USBOTGDefaultRoleIs();


/****************************************************************************
  Function:
   void USBOTGRequestSession()

  Description:
    This function requests a Session from an A side Host using the Session Request Protocol (SRP).
    The function will return TRUE if the request was successful or FALSE otherwise.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    TRUE or FALSE

  Remarks:
    This function should only be called by a B side Device.
  ***************************************************************************/
BOOL USBOTGRequestSession();


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
   BOOL USBOTGGetSessionStatus()

  Description:
    This function gets a session status.  The function will return
    TRUE if VBUS > Session Valid Voltage or FALSE if VBUS < Session Valid Voltage.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    TRUE or FALSE

  Remarks:

  ***************************************************************************/
BOOL USBOTGGetSessionStatus();
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
   void USBOTGDischargeVBus()

  Description:
    This function discharges VBUS.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:

  ***************************************************************************/
void USBOTGDischargeVBus();
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  void USB_OTGEventHandler ( BYTE address, BYTE event, void *data, DWORD size )

  Description:
    This function is the event handler used by both the Host and Device stacks for calling the OTG layer
    when SRP and HNP events occur

  Precondition:
    None

  Parameters:
    BYTE event -
                 OTG_EVENT_SRP_DPLUS_HIGH
                 OTG_EVENT_SRP_DPLUS_LOW
                 OTG_EVENT_SRP_VBUS_HIGH
                 OTG_EVENT_SRP_VBUS_LOW
                 OTG_EVENT_DISCONNECT
                 OTG_EVENT_CONNECT
                 OTG_EVENT_HNP_ABORT
                 OTG_EVENT_HNP_FAILED

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void USB_OTGEventHandler ( BYTE address, BYTE event, void *data, DWORD size );
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  void USBOTGDelayMs(WORD time)

  Description:
    This function will delay a given amount of time in milliseconds determined by the time parameter
    passed to this function.  The function uses the hardware based 1 millisecond timer.

  Precondition:
    USB Module Must Be Enabled Prior To Calling This Function (U1PWRCbits.USBPWR = 1)

  Parameters:
    WORD time - The time to delay in milliseconds

  Return Values:
    BOOL - TRUE  - Time Not Expired
                FALSE - Time Expired

  Remarks:
    Assumes USB Interrupt Is Disabled
  ***************************************************************************/
void USBOTGDelayMs(WORD time);
//DOM-IGNORE-END



//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  BOOL USBOTGIsHNPTimeOutExpired()

  Description:
    This function decrements HNPTimeOut and checks to see if HNPTimeOut has expired.  This function
    returns TRUE if HNPTimeOut has expired, FALSE otherwise.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    BOOL - TRUE - Time Expired
                FALSE - Time Not Expired

  Remarks:
  HNPTimeOut value should be > 0
  ***************************************************************************/
BOOL USBOTGIsHNPTimeOutExpired();
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  BOOL USBOTGGetHNPTimeOutFlag()

  Description:
    This function returns the HNPTimeOutFlag.  This flag is used for timing the TB_ASE0_BRST USB OTG
    Timing parameter.  This flag is checked in the 1ms Timer interrupt handler.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    BOOL - TRUE or FALSE

  Remarks:
    None
  ***************************************************************************/
BOOL USBOTGGetHNPTimeOutFlag();
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  BOOL USBOTGIsSRPTimeOutExpired()

  Description:
    This function decrements SRPTimeOut and checks to see if SRPTimeOut has expired.  This function
    returns TRUE if SRPTimeOut has expired, FALSE otherwise.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    BOOL - TRUE - Time Expired
                FALSE - Time Not Expired

  Remarks:
  HNPTimeOut value should be > 0
  ***************************************************************************/
BOOL USBOTGIsSRPTimeOutExpired();
//DOM-IGNORE-END



//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  BOOL USBOTGGetSRPTimeOutFlag()

  Description:
    This function returns the SRPTimeOutFlag.  This flag is used for timing the TA_WAIT_BCON USB OTG
    Timing parameter.  This flag is checked in the 1ms Timer interrupt handler.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    BOOL - TRUE or FALSE

  Remarks:
    None
  ***************************************************************************/
BOOL USBOTGGetSRPTimeOutFlag();
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  void USBOTGClearSRPTimeOutFlag()

  Description:
    This function clears the SRPTimeOutFlag.  This flag is checked in the 1ms
    Timer interrupt handler.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void USBOTGClearSRPTimeOutFlag();
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  BOOL USBOTGSRPIsReady()

  Description:
    This function returns the value of SRPReady.  This flag is set after the B-device finishes SRP
     and the A-device is ready for the B-device to connect

  Precondition:
    None

  Parameters:
    None

  Return Values:
    BOOL - TRUE or FALSE

  Remarks:
    None
  ***************************************************************************/
BOOL USBOTGSRPIsReady();
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  void USBOTGClearSRPReady()

  Description:
    This function clears SRPReady.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
void USBOTGClearSRPReady();
//DOM-IGNORE-END



#endif
