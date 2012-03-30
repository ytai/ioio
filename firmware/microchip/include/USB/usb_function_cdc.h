/************************************************************************
  File Information:
    FileName:       usb_function_cdc.h
    Dependencies:   See INCLUDES section
    Processor:      PIC18,PIC24, PIC32 and dsPIC33 USB Microcontrollers
    Hardware:       This code is natively intended to be used on Mirochip USB
                    demo boards.  See www.microchip.com/usb (Software & Tools 
                    section) for list of available platforms.  The firmware may 
                    be modified for use on other USB platforms by editing the
                    HardwareProfile.h and HardwareProfile - [platform].h files.
    Complier:  	    Microchip C18 (for PIC18),C30 (for PIC24 and dsPIC33)
                    and C32 (for PIC32)
    Company:        Microchip Technology, Inc.

    Software License Agreement:

    The software supplied herewith by Microchip Technology Incorporated
    (the "Company") for its PIC® Microcontroller is intended and
    supplied to you, the Company's customer, for use solely and
    exclusively on Microchip PIC Microcontroller products. The
    software is owned by the Company and/or its supplier, and is
    protected under applicable copyright laws. All rights are reserved.
    Any use in violation of the foregoing restrictions may subject the
    user to criminal sanctions under applicable laws, as well as to
    civil liability for the breach of the terms and conditions of this
    license.

    THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
    WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
    TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
    IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
    CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

  Summary:
    This file contains all of functions, macros, definitions, variables,
    datatypes, etc. that are required for usage with the CDC function
    driver. This file should be included in projects that use the CDC
    \function driver.  This file should also be included into the 
    usb_descriptors.c file and any other user file that requires access to the
    CDC interface.
    
    
    
    This file is located in the "\<Install Directory\>\\Microchip\\Include\\USB"
    directory.

  Description:
    USB CDC Function Driver File
    
    This file contains all of functions, macros, definitions, variables,
    datatypes, etc. that are required for usage with the CDC function
    driver. This file should be included in projects that use the CDC
    \function driver.  This file should also be included into the 
    usb_descriptors.c file and any other user file that requires access to the
    CDC interface.
    
    This file is located in the "\<Install Directory\>\\Microchip\\Include\\USB"
    directory.
    
    When including this file in a new project, this file can either be
    referenced from the directory in which it was installed or copied
    directly into the user application folder. If the first method is
    chosen to keep the file located in the folder in which it is installed
    then include paths need to be added so that the library and the
    application both know where to reference each others files. If the
    application folder is located in the same folder as the Microchip
    folder (like the current demo folders), then the following include
    paths need to be added to the application's project:
    
    .
    
    ..\\..\\Microchip\\Include
    
    If a different directory structure is used, modify the paths as
    required. An example using absolute paths instead of relative paths
    would be the following:
    
    C:\\Microchip Solutions\\Microchip\\Include
    
    C:\\Microchip Solutions\\My Demo Application                       

********************************************************************/

/********************************************************************
 Change History:
  Rev    Description
  ----   -----------
  2.3    Decricated the mUSBUSARTIsTxTrfReady() macro.  It is 
         replaced by the USBUSARTIsTxTrfReady() function.

  2.6    Minor definition changes

  2.6a   No Changes
  
  2.9b   Added new CDCNotificationHandler() prototype and related 
         structure definitions useful when sending serial state
         notifications over the CDC interrupt endpoint.

********************************************************************/

#ifndef CDC_H
#define CDC_H

/** I N C L U D E S **********************************************************/
#include "GenericTypeDefs.h"
#include "USB/usb.h"
#include "usb_config.h"

/** D E F I N I T I O N S ****************************************************/

/* Class-Specific Requests */
#define SEND_ENCAPSULATED_COMMAND   0x00
#define GET_ENCAPSULATED_RESPONSE   0x01
#define SET_COMM_FEATURE            0x02
#define GET_COMM_FEATURE            0x03
#define CLEAR_COMM_FEATURE          0x04
#define SET_LINE_CODING             0x20
#define GET_LINE_CODING             0x21
#define SET_CONTROL_LINE_STATE      0x22
#define SEND_BREAK                  0x23

/* Notifications *
 * Note: Notifications are polled over
 * Communication Interface (Interrupt Endpoint)
 */
#define NETWORK_CONNECTION          0x00
#define RESPONSE_AVAILABLE          0x01
#define SERIAL_STATE                0x20


/* Device Class Code */
#define CDC_DEVICE                  0x02

/* Communication Interface Class Code */
#define COMM_INTF                   0x02

/* Communication Interface Class SubClass Codes */
#define ABSTRACT_CONTROL_MODEL      0x02

/* Communication Interface Class Control Protocol Codes */
#define V25TER                      0x01    // Common AT commands ("Hayes(TM)")


/* Data Interface Class Codes */
#define DATA_INTF                   0x0A

/* Data Interface Class Protocol Codes */
#define NO_PROTOCOL                 0x00    // No class specific protocol required


/* Communication Feature Selector Codes */
#define ABSTRACT_STATE              0x01
#define COUNTRY_SETTING             0x02

/* Functional Descriptors */
/* Type Values for the bDscType Field */
#define CS_INTERFACE                0x24
#define CS_ENDPOINT                 0x25

/* bDscSubType in Functional Descriptors */
#define DSC_FN_HEADER               0x00
#define DSC_FN_CALL_MGT             0x01
#define DSC_FN_ACM                  0x02    // ACM - Abstract Control Management
#define DSC_FN_DLM                  0x03    // DLM - Direct Line Managment
#define DSC_FN_TELEPHONE_RINGER     0x04
#define DSC_FN_RPT_CAPABILITIES     0x05
#define DSC_FN_UNION                0x06
#define DSC_FN_COUNTRY_SELECTION    0x07
#define DSC_FN_TEL_OP_MODES         0x08
#define DSC_FN_USB_TERMINAL         0x09
/* more.... see Table 25 in USB CDC Specification 1.1 */

/* CDC Bulk IN transfer states */
#define CDC_TX_READY                0
#define CDC_TX_BUSY                 1
#define CDC_TX_BUSY_ZLP             2       // ZLP: Zero Length Packet
#define CDC_TX_COMPLETING           3

#if defined(USB_CDC_SET_LINE_CODING_HANDLER) 
    #define LINE_CODING_TARGET &cdc_notice.SetLineCoding._byte[0]
    #define LINE_CODING_PFUNC &USB_CDC_SET_LINE_CODING_HANDLER
#else
    #define LINE_CODING_TARGET &line_coding._byte[0]
    #define LINE_CODING_PFUNC NULL
#endif

#if defined(USB_CDC_SUPPORT_HARDWARE_FLOW_CONTROL)
    #define CONFIGURE_RTS(a) UART_RTS = a;
#else
    #define CONFIGURE_RTS(a)
#endif

#if defined(USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D3)
    #error This option is not currently supported.
#else
    #define USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D3_VAL 0x00
#endif

#if defined(USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D2)
    #define USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D2_VAL 0x04
#else
    #define USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D2_VAL 0x00
#endif

#if defined(USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D1)
    #define USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D1_VAL 0x02
#else
    #define USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D1_VAL 0x00
#endif

#if defined(USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D0)
    #error This option is not currently supported.
#else
    #define USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D0_VAL 0x00
#endif

#define USB_CDC_ACM_FN_DSC_VAL  \
    USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D3_VAL |\
    USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D2_VAL |\
    USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D1_VAL |\
    USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D0_VAL

/******************************************************************************
    Function:
        void CDCSetBaudRate(DWORD baudRate)
        
    Summary:
        This macro is used set the baud rate reported back to the host during
        a get line coding request. (optional)

    Description:
        This macro is used set the baud rate reported back to the host during
        a get line coding request.

        Typical Usage:
        <code>
            CDCSetBaudRate(19200);
        </code>

        This function is optional for CDC devices that do not actually convert
        the USB traffic to a hardware UART.
        
    PreCondition:
        None
        
    Parameters:
        DWORD baudRate - The desired baudrate
        
    Return Values:
        None
        
    Remarks:
        None
  
 *****************************************************************************/
#define CDCSetBaudRate(baudRate) {line_coding.dwDTERate.Val=baudRate;}

/******************************************************************************
    Function:
        void CDCSetCharacterFormat(BYTE charFormat)
        
    Summary:
        This macro is used manually set the character format reported back to 
        the host during a get line coding request. (optional)

    Description:
        This macro is used manually set the character format reported back to 
        the host during a get line coding request.

        Typical Usage:
        <code>
            CDCSetCharacterFormat(NUM_STOP_BITS_1);
        </code>
        
        This function is optional for CDC devices that do not actually convert
        the USB traffic to a hardware UART.

    PreCondition:
        None
        
    Parameters:
        BYTE charFormat - number of stop bits.  Available options are:
         * NUM_STOP_BITS_1 - 1 Stop bit
         * NUM_STOP_BITS_1_5 - 1.5 Stop bits
         * NUM_STOP_BITS_2 - 2 Stop bits
        
    Return Values:
        None
        
    Remarks:
        None
  
 *****************************************************************************/
#define CDCSetCharacterFormat(charFormat) {line_coding.bCharFormat=charFormat;}
#define NUM_STOP_BITS_1     0   //1 stop bit - used by CDCSetLineCoding() and CDCSetCharacterFormat()
#define NUM_STOP_BITS_1_5   1   //1.5 stop bit - used by CDCSetLineCoding() and CDCSetCharacterFormat()
#define NUM_STOP_BITS_2     2   //2 stop bit - used by CDCSetLineCoding() and CDCSetCharacterFormat()

/******************************************************************************
    Function:
        void CDCSetParity(BYTE parityType)
        
    Summary:
        This function is used manually set the parity format reported back to 
        the host during a get line coding request. (optional)

    Description:
        This macro is used manually set the parity format reported back to 
        the host during a get line coding request.

        Typical Usage:
        <code>
            CDCSetParity(PARITY_NONE);
        </code>
        
        This function is optional for CDC devices that do not actually convert
        the USB traffic to a hardware UART.

    PreCondition:
        None
        
    Parameters:
        BYTE parityType - Type of parity.  The options are the following:
            * PARITY_NONE
            * PARITY_ODD
            * PARITY_EVEN
            * PARITY_MARK
            * PARITY_SPACE
        
    Return Values:
        None
        
    Remarks:
        None
  
 *****************************************************************************/
#define CDCSetParity(parityType) {line_coding.bParityType=parityType;}
#define PARITY_NONE     0 //no parity - used by CDCSetLineCoding() and CDCSetParity()
#define PARITY_ODD      1 //odd parity - used by CDCSetLineCoding() and CDCSetParity()
#define PARITY_EVEN     2 //even parity - used by CDCSetLineCoding() and CDCSetParity()
#define PARITY_MARK     3 //mark parity - used by CDCSetLineCoding() and CDCSetParity()
#define PARITY_SPACE    4 //space parity - used by CDCSetLineCoding() and CDCSetParity()

/******************************************************************************
    Function:
        void CDCSetDataSize(BYTE dataBits)
        
    Summary:
        This function is used manually set the number of data bits reported back 
        to the host during a get line coding request. (optional)

    Description:
        This function is used manually set the number of data bits reported back 
        to the host during a get line coding request.

        Typical Usage:
        <code>
            CDCSetDataSize(8);
        </code>
        
        This function is optional for CDC devices that do not actually convert
        the USB traffic to a hardware UART.

    PreCondition:
        None
        
    Parameters:
        BYTE dataBits - number of data bits.  The options are 5, 6, 7, 8, or 16.
        
    Return Values:
        None
        
    Remarks:
        None
  
 *****************************************************************************/
#define CDCSetDataSize(dataBits) {line_coding.bDataBits=dataBits;}

/******************************************************************************
    Function:
        void CDCSetLineCoding(DWORD baud, BYTE format, BYTE parity, BYTE dataSize)
        
    Summary:
        This function is used to manually set the data reported back 
        to the host during a get line coding request. (optional)

    Description:
        This function is used to manually set the data reported back 
        to the host during a get line coding request.

        Typical Usage:
        <code>
            CDCSetLineCoding(19200, NUM_STOP_BITS_1, PARITY_NONE, 8);
        </code>
        
        This function is optional for CDC devices that do not actually convert
        the USB traffic to a hardware UART.

    PreCondition:
        None
        
    Parameters:
        DWORD baud - The desired baudrate
        BYTE format - number of stop bits.  Available options are:
         * NUM_STOP_BITS_1 - 1 Stop bit
         * NUM_STOP_BITS_1_5 - 1.5 Stop bits
         * NUM_STOP_BITS_2 - 2 Stop bits
        BYTE parity - Type of parity.  The options are the following:
            * PARITY_NONE
            * PARITY_ODD
            * PARITY_EVEN
            * PARITY_MARK
            * PARITY_SPACE
        BYTE dataSize - number of data bits.  The options are 5, 6, 7, 8, or 16.
        
    Return Values:
        None
        
    Remarks:
        None
  
 *****************************************************************************/
#define CDCSetLineCoding(baud,format,parity,dataSize) {\
            CDCSetBaudRate(baud);\
            CDCSetCharacterFormat(format);\
            CDCSetParity(parity);\
            CDCSetDataSize(dataSize);\
        }

/******************************************************************************
    Function:
        BOOL USBUSARTIsTxTrfReady(void)
        
    Summary:
        This macro is used to check if the CDC class is ready
        to send more data.

    Description:
        This macro is used to check if the CDC class handler firmware is 
        ready to send more data to the host over the CDC bulk IN endpoint.

        Typical Usage:
        <code>
            if(USBUSARTIsTxTrfReady())
            {
                putrsUSBUSART("Hello World");
            }
        </code>
        
    PreCondition:
        The return value of this function is only valid if the device is in a
        configured state (i.e. - USBDeviceGetState() returns CONFIGURED_STATE)
        
    Parameters:
        None
        
    Return Values:
        Returns a boolean value indicating if the CDC class handler firmware
        is ready to receive new data to send to the host over the bulk data IN
        endpoint.  A return value of true indicates that the CDC handler 
        firmware is ready to receive new data, and it is therefore safe to
        call other APIs like putrsUSBUSART() and putsUSBUSART().  A return
        value of false implies that the firmware is still busy sending the
        last data, or is otherwise not ready to process any new data at
        this time.
        
    Remarks:
        Make sure the application periodically calls the CDCTxService()
        handler, or pending USB IN transfers will not be able to advance
        and complete.
  
 *****************************************************************************/
#define USBUSARTIsTxTrfReady()      (cdc_trf_state == CDC_TX_READY)

/******************************************************************************
    Function:
        void mUSBUSARTTxRam(BYTE *pData, BYTE len)
    
    Description:
        Depricated in MCHPFSUSB v2.3.  This macro has been replaced by 
        USBUSARTIsTxTrfReady().
 *****************************************************************************/
#define mUSBUSARTIsTxTrfReady()     USBUSARTIsTxTrfReady()

/******************************************************************************
    Function:
        void mUSBUSARTTxRam(BYTE *pData, BYTE len)
        
    Description:
        Use this macro to transfer data located in data memory.
        Use this macro when:
            1. Data stream is not null-terminated
            2. Transfer length is known
        Remember: cdc_trf_state must == CDC_TX_READY
        Unlike putsUSBUSART, there is not code double checking
        the transfer state. Unexpected behavior will occur if
        this function is called when cdc_trf_state != CDC_TX_READY
 
         Typical Usage:
        <code>
            if(USBUSARTIsTxTrfReady())
            {
                mUSBUSARTTxRam(&UserDataBuffer[0], 200);
            }
        </code>
        
    PreCondition:
        cdc_trf_state must be in the CDC_TX_READY state.
        Value of 'len' must be equal to or smaller than 255 bytes.
        The USB stack should have reached the CONFIGURED_STATE prior
        to calling this API function for the first time.
        
    Paramters:
        pDdata  : Pointer to the starting location of data bytes
        len     : Number of bytes to be transferred
        
    Return Values:
        None
        
    Remarks:
        This macro only handles the setup of the transfer. The
        actual transfer is handled by CDCTxService().  This macro
        does not "double buffer" the data.  The application firmware
        should not modify the contents of the pData buffer until all
        of the data has been sent, as indicated by the 
        USBUSARTIsTxTrfReady() function returning true, subsequent to
        calling mUSBUSARTTxRam().
        
  
 *****************************************************************************/
#define mUSBUSARTTxRam(pData,len)   \
{                                   \
    pCDCSrc.bRam = pData;           \
    cdc_tx_len = len;               \
    cdc_mem_type = USB_EP0_RAM;     \
    cdc_trf_state = CDC_TX_BUSY;    \
}

/******************************************************************************
    Function:
        void mUSBUSARTTxRom(rom BYTE *pData, BYTE len)
        
    Description:
        Use this macro to transfer data located in program memory.
        Use this macro when:
            1. Data stream is not null-terminated
            2. Transfer length is known
 
        Remember: cdc_trf_state must == CDC_TX_READY
        Unlike putrsUSBUSART, there is not code double checking
        the transfer state. Unexpected behavior will occur if
        this function is called when cdc_trf_state != CDC_TX_READY
 
          Typical Usage:
        <code>
            if(USBUSARTIsTxTrfReady())
            {
                mUSBUSARTTxRom(&SomeRomString[0], 200);
            }
        </code>
       
    PreCondition:
        cdc_trf_state must be in the CDC_TX_READY state.
        Value of 'len' must be equal to or smaller than 255 bytes.
        
    Parameters:
        pDdata  : Pointer to the starting location of data bytes
        len     : Number of bytes to be transferred
        
    Return Values:
        None
        
    Remarks:
        This macro only handles the setup of the transfer. The
        actual transfer is handled by CDCTxService().
                    
 *****************************************************************************/
#define mUSBUSARTTxRom(pData,len)   \
{                                   \
    pCDCSrc.bRom = pData;           \
    cdc_tx_len = len;               \
    cdc_mem_type = USB_EP0_ROM;     \
    cdc_trf_state = CDC_TX_BUSY;    \
}

/**************************************************************************
  Function:
        void CDCInitEP(void)
    
  Summary:
    This function initializes the CDC function driver. This function should
    be called after the SET_CONFIGURATION command (ex: within the context of
    the USBCBInitEP() function).
  Description:
    This function initializes the CDC function driver. This function sets
    the default line coding (baud rate, bit parity, number of data bits,
    and format). This function also enables the endpoints and prepares for
    the first transfer from the host.
    
    This function should be called after the SET_CONFIGURATION command.
    This is most simply done by calling this function from the
    USBCBInitEP() function.
    
    Typical Usage:
    <code>
        void USBCBInitEP(void)
        {
            CDCInitEP();
        }
    </code>
  Conditions:
    None
  Remarks:
    None                                                                   
  **************************************************************************/
void CDCInitEP(void);

/******************************************************************************
 	Function:
 		void USBCheckCDCRequest(void)
 
 	Description:
 		This routine checks the most recently received SETUP data packet to 
 		see if the request is specific to the CDC class.  If the request was
 		a CDC specific request, this function will take care of handling the
 		request and responding appropriately.
 		
 	PreCondition:
 		This function should only be called after a control transfer SETUP
 		packet has arrived from the host.

	Parameters:
		None
		
	Return Values:
		None
		
	Remarks:
		This function does not change status or do anything if the SETUP packet
		did not contain a CDC class specific request.		 
  *****************************************************************************/
void USBCheckCDCRequest(void);


/**************************************************************************
  Function: void CDCNotificationHandler(void)
  Summary: Checks for changes in DSR status and reports them to the USB host.
  Description: Checks for changes in DSR pin state and reports any changes
               to the USB host. 
  Conditions: CDCInitEP() must have been called previously, prior to calling
              CDCNotificationHandler() for the first time.
  Remarks:
    This function is only implemented and needed when the 
    USB_CDC_SUPPORT_DSR_REPORTING option has been enabled.  If the function is
    enabled, it should be called periodically to sample the DSR pin and feed
    the information to the USB host.  This can be done by calling 
    CDCNotificationHandler() by itself, or, by calling CDCTxService() which
    also calls CDCNotificationHandler() internally, when appropriate.
  **************************************************************************/
void CDCNotificationHandler(void);


/**********************************************************************************
  Function:
    BOOL USBCDCEventHandler(USB_EVENT event, void *pdata, WORD size)
    
  Summary:
    Handles events from the USB stack, which may have an effect on the CDC 
    endpoint(s).

  Description:
    Handles events from the USB stack.  This function should be called when 
    there is a USB event that needs to be processed by the CDC driver.
    
  Conditions:
    Value of input argument 'len' should be smaller than the maximum
    endpoint size responsible for receiving bulk data from USB host for CDC
    class. Input argument 'buffer' should point to a buffer area that is
    bigger or equal to the size specified by 'len'.
  Input:
    event - the type of event that occured
    pdata - pointer to the data that caused the event
    size - the size of the data that is pointed to by pdata
                                                                                   
  **********************************************************************************/
BOOL USBCDCEventHandler(USB_EVENT event, void *pdata, WORD size);


/**********************************************************************************
  Function:
        BYTE getsUSBUSART(char *buffer, BYTE len)
    
  Summary:
    getsUSBUSART copies a string of BYTEs received through USB CDC Bulk OUT
    endpoint to a user's specified location. It is a non-blocking function.
    It does not wait for data if there is no data available. Instead it
    returns '0' to notify the caller that there is no data available.

  Description:
    getsUSBUSART copies a string of BYTEs received through USB CDC Bulk OUT
    endpoint to a user's specified location. It is a non-blocking function.
    It does not wait for data if there is no data available. Instead it
    returns '0' to notify the caller that there is no data available.
    
    Typical Usage:
    <code>
        BYTE numBytes;
        BYTE buffer[64]
    
        numBytes = getsUSBUSART(buffer,sizeof(buffer)); //until the buffer is free.
        if(numBytes \> 0)
        {
            //we received numBytes bytes of data and they are copied into
            //  the "buffer" variable.  We can do something with the data
            //  here.
        }
    </code>
  Conditions:
    Value of input argument 'len' should be smaller than the maximum
    endpoint size responsible for receiving bulk data from USB host for CDC
    class. Input argument 'buffer' should point to a buffer area that is
    bigger or equal to the size specified by 'len'.
  Input:
    buffer -  Pointer to where received BYTEs are to be stored
    len -     The number of BYTEs expected.
  Output:
    BYTE -    Returns a byte indicating the total number of bytes that were actually
              received and copied into the specified buffer.  The returned value
              can be anything from 0 up to the len input value.  A return value of 0
              indicates that no new CDC bulk OUT endpoint data was available.
                                                                                   
  **********************************************************************************/
BYTE getsUSBUSART(char *buffer, BYTE len);

/******************************************************************************
  Function:
	void putUSBUSART(char *data, BYTE length)
		
  Summary:
    putUSBUSART writes an array of data to the USB. Use this version, is
    capable of transfering 0x00 (what is typically a NULL character in any of
    the string transfer functions).

  Description:
    putUSBUSART writes an array of data to the USB. Use this version, is
    capable of transfering 0x00 (what is typically a NULL character in any of
    the string transfer functions).
    
    Typical Usage:
    <code>
        if(USBUSARTIsTxTrfReady())
        {
            char data[] = {0x00, 0x01, 0x02, 0x03, 0x04};
            putUSBUSART(data,5);
        }
    </code>
    
    The transfer mechanism for device-to-host(put) is more flexible than
    host-to-device(get). It can handle a string of data larger than the
    maximum size of bulk IN endpoint. A state machine is used to transfer a
    \long string of data over multiple USB transactions. CDCTxService()
    must be called periodically to keep sending blocks of data to the host.

  Conditions:
    USBUSARTIsTxTrfReady() must return TRUE. This indicates that the last
    transfer is complete and is ready to receive a new block of data. The
    string of characters pointed to by 'data' must equal to or smaller than
    255 BYTEs.

  Input:
    char *data - pointer to a RAM array of data to be transfered to the host
    BYTE length - the number of bytes to be transfered (must be less than 255).
		
 *****************************************************************************/
void putUSBUSART(char *data, BYTE Length);

/******************************************************************************
	Function:
		void putsUSBUSART(char *data)
		
  Summary:
    putsUSBUSART writes a string of data to the USB including the null
    character. Use this version, 'puts', to transfer data from a RAM buffer.

  Description:
    putsUSBUSART writes a string of data to the USB including the null
    character. Use this version, 'puts', to transfer data from a RAM buffer.
    
    Typical Usage:
    <code>
        if(USBUSARTIsTxTrfReady())
        {
            char data[] = "Hello World";
            putsUSBUSART(data);
        }
    </code>
    
    The transfer mechanism for device-to-host(put) is more flexible than
    host-to-device(get). It can handle a string of data larger than the
    maximum size of bulk IN endpoint. A state machine is used to transfer a
    \long string of data over multiple USB transactions. CDCTxService()
    must be called periodically to keep sending blocks of data to the host.

  Conditions:
    USBUSARTIsTxTrfReady() must return TRUE. This indicates that the last
    transfer is complete and is ready to receive a new block of data. The
    string of characters pointed to by 'data' must equal to or smaller than
    255 BYTEs.

  Input:
    char *data -  null\-terminated string of constant data. If a
                            null character is not found, 255 BYTEs of data
                            will be transferred to the host.
		
 *****************************************************************************/
void putsUSBUSART(char *data);


/**************************************************************************
  Function:
        void putrsUSBUSART(const ROM char *data)
    
  Summary:
    putrsUSBUSART writes a string of data to the USB including the null
    character. Use this version, 'putrs', to transfer data literals and
    data located in program memory.

  Description:
    putrsUSBUSART writes a string of data to the USB including the null
    character. Use this version, 'putrs', to transfer data literals and
    data located in program memory.
    
    Typical Usage:
    <code>
        if(USBUSARTIsTxTrfReady())
        {
            putrsUSBUSART("Hello World");
        }
    </code>
    
    The transfer mechanism for device-to-host(put) is more flexible than
    host-to-device(get). It can handle a string of data larger than the
    maximum size of bulk IN endpoint. A state machine is used to transfer a
    \long string of data over multiple USB transactions. CDCTxService()
    must be called periodically to keep sending blocks of data to the host.

  Conditions:
    USBUSARTIsTxTrfReady() must return TRUE. This indicates that the last
    transfer is complete and is ready to receive a new block of data. The
    string of characters pointed to by 'data' must equal to or smaller than
    255 BYTEs.

  Input:
    const ROM char *data -  null\-terminated string of constant data. If a
                            null character is not found, 255 BYTEs of data
                            will be transferred to the host.
                                                                           
  **************************************************************************/
void putrsUSBUSART(const ROM char *data);

/************************************************************************
  Function:
        void CDCTxService(void)
    
  Summary:
    CDCTxService handles device-to-host transaction(s). This function
    should be called once per Main Program loop after the device reaches
    the configured state.
  Description:
    CDCTxService handles device-to-host transaction(s). This function
    should be called once per Main Program loop after the device reaches
    the configured state (after the CDCIniEP() function has already executed).
    This function is needed, in order to advance the internal software state 
    machine that takes care of sending multiple transactions worth of IN USB
    data to the host, associated with CDC serial data.  Failure to call 
    CDCTxService() perioidcally will prevent data from being sent to the
    USB host, over the CDC serial data interface.
    
    Typical Usage:
    <code>
    void main(void)
    {
        USBDeviceInit();
        while(1)
        {
            USBDeviceTasks();
            if((USBGetDeviceState() \< CONFIGURED_STATE) ||
               (USBIsDeviceSuspended() == TRUE))
            {
                //Either the device is not configured or we are suspended
                //  so we don't want to do execute any application code
                continue;   //go back to the top of the while loop
            }
            else
            {
                //Keep trying to send data to the PC as required
                CDCTxService();
    
                //Run application code.
                UserApplication();
            }
        }
    }
    </code>
  Conditions:
    CDCIniEP() function should have already exectuted/the device should be
    in the CONFIGURED_STATE.
  Remarks:
    None                                                                 
  ************************************************************************/
void CDCTxService(void);


/** S T R U C T U R E S ******************************************************/

/* Line Coding Structure */
#define LINE_CODING_LENGTH          0x07

typedef union _LINE_CODING
{
    struct
    {
        BYTE _byte[LINE_CODING_LENGTH];
    };
    struct
    {
        DWORD_VAL   dwDTERate;          // Complex data structure
        BYTE    bCharFormat;
        BYTE    bParityType;
        BYTE    bDataBits;
    };
} LINE_CODING;

typedef union _CONTROL_SIGNAL_BITMAP
{
    BYTE _byte;
    struct
    {
        unsigned DTE_PRESENT:1;       // [0] Not Present  [1] Present
        unsigned CARRIER_CONTROL:1;   // [0] Deactivate   [1] Activate
    };
} CONTROL_SIGNAL_BITMAP;


/* Functional Descriptor Structure - See CDC Specification 1.1 for details */

/* Header Functional Descriptor */
typedef struct __attribute__((packed)) _USB_CDC_HEADER_FN_DSC
{
    BYTE bFNLength;
    BYTE bDscType;
    BYTE bDscSubType;
    WORD bcdCDC;
} USB_CDC_HEADER_FN_DSC;

/* Abstract Control Management Functional Descriptor */
typedef struct __attribute__((packed)) _USB_CDC_ACM_FN_DSC
{
    BYTE bFNLength;
    BYTE bDscType;
    BYTE bDscSubType;
    BYTE bmCapabilities;
} USB_CDC_ACM_FN_DSC;

/* Union Functional Descriptor */
typedef struct __attribute__((packed)) _USB_CDC_UNION_FN_DSC
{
    BYTE bFNLength;
    BYTE bDscType;
    BYTE bDscSubType;
    BYTE bMasterIntf;
    BYTE bSaveIntf0;
} USB_CDC_UNION_FN_DSC;

/* Call Management Functional Descriptor */
typedef struct __attribute__((packed)) _USB_CDC_CALL_MGT_FN_DSC
{
    BYTE bFNLength;
    BYTE bDscType;
    BYTE bDscSubType;
    BYTE bmCapabilities;
    BYTE bDataInterface;
} USB_CDC_CALL_MGT_FN_DSC;

typedef union __attribute__((packed)) _CDC_NOTICE
{
    LINE_CODING GetLineCoding;
    LINE_CODING SetLineCoding;
    unsigned char packet[CDC_COMM_IN_EP_SIZE];
} CDC_NOTICE, *PCDC_NOTICE;

/* Bit structure definition for the SerialState notification byte */
typedef union
{
    BYTE byte;
    struct
    {
        BYTE    DCD             :1;
        BYTE    DSR             :1;
        BYTE    BreakState      :1;
        BYTE    RingDetect      :1;
        BYTE    FramingError    :1;
        BYTE    ParityError     :1;
        BYTE    Overrun         :1;
        BYTE    Reserved        :1;          
    }bits;    
}BM_SERIAL_STATE;  

/* Serial State Notification Packet Structure */
typedef struct
{
    BYTE    bmRequestType;  //Always 0xA1 for serial state notification packets
    BYTE    bNotification;  //Always SERIAL_STATE (0x20) for serial state notification packets
    UINT16  wValue;     //Always 0 for serial state notification packets
    UINT16  wIndex;     //Interface number
    UINT16  wLength;    //Should always be 2 for serial state notification packets
    BM_SERIAL_STATE SerialState;
    BYTE    Reserved;
}SERIAL_STATE_NOTIFICATION;   


/** E X T E R N S ************************************************************/
extern BYTE cdc_rx_len;
extern USB_HANDLE lastTransmission;

extern BYTE cdc_trf_state;
extern POINTER pCDCSrc;
extern BYTE cdc_tx_len;
extern BYTE cdc_mem_type;

extern volatile FAR CDC_NOTICE cdc_notice;
extern LINE_CODING line_coding;

extern volatile CTRL_TRF_SETUP SetupPkt;
extern ROM BYTE configDescriptor1[];

/** Public Prototypes *************************************************/
//------------------------------------------------------------------------------
//This is the list of public API functions provided by usb_function_cdc.c.
//This list is commented out, since the actual prototypes are declared above
//with associated inline documentation.
//------------------------------------------------------------------------------
//void USBCheckCDCRequest(void);
//void CDCInitEP(void);
//BOOL USBCDCEventHandler(USB_EVENT event, void *pdata, WORD size);
//BYTE getsUSBUSART(char *buffer, BYTE len);
//void putUSBUSART(char *data, BYTE Length);
//void putsUSBUSART(char *data);
//void putrsUSBUSART(const ROM char *data);
//void CDCTxService(void);
//void CDCNotificationHandler(void);
//------------------------------------------------------------------------------



#endif //CDC_H
