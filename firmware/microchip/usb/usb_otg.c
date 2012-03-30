/******************************************************************************

    USB OTG Layer


*******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************

* File Name:       usb_otg.c
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
MR                      First Release
*******************************************************************************/

#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "Compiler.h"
#include "usb_config.h"
#include "USB/usb.h"
//#include "USB/usb_device.h"  
//#include "USB/usb_host.h"
#include "USB/usb_otg.h"
#include "USB/usb_host_generic.h"
#include "uart2.h"

//#define DEBUG_MODE


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
// Global Variables
// *****************************************************************************
//DEFAULT_ROLE needs to be configured as ROLE_HOST or ROLE_DEVICE if not using a Micro AB OTG cable, see usb_config.h for configuring
BYTE CurrentRole, DefaultRole;   
WORD HNPTimeOut, SRPTimeOut;
BOOL HNPActive=0, SRPActive=0, SRPReady=0, HNPEnable=0, HNPSupport=0, HNPAltSupport=0, HNPTimeOutFlag=0, SRPTimeOutFlag=0;
BOOL RoleSwitch=0;
extern volatile WORD    usbOverrideHostState;

// *****************************************************************************
// Local Prototypes
// *****************************************************************************
void USBOTGActivateSrp();
void USBOTGDeactivateSrp();
void USBOTGActivateHnp();
void USBOTGDisableInterrupts();

//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
    void USBOTGDisableInterrupts()

  Description:
    This function disables the apropriate interrupts

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
//DOM-IGNORE-END
void USBOTGDisableInterrupts()
{
    #if defined(__C30__)
          //Disable All Interrupts
          IEC5 = 0; 
          U1IR=0xFF;
          U1IE=0;
          U1OTGIE &= 0x8C;
          U1OTGIR = 0x7D;
          IFS5 = 0;
      #elif defined( __PIC32MX__ )
          // Disable All Interrupts
          IEC1CLR         = 0x02000000;
          U1IR=0xFF;
          U1IE=0;
          U1OTGIE &= 0x8C;
          U1OTGIR = 0x7D;
          IFS1CLR         = 0x02000000;
      #endif
}

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
//DOM-IGNORE-END
void USBOTGInitializeDeviceStack()
{
    //Disable Interrupts
    USBOTGDisableInterrupts();

    //Enable D+ Pullup
    U1OTGCONbits.DPPULUP = 1;

     //Disable D+ Pulldown
    U1OTGCONbits.DPPULDWN = 0;

    //Switch Role To Device
    CurrentRole = ROLE_DEVICE;

    //Ensure HNP/SRP Fail Timer Check Is Disabled
    U1OTGIEbits.T1MSECIE = 0;
    HNPTimeOutFlag = 0;
    SRPTimeOutFlag = 0;
    SRPReady = 0;
    
    //Initialize Device Stack
    USBDeviceInit();

    //Call Device Tasks
    USBDeviceTasks();

    //Deactivate HNP
    USBOTGDeactivateHnp();

    //Disable HNP
    USBOTGDisableHnp();
          
    //Set Role Switch Flag
    RoleSwitch = TRUE; 
}


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
//DOM-IGNORE-END
void USBOTGInitializeHostStack()
{
    //Enable D+ Pulldown
    U1OTGCONbits.DPPULDWN = 1;

    //Disable D+ Pullup
    U1OTGCONbits.DPPULUP = 0;

    //Switch Role To Host
    CurrentRole = ROLE_HOST;
     
    //Ensure HNP/SRP Fail Timer Check Is Disabled
    U1OTGIEbits.T1MSECIE = 0;
    HNPTimeOutFlag = 0;
    SRPTimeOutFlag = 0;
    SRPReady = 0;

     //If A side Host Then
     if (DefaultRole == ROLE_HOST)
     {
         //Substate Initialize
         usbOverrideHostState = 0;
         USBHostTasks();
     }

     //If B side, Host Stack Needs To Be Manually Initialized Here To Perfom Reset Within TB_ACON_BSE0 (1ms Max)
     else if (DefaultRole == ROLE_DEVICE)
     {
         //Substate Initialize
        usbOverrideHostState = 0;
        USBHostTasks();

        //Issue Power On
        usbOverrideHostState = 0x0020;
        USBHostTasks();

        //Issue Reset
        usbOverrideHostState = 0x0110;
        USBHostTasks();
     }

     //Deactivate HNP
     USBOTGDeactivateHnp();

     //Disable HNP
     USBOTGDisableHnp();
                
     //Set Role Switch Flag
     RoleSwitch = TRUE;
}


//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END
void USBOTGInitialize()
{  
    #ifdef USB_MICRO_AB_OTG_CABLE
        //Power Up Module
        U1PWRCbits.USBPWR = 1;

        //Ensure Fail Timer Check Is Disabled
        U1OTGIEbits.T1MSECIE = 0;
        HNPTimeOutFlag = 0;
        SRPTimeOutFlag = 0;
        SRPReady = 0;

        #if defined(__C30__)
            //Configure VBUS I/O
            PORTGbits.RG12 = 0;
            LATGbits.LATG12 = 0;
            TRISGbits.TRISG12 = 0;
        #elif defined(__PIC32MX__)
            VBUS_Off;
        #endif

        //Initialize VB_SESS_END Interrupt
        U1OTGIR = 0x04;
        U1OTGIEbits.SESENDIE = 1;
         
        //Disable HNP
        USBOTGDisableHnp();

        //Deactivate HNP
        USBOTGDeactivateHnp();
      
        //Enable OTG Features
        U1OTGCONbits.OTGEN = 1;

        //Enable D- Pulldown
        U1OTGCONbits.DMPULDWN = 1;

        //Clear ID Flag
        U1OTGIR = 0x80;

        //Delay Some Time For Module To Detect Cable
        USBOTGDelayMs(40);

         //Clear ID Flag
        U1OTGIR = 0x80;

         //Enable ID Interrupt
        U1OTGIEbits.IDIE = 1;

        //If "A" side Plug Then
        if (U1OTGSTATbits.ID == CABLE_A_SIDE)
        {
            #ifdef DEBUG_MODE
                UART2PrintString( "\r\n***** USB OTG Initialize A Side**********\r\n" );  
            #endif

            //Configure Default Host Role
            DefaultRole=ROLE_HOST;
            CurrentRole=DefaultRole;

             //Set Role Switch Flag
            RoleSwitch = TRUE;

            //Initialize Host Stack
            USBOTGInitializeHostStack();

            //Start Session
            USBOTGSession(START_SESSION);
        }

        //Else, "B" side Plug 
        else
        {
            #ifdef DEBUG_MODE
                UART2PrintString( "\r\n***** USB OTG Initialize B Side**********\r\n" );  
            #endif

            //Configure Default Device Role
            DefaultRole=ROLE_DEVICE;
            CurrentRole=DefaultRole;

            //Set Role Switch Flag
            RoleSwitch = TRUE;

            //Initialize Device Stack
            USBOTGInitializeDeviceStack();
        }
  
   #else
        //Power Up Module
        U1PWRCbits.USBPWR = 1;

        //Ensure HNP Fail Timer Check Is Disabled
        U1OTGIEbits.T1MSECIE = 0;
        HNPTimeOutFlag = 0;
        SRPTimeOutFlag = 0;
        SRPReady = 0;

        #if defined(__C30__)
            //Configure VBUS I/O
            PORTGbits.RG12 = 0;
            LATGbits.LATG12 = 0;
            TRISGbits.TRISG12 = 0;
        #elif defined(__PIC32MX__)
            VBUS_Off;
        #endif
        
         //Disable HNP
        USBOTGDisableHnp();

        //Deactivate HNP
        USBOTGDeactivateHnp();

         //Enable OTG Features
        U1OTGCONbits.OTGEN = 1;

        //Enable D- Pulldown
        U1OTGCONbits.DMPULDWN = 1;

        DefaultRole = DEFAULT_ROLE;
        CurrentRole = DefaultRole; 
        
        if (CurrentRole == ROLE_HOST)
        {  
            #ifdef DEBUG_MODE
                UART2PrintString( "\r\n***** USB OTG Initialize A Side**********\r\n" );  
            #endif

            //Set Role Switch Flag
            RoleSwitch = TRUE;
            
            //Initialize Host Stack
            USBOTGInitializeHostStack();

            //Start Session
            USBOTGSession(START_SESSION);
        }

        else if (CurrentRole == ROLE_DEVICE)
        {
            #ifdef DEBUG_MODE
                UART2PrintString( "\r\n***** USB OTG Initialize B Side**********\r\n" );  
            #endif

            //Set Role Switch Flag
            RoleSwitch = TRUE;
            
            //Initialize Device Stack
            USBOTGInitializeDeviceStack();
        }
   #endif

}


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
   void USBOTGSelectRole(BOOL role)

  Description:
    This function initiates a role switch.  The parameter role that is passed to this function
    is the desired role to switch to.

  Precondition:
    None

  Parameters:
    BOOL role - ROLE_DEVICE or ROLE_HOST

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
//DOM-IGNORE-END
void USBOTGSelectRole(BOOL role)
{
    //If HNP Is Enabled Then
    if (HNPEnable)
    {
        //If Currentrole=Host and role=Device Then
        if(CurrentRole == ROLE_HOST && role == ROLE_DEVICE)
        {
            //If Session Is Valid Then
            if (USBOTGGetSessionStatus())
            {   
                #ifdef DEBUG_MODE
                    if (DefaultRole == ROLE_HOST)
                    {   
                        UART2PrintString( "\r\n*****USB OTG A Event - HNP - A Host Suspend *****\r\n" ); 
                    }
                    else if (DefaultRole == ROLE_DEVICE)
                    {
                        UART2PrintString( "\r\n***** USB OTG B Event - HNP - B Host Suspend *****\r\n" );
                    }
                #endif

                //Disable Interrupts
                USBOTGDisableInterrupts();

                //If A side Host Then
                if (DefaultRole == ROLE_HOST)
                {
                    //Configure Timer For TA_AIDL_BDIS
                    HNPTimeOut = DELAY_TA_AIDL_BDIS;
                    HNPTimeOutFlag = 1;

                    //Enable 1ms Timer Interrupt
                    U1OTGIR = 0x40;
                    U1OTGIEbits.T1MSECIE = 1;

                    //Enable Resume Signaling Interrupt
                    U1IR = 0x20;
                    U1IEbits.RESUMEIE = 1;
                }

                //Enable Detach Interrupt
                U1IR = 0x01;
                U1IEbits.DETACHIE = 1;

                //Enable USB Interrupt
                #if defined(__C30__)
                    IFS5 &= 0xFFBF;
                    IEC5 |= 0x0040;
                #elif defined(__PIC32MX__)
                    IFS1CLR         = 0x02000000;
                    IPC11CLR        = 0x0000FF00;
                    IPC11SET        = 0x00001000;
                    IEC1SET         = 0x02000000;
                #endif

                //Activate Hnp (Used to capture an OTG_EVENT_DISCONNECT)
                USBOTGActivateHnp();

                //Suspend Bus
                U1CONbits.SOFEN = 0;

                //Put the state machine in suspend mode.
                usbOverrideHostState = 0x410;
            }

            //If Session Is Invalid Then
            else
            {
                //Display Some Debug Messages
                #ifdef DEBUG_MODE
                    if (DefaultRole == ROLE_HOST)
                    {   
                        UART2PrintString( "\r\n*****USB OTG A Error - Session Is Invalid - Cannot Suspend *****\r\n" ); 
                    }
                    else if (DefaultRole == ROLE_DEVICE)
                    {
                        UART2PrintString( "\r\n***** USB OTG B Error - Session Is Invalid - Cannot Suspend *****\r\n" );
                    }
                #endif
            }
        }

        //Else If Currentrole=Device and role=Host Then
        else if (CurrentRole == ROLE_DEVICE && role == ROLE_HOST)
        {
            //If B Side Device Then
            if (DefaultRole == ROLE_DEVICE)
            {
                //Delay 4 < time < 150ms IDLE time (TB_AIDL_BDIS)
                USBOTGDelayMs(DELAY_TB_AIDL_BDIS);  
            }      

            //If A side Device Then
            else if (DefaultRole == ROLE_HOST)
            {
                //Delay 3 < time < 200ms IDLE time (TA_BIDL_ADIS)
                USBOTGDelayMs(DELAY_TA_BIDL_ADIS);  
            }

            //Disable Interrupts
            USBOTGDisableInterrupts();

            //If B side Device Then
            if (DefaultRole == ROLE_DEVICE)
            {
                //Activate HNP (Used to capture an OTG_EVENT_CONNECT event)
                USBOTGActivateHnp();

                //Configure Timer For TB_ASE0_BRST Check
                HNPTimeOut = DELAY_TB_ASE0_BRST;
                HNPTimeOutFlag = 1;

                //Enable 1ms Timer Interrupt
                U1OTGIR = 0x40;
                U1OTGIEbits.T1MSECIE = 1;

                //Disable Device Mode
                U1CONbits.USBEN=0;

                //Enable Host Mode
                U1CONbits.HOSTEN = 1;

                //Enable Attach Interrupt to capture OTG_EVENT_CONNECT from other side
                U1IR = 0x40;
                U1IEbits.ATTACHIE = 1;
                
                //Cause an OTG_EVENT_DISCONNECT on other side of cable by
                //disabling D+ Pullup Resistor
                U1OTGCONbits.DPPULUP = 0;
                U1OTGCONbits.DPPULDWN = 1;

                //Wait For Line To Discharge To SE0 State
                while(U1CONbits.SE0 == 0)
                {}

                //Clear Attach Interrupt
                U1IR = 0x40;

                //Enable USB Interrupt
                 #if defined(__C30__)
                    IFS5 &= 0xFFBF;
                    IEC5 |= 0x0040;
                #elif defined(__PIC32MX__)
                    IFS1CLR         = 0x02000000;
                    IPC11CLR        = 0x0000FF00;
                    IPC11SET        = 0x00001000;
                    IEC1SET         = 0x02000000;
                #endif
            }

            //If A side Device Then
            else if (DefaultRole == ROLE_HOST)
            {
                #ifdef DEBUG_MODE
                    UART2PrintString( "\r\n***** USB OTG A Event - Role Switch - Device -> Host *****\r\n" );
                #endif

                //Initialize Host Stack
                USBOTGInitializeHostStack();
            }           
        }         
    }

    //Handles Case If B side Abruptly Ends Role As Host Without Sending A Set Feature
    else if (CurrentRole == ROLE_DEVICE &&  DefaultRole == ROLE_HOST)
    {
        #ifdef DEBUG_MODE
            UART2PrintString( "\r\n***** USB OTG A Event - Role Switch - Device -> Host *****\r\n" );
        #endif

        //Disable Interrupts
        USBOTGDisableInterrupts();
        
        //Delay 3 < time < 200ms IDLE time (TA_BIDL_ADIS)
        USBOTGDelayMs(DELAY_TA_BIDL_ADIS);  

        //Switch Back To Host
        USBOTGInitializeHostStack();
    }
}


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
   BOOL USBOTGRequestSession()

  Description:
    This function requests a Session from an A side Host using SRP.  The function will return 
    TRUE if the request was successful or FALSE otherwise.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    TRUE or FALSE

  Remarks:
    This function should only be called by a B side Device.
  ***************************************************************************/
//DOM-IGNORE-END
BOOL USBOTGRequestSession()
{
    //If B side Device Then
    if (DefaultRole == ROLE_DEVICE)
    {
        //If Session Is Invalid and SRP Is Not In Progress Then
        if (!USBOTGGetSessionStatus() && SRPReady == 0)
        {
            UART2PrintString( "\r\n***** USB OTG B Event - SRP - Trying To Request Session *****\r\n" );
            
            //Initial Conditions Check
                //Discharge VBUS
                USBOTGDischargeVBus();

                //Ensure single ended zero state for 2ms (TB_SE0_SRP)
                if (U1CONbits.SE0 == 1)
                {
                    //Delay TB_SE0_SRP
                    USBOTGDelayMs(DELAY_TB_SE0_SRP);

                    //If Not Single ended zero and line has been stable Then
                    if (U1OTGSTATbits.LSTATE == 1 && U1CONbits.SE0 != 1)
                    {
                        //Display Error Message
                        UART2PrintString( "\r\n***** USB OTG B Error - SRP No SE0 - Cannot Request Session*****\r\n" );
                        return FALSE;
                    }
                }
                else
                {
                    //Display Error Message
                    UART2PrintString( "\r\n***** USB OTG B Error - SRP No SE0 - Cannot Request Session*****\r\n" );
                    return FALSE;
                }
                
            //D+ Pulsing
             #ifdef DEBUG_MODE
                    UART2PrintString( "\r\n***** USB OTG B Event - SRP - D+ Pulsing*****\r\n" );
             #endif
             
                //Ensure OTG Bit Is Set
                U1OTGCONbits.OTGEN = 1;

                //Disable D+ Pulldown
                U1OTGCONbits.DPPULDWN = 0;

                //Enable D+ Pullup
                U1OTGCONbits.DPPULUP = 1;
               
                //5ms <= Delay < 10ms (TB_DATA_PLS)
                USBOTGDelayMs(DELAY_TB_DATA_PLS);

                //Disable D+ Pullup
                U1OTGCONbits.DPPULUP = 0;

            //VBUS Pulsing
            #ifdef DEBUG_MODE
                    UART2PrintString( "\r\n***** USB OTG B Event - SRP - VBUS Pulsing*****\r\n" );
            #endif
                //Delay 10ms To Give A side Time To Get Ready For VBUS Pulsing
                USBOTGDelayMs(10);

                //Enable VBUS
                VBUS_On;

                //Delay 1ms 
                USBOTGDelayMs(1);

                //Disable VBUS
                VBUS_Off;   

                //Discharge VBUS
                U1OTGCONbits.VBUSDIS = 1;

                //Delay some time to allow VBUS to settle
                USBOTGDelayMs(DELAY_VBUS_SETTLE);

                //Disable Discharge Bit
                U1OTGCONbits.VBUSDIS = 0;

                //Configure Timer For TB_SRP_FAIL Check
                SRPTimeOut = (DELAY_TB_SRP_FAIL - DELAY_VBUS_SETTLE);
                SRPTimeOutFlag = 1;

                //Enable 1ms Timer Interrupt
                U1OTGIR = 0x40;
                U1OTGIEbits.T1MSECIE = 1;

                //Indicate SRP Is Ready
                SRPReady = 1;

                return TRUE;
        }
        else
        {
            //Session Is Still Active
            UART2PrintString( "\r\n***** USB OTG B Error - Request Session Failed - Session Is Already Active *****\r\n" );

            return FALSE;
        }
    }

   //Should Never Get Here Unless This Function Is Called By A-side
   return FALSE;
}


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
//DOM-IGNORE-END
BOOL USBOTGGetSessionStatus()
{
     //If VBUS < VA_SESS_VLD Then
     if (U1OTGSTATbits.SESVD == 0)
     {
        //Session Invalid
        return FALSE;
     }
     else
     {
        //Session Valid
        return TRUE;
     } 
}


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
//DOM-IGNORE-END
void USBOTGDischargeVBus()
{
    //Enable VBUS Discharge Enable Bit
    U1OTGCONbits.VBUSDIS = 1;

    //Delay 50ms
    //8k pulldown internal to device + ~60k on the board = ~7k, 6.2uF, time to discharge is ~50ms
    USBOTGDelayMs(50);

    //Disable VBUS Discharge Bit
    U1OTGCONbits.VBUSDIS = 0;
}


//DOM-IGNORE-BEGIN
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
  ***************************************************************************/
//DOM-IGNORE-END
BOOL USBOTGSession(BYTE Value)
{ 
   //If START_SESSION or (TOGGLE_SESSION and VBUS Is Off)
   if (Value == START_SESSION || (Value == TOGGLE_SESSION && VBUS_Status == 0))
   {
        //Enable VBUS
        VBUS_On;
        
        //Wait 100ms for VBUS to rise (TA_WAIT_VRISE)
        USBOTGDelayMs(DELAY_TA_WAIT_VRISE);

        //If PGOOD Is Still Low Then
        if (PGOOD == 0)
        {
           //Disable VBUS
           VBUS_Off;

           //Indicate Error Message
           UART2PrintString( "\r\n*****USB OTG A Error - VBUS Rise Time - B Device Not Supported  *****\r\n" );

           return FALSE;
        }
        
        //If PGOOD is High Then
        else
        {  
            #ifdef  OVERCURRENT_DETECTION
                #if defined(__C30__)
                    //Enable Change Notification For Overcurrent Detection
                    CNEN6bits.CN80IE = 1;
                    IFS1bits.CNIF = 0;
                    IEC1bits.CNIE = 1;
                    IPC4bits.CNIP = 7;
                #elif defined(__PIC32MX__)
                    BYTE temp;
                    //Enable Change Notification For Overcurrent Detection
                    CNCONSET = 0x00008000; // Enable Change Notice module
                    CNENSET= 0x00000004; // Enable CN2
                    PORTB = temp;
                    IPC6SET = 0x00140000; // Set priority level=5
                    IPC6SET = 0x00030000; // Set subpriority level=3
                    IFS1CLR = 0x00000001; // Clear the interrupt flag status bit
                    IEC1SET = 0x00000001; // Enable Change Notice interrupts
                #endif
            #endif
            
            #ifdef DEBUG_MODE
                //Indicate Success
               UART2PrintString( "\r\n***** USB OTG A Event - Session Started *****\r\n" );
            #endif
        }
        
   }

   //If END_SESSION or (TOGGLE_SESSION and VBUS Is On)
   else if (Value == END_SESSION || (Value == TOGGLE_SESSION && VBUS_Status == 1))
   {   
        #ifdef  OVERCURRENT_DETECTION
            #if defined(__C30__)
                //Disable Change Notification For Overcurrent Detection
                IEC1bits.CNIE = 0;
                CNEN6bits.CN80IE = 0;
            #elif defined(__PIC32MX__)
                BYTE temp;
                // Disable Change Notification
                IEC1CLR = 0x00000001; 
                CNENCLR= 0x00000004; 
                PORTB = temp;
            #endif
        #endif
        
        //Disable VBUS
        VBUS_Off;
        
        #ifdef DEBUG_MODE
            if (DefaultRole == ROLE_HOST)
            {   
                UART2PrintString( "\r\n***** USB OTG A Event - Session Ended *****\r\n" );
            }
        #endif    
   }
   
   return TRUE;
}


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
//DOM-IGNORE-END
void USB_OTGEventHandler ( BYTE address, BYTE event, void *data, DWORD size )
{
    switch(event)
    {
        case    OTG_EVENT_SRP_DPLUS_HIGH:  //SRP D+ High Event (Caused by ATTACHIF)
            //If A side Host Then
            if (DefaultRole == ROLE_HOST)
            {    
                 //If Session Is Invalid and VBUS is turned off (For Detecting D+ SRP Pulsing) then
                 if (!USBOTGGetSessionStatus() && VBUS_Status == 0)
                 {
                     //If Full Speed Device That Is Performing the SRP Then
                     if (U1OTGSTATbits.LSTATE == 1 && U1CONbits.JSTATE == 1)
                     {
                        #ifdef DEBUG_MODE
                             UART2PrintString( "\r\n***** USB OTG A Event - SRP - D+ High *****\r\n" );
                        #endif

                        //Enable Session Valid Change Interrupt To Capture VBUS > VA_SESS_VLD
                        U1OTGIR = 0x08;
                        U1OTGIEbits.SESVDIE = 1;

                        //Activate SRP
                        USBOTGActivateSrp();
                     }
                     else
                     {
                         //Deactivate SRP
                         USBOTGDeactivateSrp();   

                         //Indicate Error Message
                         UART2PrintString( "\r\n***** USB OTG A Error - SRP - Low Speed Unsupported Device*****\r\n" );
                     }
                 }
            }
        break;

        case OTG_EVENT_SRP_DPLUS_LOW:  //SRP D+ Low Event (Caused by DETACHIF)
            //If SRP Is Active Then
            if (SRPActive)
            {
                #ifdef DEBUG_MODE
                    UART2PrintString( "\r\n***** USB OTG A Event - SRP - D+ Low *****\r\n" );
                #endif
            }
        break;

        case    OTG_EVENT_SRP_VBUS_HIGH:  //SRP VBUS > VA_SESS_VLD Event (Caused by SESVDIF)
            if (SRPActive)
            {
                #ifdef DEBUG_MODE
                    UART2PrintString( "\r\n***** USB OTG A Event - SRP - VBUS > VA_SESS_VLD *****\r\n" );
                #endif       
            }
        break;
    
        case    OTG_EVENT_SRP_VBUS_LOW:  //SRP VBUS < VA_SESS_VLD Event (Caused by SESVDIF)
            if (SRPActive)
            {
                #ifdef DEBUG_MODE
                    UART2PrintString( "\r\n***** USB OTG A Event - SRP - VBUS < VA_SESS_VLD *****\r\n" );
                #endif

                //Disable Session Valid Change Interrupt
                U1OTGIEbits.SESVDIE = 0;

                //Deactivate SRP
                USBOTGDeactivateSrp();
                
                //Configure Timer For > TA_WAIT_BCON Check
                SRPTimeOut = DELAY_TA_WAIT_BCON;
                SRPTimeOutFlag = 1;

                //Enable 1ms Timer Interrupt
                U1OTGIR = 0x40;
                U1OTGIEbits.T1MSECIE = 1;

                //Set SRP Ready
                SRPReady = 1;

                //Start Session 
                //If The B Side Connects within DELAY_TA_WAIT_BCON
                //this will cause a OTG_EVENT_SRP_CONNECT Event
                USBOTGSession(START_SESSION);
            }
        break;

        case OTG_EVENT_SRP_CONNECT:  //SRP Connect Condition (Caused by ATTACHIF after Session Is Started)
            if (SRPReady)
            {
                //Clear SRP Ready
                SRPReady = 0;
                
                //Disable Time Out
                SRPTimeOutFlag = 0;
            }
        break;
            
        case    OTG_EVENT_DISCONNECT:  //Host Disconnect Condition (Caused by DETACHIF) 
            //If Currentrole is Host And HNP Is Active Then
            if (CurrentRole == ROLE_HOST && HNPActive)
            {              
                        
                //Initialize Device Stack
                //Will Cause an OTG_EVENT_CONNECT Event on other side When D+ Pullup Is Enabled 
                USBOTGInitializeDeviceStack();

                #ifdef DEBUG_MODE
                    if (DefaultRole == ROLE_HOST)
                    {   
                        UART2PrintString( "\r\n***** USB OTG A Event - B Device Disconnect *****\r\n" );  
                        UART2PrintString( "\r\n***** USB OTG A Event - Role Switch - Host -> Device *****\r\n" );  
                    }
                    else if (DefaultRole == ROLE_DEVICE)
                    {
                        UART2PrintString( "\r\n***** USB OTG B Event - A Device Disconnect *****\r\n" );  
                        UART2PrintString( "\r\n***** USB OTG B Event - Role Switch - Host -> Device *****\r\n" );  
                    }
                #endif
            }
        break;

        case    OTG_EVENT_CONNECT:  //Device Connect Condition (Caused by ATTACHIF)
            //If Currentrole is Device And HNP Is Active Then
            if (CurrentRole == ROLE_DEVICE && HNPActive)
            {         
                //Initialize Host Stack
                USBOTGInitializeHostStack();
                
                #ifdef DEBUG_MODE
                    if (DefaultRole == ROLE_HOST)
                    {   
                        UART2PrintString( "\r\n***** USB OTG A Event - Role Switch - Device -> Host *****\r\n" );
                        UART2PrintString( "\r\n***** USB OTG A Event - B Device Connect *****\r\n" );  
                    }
                    else if (DefaultRole == ROLE_DEVICE)
                    {
                        UART2PrintString( "\r\n***** USB OTG B Event - Role Switch - Device -> Host *****\r\n" );
                        UART2PrintString( "\r\n***** USB OTG B Event - A Device Connect *****\r\n" );  
                    }
                #endif
            }
        break;

        case    OTG_EVENT_HNP_ABORT:  //HNP Abort Condition, Caused by Unsupported Device (CASE_HOLDING)
                //If HNP Is Enabled Then
                if (HNPEnable)
                {                            
                    //If A side Host Then
                    if (DefaultRole == ROLE_HOST)
                    {
                        //Switch To Device
                        USBOTGSelectRole(ROLE_DEVICE);

                        #ifdef  DEBUG_MODE
                            //Indicate Error Message
                            UART2PrintString( "\r\n***** USB OTG A Error - HNP - Unsupported Device*****\r\n" );
                        #endif
                    }

                    //If B side Host Then
                    else if (DefaultRole == ROLE_DEVICE)
                    {                     
                        //Initialize Device Stack
                        USBOTGInitializeDeviceStack();

                        #ifdef  DEBUG_MODE
                            //Indicate Error Message
                            UART2PrintString( "\r\n***** USB OTG B Error - HNP Abort - Unsupported Device*****\r\n" );
                        #endif
                    }
                }
        break;

        case    OTG_EVENT_HNP_FAILED:  //HNP Fail Condition, Caused by device not responding When HNP Is Enabled And A Suspend is Issued or Received(T1MSECIF) 
                if (DefaultRole == ROLE_HOST)
                {
                    //Initialize Host Stack
                    USBOTGInitializeHostStack();
                    
                     //End Session
                    USBOTGSession(END_SESSION);
                                     
                    //Indicate Error
                    UART2PrintString( "\r\n***** USB OTG A Error - HNP Failed - Device Not Responding - Session Ended *****\r\n" );
                }
                else if (DefaultRole == ROLE_DEVICE)
                {
                    //Initialize Device Stack
                    USBOTGInitializeDeviceStack();

                    //Indicate Error Message
                    UART2PrintString( "\r\n***** USB OTG B Error - HNP Failed - Device Not Responding *****\r\n" );
                }
        break;

        case    OTG_EVENT_SRP_FAILED:  //SRP Fail Condition, Caused by A or B side not Connecting on a SRP Transition (T1MSECIF)  
                if (DefaultRole == ROLE_HOST)
                {
                    //Clear SRP Ready
                    SRPReady = 0;
       
                    //End Session
                    USBOTGSession(END_SESSION);
                                     
                    //Indicate Error
                    UART2PrintString( "\r\n***** USB OTG A Error - SRP Failed - Device Not Responding - Session Ended *****\r\n" );
                }
                else if (DefaultRole == ROLE_DEVICE)
                {
                     //Clear SRP Ready
                     SRPReady = 0;

                     //SRP Failed
                     UART2PrintString( "\r\n***** USB OTG B Error - SRP Failed - Device Not Responding *****\r\n" );
                }
        break;

        case OTG_EVENT_RESUME_SIGNALING:
            #ifdef DEBUG_MODE
                UART2PrintString("\r\n***** USB OTG A Event - Resume Signaling Detected *****\r\n");
            #endif

            //Disable Timers
            U1OTGIEbits.T1MSECIE = 0;
            HNPTimeOutFlag = 0;
            SRPTimeOutFlag = 0;
            SRPReady = 0;
            
            //Enable Host Resume Signaling
            U1CONbits.RESUME = 1;

            //Hold Resume for >= 20ms
            USBOTGDelayMs(40);

            //Disable Host Resume Signaling
            U1CONbits.RESUME = 0;

            //Send SOF
            U1CONbits.SOFEN = 1;

            //Resume Recovery
            USBOTGDelayMs(10);

            //Setup Normal Run State
            //usbOverrideHostState = 0x400;
            //Issue Reset
            usbOverrideHostState = 0x0110;

            //Deactivate Hnp
            USBOTGDeactivateHnp();

            //Disable Resume Interrupt
            U1IEbits.RESUMEIE = 0;
        break;
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: Interrupt Handlers
// *****************************************************************************
// *****************************************************************************

//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
    void _CNInterrupt(void)

  Description:
    This is the interrupt service routine for the Change Notice interrupt.  The
    following cases are serviced:
         * Overcurrent Detection (PGOOD Goes Low After VBUS Is Enabled)
         
  Precondition:
    MCP1253 Is Enabled and Activated => VBUS = 5V

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
//DOM-IGNORE-END
#ifdef  OVERCURRENT_DETECTION
    #if defined(__C30__)
    void __attribute__((__interrupt__, auto_psv)) _CNInterrupt(void)
    #elif defined(__PIC32MX__)
    #pragma interrupt _CNInterrupt ipl5 vector 26
    void _CNInterrupt( void )
    #endif
    {
        BYTE temp;

        #ifdef DEBUG_MODE
            UART2PrintString( "\r\n***** PGOOD Change Notice Detected *****\r\n" );
        #endif
        
        //Debounce Interrupt To Ensure Wasn't Caused By Inrush
        USBOTGDelayMs(10);

        //If PGOOD Is Still Low
        if (PGOOD == 0)
        {
            //Disable VBUS
            VBUS_Off;

            //Indicate Error
            UART2PrintString( "\r\n***** USB OTG A Error - Overcurrent Condition Detected - Session Ended *****\r\n" );

            #if defined(__C30__)
                //Disable Change Notification 
                IEC1bits.CNIE = 0;
                CNEN6bits.CN80IE = 0;
            #elif defined(__PIC32MX__)
                // Disable Change Notification
                IEC1CLR = 0x00000001; 
                CNENCLR= 0x00000004; 
                PORTB = temp;
            #endif

            //Initialize Host Stack
            USBOTGInitializeHostStack();
        }

        #if defined(__C30__)
            //Clear Interrupt Flag
            IFS1bits.CNIF = 0;
        #elif defined(__PIC32MX__)
            PORTB = temp;
            IFS1CLR = 0x00000001; // Clear the interrupt flag status bit
        #endif
    }
#endif



//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END
BOOL USBOTGRoleSwitch()
{
    return RoleSwitch;
}


//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END
void USBOTGClearRoleSwitch()
{
    RoleSwitch = FALSE;
}

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END
BYTE USBOTGCurrentRoleIs()
{
    return CurrentRole;
}


//DOM-IGNORE-BEGIN
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
    None 
  ***************************************************************************/
//DOM-IGNORE-END
BYTE USBOTGDefaultRoleIs()
{
    return DefaultRole;
}


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
//DOM-IGNORE-END
void USBOTGEnableHnp()
{     
    HNPEnable = 1;  
}


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
//DOM-IGNORE-END
void USBOTGDisableHnp()
{   
    HNPEnable = 0;  
}


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
//DOM-IGNORE-END
void USBOTGEnableAltHnp()
{
    HNPAltSupport = 1;
}


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
//DOM-IGNORE-END
void USBOTGDisableAltHnp()
{
    HNPAltSupport = 0;
}


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
//DOM-IGNORE-END
void USBOTGEnableSupportHnp()
{
    HNPSupport = 1;
}


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
//DOM-IGNORE-END
void USBOTGDisableSupportHnp()
{
    HNPSupport = 0;
}


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
//DOM-IGNORE-END
BOOL USBOTGHnpIsEnabled()
{
    return HNPEnable;
   
}


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  BOOL USBOTGHnpIsActive()
   
  Description:
    This function returns TRUE if HNP is active, FALSE otherwise

  Precondition:
    None

  Parameters:
    BOOL - TRUE or FALSE
                        
  Return Values:
    None

  Remarks:
    None 
  ***************************************************************************/
//DOM-IGNORE-END
BOOL USBOTGHnpIsActive()
{
    return HNPActive;
}


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
//DOM-IGNORE-END
BOOL USBOTGSrpIsActive()
{
    return SRPActive;
}


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  void USBOTGActivateSrp()
   
  Description:
    This function activates SRP

  Precondition:
    None

  Parameters:
    None
                        
  Return Values:
    None

  Remarks:
    None 
  ***************************************************************************/
//DOM-IGNORE-END
void USBOTGActivateSrp()
{
    SRPActive = 1;
}


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  void USBOTGDeactivateSrp()
   
  Description:
    This function Deactivates SRP

  Precondition:
    None

  Parameters:
    None
                        
  Return Values:
    None

  Remarks:
    None 
  ***************************************************************************/
//DOM-IGNORE-END
void USBOTGDeactivateSrp()
{
     SRPActive = 0;
}


//DOM-IGNORE-BEGIN
/****************************************************************************
  Function:
  void USBOTGActivateHnp()
   
  Description:
    This function activates HNP

  Precondition:
    None

  Parameters:
    None
                        
  Return Values:
    None

  Remarks:
    None 
  ***************************************************************************/
//DOM-IGNORE-END
void USBOTGActivateHnp()
{   
    HNPActive = 1;
}


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
//DOM-IGNORE-END
void USBOTGDeactivateHnp()
{   
    HNPActive = 0;
}


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
//DOM-IGNORE-END
BOOL USBOTGGetHNPTimeOutFlag()
{
    return HNPTimeOutFlag;
}


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
  HNPTime should be > 0
  ***************************************************************************/
//DOM-IGNORE-END
BOOL USBOTGIsHNPTimeOutExpired()
{
    //Decrement Count
    HNPTimeOut--;

    if (HNPTimeOut == 0)
    {
        //Clear Timer Flag
        HNPTimeOutFlag = 0;

        // Turn off the timer interrupt.
        U1OTGIEbits.T1MSECIE = 0;

        //Timer Expired
        return TRUE;
    }
    else
    {      
        //Timer Not Expired
        return FALSE;
    }
}


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
  //DOM-IGNORE-END
BOOL USBOTGIsSRPTimeOutExpired()
{
    //Decrement Count
    SRPTimeOut--;

    if (SRPTimeOut == 0)
    {
        //Clear Timer Flag
        SRPTimeOutFlag = 0;

        // Turn off the timer interrupt.
        U1OTGIEbits.T1MSECIE = 0;

        //Timer Expired
        return TRUE;
    }
    else
    {      
        //Timer Not Expired
        return FALSE;
    }
}


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
//DOM-IGNORE-END
BOOL USBOTGGetSRPTimeOutFlag()
{
    return SRPTimeOutFlag;
}


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
 //DOM-IGNORE-END
BOOL USBOTGSRPIsReady()
{
    return SRPReady;
}


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
//DOM-IGNORE-END
void USBOTGClearSRPTimeOutFlag()
{
    SRPTimeOutFlag = 0;
}


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
//DOM-IGNORE-END
void USBOTGClearSRPReady()
{
    SRPReady = 0;
}


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
//DOM-IGNORE-END
void USBOTGDelayMs(WORD time)
{
    WORD i;

    U1OTGIR = 0x40;
    for (i=0; i < time; i++)
    {
        while(U1OTGIRbits.T1MSECIF == 0)
        {}
        U1OTGIR = 0x40;
    }
}


