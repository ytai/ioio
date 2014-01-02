/******************************************************************************
 *
 *               Microchip Memory Disk Drive File System
 *
 ******************************************************************************
 * FileName:        CF-PMP.h
 * Dependencies:    GenericTypeDefs.h
 *					FSDefs.h
 *                  FSconfig.h
 * Processor:       PIC24/dsPIC30/dsPIC33
 * Compiler:        C30
 * Company:         Microchip Technology, Inc.
 * Version:         1.2.4
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *****************************************************************************/

#include "GenericTypeDefs.h"
#include "FSconfig.h"
#include "FSDefs.h"


/*******************************************************************************/
/*                              Macros                                         */
/*******************************************************************************/

// Description: A macro to set the CF data bus TRIS register to outputs
#define MDD_CFPMP_DATABoutput	MDD_CFPMP_DATADIR = 0;
// Description: A macro to set the CF data bus TRIS register to inputs
#define MDD_CFPMP_DATABinput	    MDD_CFPMP_DATADIR = 0xff;


/*******************************************************************************/
/*                       Structure and defines                                 */
/*******************************************************************************/

// Description: A macro for the data register offset for CF cards
#define R_DATA      0
// Description: A macro for the error register offset for CF cards
#define R_ERROR     1
// Description: A macro for the count register offset for CF cards
#define R_COUNT     2
// Description: A macro for the sector register offset for CF cards
#define R_SECT      3
// Description: A macro for the cylinder-low register offset for CF cards
#define R_CYLO      4
// Description: A macro for the cylinder-high register offset for CF cards
#define R_CYHI      5
// Description: A macro for the drive register offset for CF cards
#define R_DRIVE     6
// Description: A macro for the command register offset for CF cards
#define R_CMD       7
// Description: A macro for the status offset for CF cards
#define R_STATUS    7



// Description: A macro for the CF read comment
#define C_SECTOR_READ     0x20
// Description: A macro for the CF drive diagnostic command
#define C_DRIVE_DIAG      0x90
// Description: A macro for the CF drive identify command
#define C_DRIVE_IDENT     0xEC
// Description: A macro for the CF write command
#define C_SECTOR_WRITE    0x30


// Description: A macro indicating that the CF status register reports a ready condition
#define S_READY	0x58
// Description: A macro indicating that the CF status register reports an error condition
#define S_ERROR	0x51

// Description: A macro used to set TRIS register bits to output
#define OUTPUT	0
// Description: A macro used to set TRIS register bits to input
#define INPUT	1

#ifndef FALSE
    #define FALSE   0
#endif
#ifndef TRUE
    #define TRUE    !FALSE
#endif


// The initialization function for CF cards (no initialization required)

/************************************************************************/
/*                          Prototypes                                  */
/************************************************************************/

void MDD_CFPMP_InitIO( void);
BYTE MDD_CFPMP_MediaDetect( void);
BYTE MDD_CFPMP_WriteProtectState (void);
BYTE MDD_CFPMP_CFread( BYTE add);
void MDD_CFPMP_CFwrite( BYTE add, BYTE d);
void MDD_CFPMP_CFwait(void);

BYTE MDD_CFPMP_SectorRead( DWORD lda, BYTE * buf);
BYTE MDD_CFPMP_SectorWrite( DWORD lda, BYTE * buf, BYTE allowWriteToZero);
MEDIA_INFORMATION * MDD_CFPMP_MediaInitialize (void);

#ifdef __C30__
	extern BYTE ReadByte( BYTE* pBuffer, WORD index );
	extern WORD ReadWord( BYTE* pBuffer, WORD index );
	extern DWORD ReadDWord( BYTE* pBuffer, WORD index );
#endif


