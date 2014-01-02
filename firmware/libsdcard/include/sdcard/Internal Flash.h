/******************************************************************************
 *
 *               Microchip Memory Disk Drive File System
 *
 ******************************************************************************
 * FileName:        Internal Flash.h
 * Dependencies:    GenericTypeDefs.h
 *					FSconfig.h
 *					FSDefs.h
 * Processor:       PIC18/PIC24/dsPIC30/dsPIC33
 * Compiler:        C18/C30
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the �Company�) for its PICmicro� Microcontroller is intended and
 * supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
*****************************************************************************/


/*************************************************************************/
/*  Note:  This file is included as a template of a header file for      */
/*         a new physical layer. It is designed to go with               */
/*         "FS Phys Interface Template.c"                               */
/*************************************************************************/

//DOM-IGNORE-BEGIN
/********************************************************************
 Change History:
  Rev            Description
  ----           -----------------------
  1.2.4 - 1.2.6  No Change
  1.2.7			 Updated several MDD_INTERNAL_FLASH_xxxx definitions.
  				 Also removed PSV 32kB compiler warning, as this is no
				 longer a limitation in this release.
  1.3.6			 Modified "FSConfig.h" to "FSconfig.h" in '#include' directive.
********************************************************************/
//DOM-IGNORE-END


#include "GenericTypeDefs.h"
#include "FSconfig.h"
#include "FSDefs.h"

#define FALSE	0
#define TRUE	!FALSE

/****************************************************************/
/*                    YOUR CODE HERE                            */
/* Add any defines here                                         */
/****************************************************************/

// sample defines
#define data_bus 					PORTB
#define data_bus_TRIS_BITS			TRISB
#define address_bus 				PORTC
#define address_bus_TRIS_BITS		TRISC
#define READSTROBE					LATDbits.LATD4
#define READSTROBE_TRIS_BITS		TRISDbits.TRISD4
#define WRITESTROBE					LATDbits.LATD5
#define WRITESTROBE_TRIS_BITS		TRISDbits.TRISD5
#define WRITEPROTECTPIN				PORTDbits.RD6
#define WRITEPROTECTPIN_TRIS		TRISDbits.TRISD6
#define DEVICE_DETECT_PIN			PORTDbits.RD7
#define DEVICE_DETECT_TRIS			TRISDbits.TRISD7


#define INITIALIZATION_VALUE		0x55

BYTE MDD_IntFlash_MediaDetect(void);
MEDIA_INFORMATION * MDD_IntFlash_MediaInitialize(void);
BYTE MDD_IntFlash_SectorRead(DWORD sector_addr, BYTE* buffer);
BYTE MDD_IntFlash_SectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero);
WORD MDD_IntFlash_ReadSectorSize(void);
DWORD MDD_IntFlash_ReadCapacity(void);
BYTE MDD_IntFlash_WriteProtectState(void);

#if !defined(MDD_INTERNAL_FLASH_MAX_NUM_FILES_IN_ROOT)
    #define MDD_INTERNAL_FLASH_MAX_NUM_FILES_IN_ROOT 16
#endif

//Note: If only 1 FAT sector is used, assuming 12-bit (1.5 byte) FAT entry size 
//(ex: FAT12 filesystem), then the total FAT entries that can fit in a single 512 
//byte FAT sector is (512 bytes) / (1.5 bytes/entry) = 341 entries.  This allows 
//the FAT table to reference up to 341*512 = ~174kB of space.  Therfore, more 
//FAT sectors are needed if creating an MSD volume bigger than this.
#define MDD_INTERNAL_FLASH_NUM_RESERVED_SECTORS 1          
#define MDD_INTERNAL_FLASH_NUM_VBR_SECTORS 1       
#define MDD_INTERNAL_FLASH_NUM_FAT_SECTORS 1                
#define MDD_INTERNAL_FLASH_NUM_ROOT_DIRECTORY_SECTORS ((MDD_INTERNAL_FLASH_MAX_NUM_FILES_IN_ROOT+15)/16) //+15 because the compiler truncates
#define MDD_INTERNAL_FLASH_OVERHEAD_SECTORS (\
            MDD_INTERNAL_FLASH_NUM_RESERVED_SECTORS + \
            MDD_INTERNAL_FLASH_NUM_VBR_SECTORS + \
            MDD_INTERNAL_FLASH_NUM_ROOT_DIRECTORY_SECTORS + \
            MDD_INTERNAL_FLASH_NUM_FAT_SECTORS)
#define MDD_INTERNAL_FLASH_TOTAL_DISK_SIZE (\
            MDD_INTERNAL_FLASH_OVERHEAD_SECTORS + \
            MDD_INTERNAL_FLASH_DRIVE_CAPACITY)
#define MDD_INTERNAL_FLASH_PARTITION_SIZE (DWORD)(MDD_INTERNAL_FLASH_TOTAL_DISK_SIZE - 1)  //-1 is to exclude the sector used for the MBR 


//---------------------------------------------------------
//Do some build time error checking
//---------------------------------------------------------
#if defined(__C30__)
    #if(MDD_INTERNAL_FLASH_TOTAL_DISK_SIZE % 2)
        #warning "MSD volume overlaps flash erase page with firmware program memory.  Please change your FSconfig.h settings to ensure the MSD volume cannot share an erase page with the firmware."
        //See code comments in FSconfig.h, and adjust the MDD_INTERNAL_FLASH_DRIVE_CAPACITY definition until the warning goes away.
    #endif
#endif

#if (MDD_INTERNAL_FLASH_MAX_NUM_FILES_IN_ROOT>64)
    #if defined(__C30__)
        #error "PSV only allows 32KB of memory.  The drive options selected result in more than 32KB of data.  Please reduce the number of root directory entries possible"
    #endif
#endif

#if (MEDIA_SECTOR_SIZE != 512)
    #error "The current implementation of internal flash MDD only supports a media sector size of 512.  Please modify your selected value in the FSconfig.h file."
#endif

#if (MDD_INTERNAL_FLASH_MAX_NUM_FILES_IN_ROOT != 16) && \
    (MDD_INTERNAL_FLASH_MAX_NUM_FILES_IN_ROOT != 32) && \
    (MDD_INTERNAL_FLASH_MAX_NUM_FILES_IN_ROOT != 48) && \
    (MDD_INTERNAL_FLASH_MAX_NUM_FILES_IN_ROOT != 64)
    #error "Number of root file entries must be a multiple of 16.  Please adjust the definition in the FSconfig.h file."
#endif

