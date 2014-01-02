/*******************************************************************************

    Debug header file

Summary:
    This file defines a debug printing interface.

Description:
    This file defines a debug printing interface.  The implementation is
    normally too hardware specific and thus should be implemented at the
    application level by creating instances of these functions.  The 
    DEBUG_MODE preprocessor macro can then be added to the specific stack if 
    debugging is desired for those files.

******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************
 FileName:     	debug.h
 Dependencies:	Compiler.h and GenericTypeDefs.h
 Processor:		any
 Hardware:		any
 Complier:  	any
 Company:		Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC® Microcontroller is intended and
 supplied to you, the Company’s customer, for use solely and
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
*******************************************************************/

#include <Compiler.h>
#include <GenericTypeDefs.h>

/**************************************************************************
    Function:
        void DEBUG_Initialize( void );
   
    Summary:
        Initializes the debug module.  This can be I/O pins, memory, etc.
        based on the debug implementation
        
    Description:
        Initializes the debug module.  This can be I/O pins, memory, etc.
        based on the debug implementation
        
    Precondition:
        None
        
    Parameters:
        None
     
    Return Values:
        None
        
    Remarks:
        None
                                                          
  **************************************************************************/
void DEBUG_Initialize();

#if !defined(DEBUG_MODE)
    #define DEBUG_Initialize(a)
#endif

/**************************************************************************
    Function:
        void DEBUG_PutChar(char c);
   
    Summary:
        Puts a character into the debug stream.
        
    Description:
        This function puts a single character into the debug stream.

    Precondition:
        None
        
    Parameters:
        None
     
    Return Values:
        None
        
    Remarks:
        None
                                                          
  **************************************************************************/
void DEBUG_PutChar(char c);

#if !defined(DEBUG_MODE)
    #define DEBUG_PutChar(a)
#endif

/**************************************************************************
    Function:
        void DEBUG_PutString(char* data);
   
    Summary:
        Prints a string to the debug stream.
        
    Description:
        Prints a string to the debug stream.
        
    Precondition:
        None
        
    Parameters:
        None
     
    Return Values:
        None
        
    Remarks:
        None
                                                          
  **************************************************************************/
void DEBUG_PutString(char* data);

#if !defined(DEBUG_MODE)
    #define DEBUG_PutString(a)
#endif

/**************************************************************************
    Function:
        void DEBUG_PutHexUINT8(UINT8 data);
   
    Summary:
        Puts a hexidecimal 8-bit number into the debug stream.
        
    Description:
        Puts a hexidecimal byte of data into the debug stream.  How this 
        is handled is implementation specific.  Some implementations may
        convert this to ASCII.  Others may print the byte directly to save
        memory/time.
        
    Precondition:
        None
        
    Parameters:
        None
     
    Return Values:
        None
        
    Remarks:
        None
                                                          
  **************************************************************************/
void DEBUG_PutHexUINT8(UINT8 data);

#if !defined(DEBUG_MODE)
    #define DEBUG_PutHexUINT8(a)
#endif

/**************************************************************************
    Function:
        void DEBUG_PutHexUINT16(UINT16 data);
   
    Summary:
        Puts a hexidecimal 16-bit number into the debug stream.
        
    Description:
        Puts a hexidecimal byte of data into the debug stream.  How this 
        is handled is implementation specific.  Some implementations may
        convert this to ASCII.  Others may print the byte directly to save
        memory/time.
        
    Precondition:
        None
        
    Parameters:
        None
     
    Return Values:
        None
        
    Remarks:
        None
                                                          
  **************************************************************************/
void DEBUG_PutHexUINT16(UINT16 data);

#if !defined(DEBUG_MODE)
    #define DEBUG_PutHexUINT16(a)
#endif

/**************************************************************************
    Function:
        void DEBUG_PutHexUINT32(UINT32 data);
   
    Summary:
        Puts a hexidecimal 32-bit number into the debug stream.
        
    Description:
        Puts a hexidecimal byte of data into the debug stream.  How this 
        is handled is implementation specific.  Some implementations may
        convert this to ASCII.  Others may print the byte directly to save
        memory/time.
        
    Precondition:
        None
        
    Parameters:
        None
     
    Return Values:
        None
        
    Remarks:
        None
                                                          
  **************************************************************************/
void DEBUG_PutHexUINT32(UINT32 data);

#if !defined(DEBUG_MODE)
    #define DEBUG_PutHexUINT32(a)
#endif



