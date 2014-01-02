/******************************************************************************
 *
 *                Microchip Memory Disk Drive File System
 *
 ******************************************************************************
 * FileName:        FSconfig.h
 * Processor:       PIC18/PIC24/dsPIC30/dsPIC33/PIC32
 * Dependencies:    None
 * Compiler:        C18/C30/C32
 * Company:         Microchip Technology, Inc.
 * Version:         1.3.0
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


#ifndef _FS_DEF_

#include "Compiler.h"
#include "HardwareProfile.h"

// Summary: A macro indicating whether Long File Name is supported
// Description: If this macro is disabled then only 8.3 format file name is enabled.
//              If this macro is enabled then long file names upto 256 characters are
//              supported.
#define	SUPPORT_LFN

// Summary: A macro indicating the maximum number of concurrently open files
// Description: The FS_MAX_FILES_OPEN #define is only applicable when dynamic memory allocation is not used (FS_DYNAMIC_MEM is not defined).
//              This macro defines the maximum number of open files at any given time.  The amount of RAM used by FSFILE objects will
//              be equal to the size of an FSFILE object multipled by this macro value.  This value should be kept as small as possible
//              as dictated by the application.  This will reduce memory usage.
#define FS_MAX_FILES_OPEN 	2


// Summary: A macro defining the size of a sector
// Description: The MEDIA_SECTOR_SIZE macro will define the size of a sector on the FAT file system.  This value must equal 512 bytes,
//              1024 bytes, 2048 bytes, or 4096 bytes.  The value of a sector will usually be 512 bytes.
#define MEDIA_SECTOR_SIZE 		512



/* *******************************************************************************************************/
/************** Compiler options to enable/Disable Features based on user's application ******************/
/* *******************************************************************************************************/


// Summary: A macro to enable/disable file search functions.
// Description: The ALLOW_FILESEARCH definition can be commented out to disable file search functions in the library.  This will
//              prevent the use of the FindFirst and FindNext functions and reduce code size.
#define ALLOW_FILESEARCH

// Summary: A macro to enable/disable write functionality
// Description: The ALLOW_WRITES definition can be commented out to disable all operations that write to the device.  This will
//              greatly reduce code size.
#define ALLOW_WRITES


// Summary: A macro to enable/disable format functionality
// Description: The ALLOW_FORMATS definition can be commented out to disable formatting functionality.  This will prevent the use of
//              the FSformat function.  If formats are enabled, write operations must also be enabled by uncommenting ALLOW_WRITES.
#define ALLOW_FORMATS

// Summary: A macro to enable/disable directory operations.
// Description: The ALLOW_DIRS definition can be commented out to disable all directory functionality.  This will reduce code size.
//              If directories are enabled, write operations must also be enabled by uncommenting ALLOW_WRITES in order to use
//              the FSmkdir or FSrmdir functions.
#define ALLOW_DIRS

// Summary: A macro to enable/disable PIC18 ROM functions.
// Description: The ALLOW_PGMFUNCTIONS definition can be commented out to disable all PIC18 functions that allow the user to pass string
//              arguments in ROM (denoted by the suffix -pgm).  Note that this functionality must be disabled when not using PIC18.
//#define ALLOW_PGMFUNCTIONS

// Summary: A macro to enable/disable the FSfprintf function.
// Description: The ALLOW_FSFPRINTF definition can be commented out to disable the FSfprintf function.  This will save code space.  Note that
//              if FSfprintf is enabled and the PIC18 architecture is used, integer promotions must be enabled in the Project->Build Options
//              menu.  Write operations must be enabled to use FSfprintf.
//#define ALLOW_FSFPRINTF

// Summary: A macro to enable/disable the FSGetDiskProperties function.
// Description: The ALLOW_GET_DISK_PROPERTIES definition can be commented out to disable the FSGetDiskProperties function in the library.
//              This will save code space.
//#define ALLOW_GET_DISK_PROPERTIES

// Summary: A macro to enable/disable FAT32 support.
// Description: The SUPPORT_FAT32 definition can be commented out to disable support for FAT32 functionality.  This will save a small amount
//              of code space.
#define SUPPORT_FAT32



/**************************************************************************************************/
// Select a method for updating file timestamps
/**************************************************************************************************/

// Summary: A macro to enable RTCC based timestamp generation
// Description: The USEREALTIMECLOCK macro will configure the code to automatically
//              generate timestamp information for files from the RTCC module. The user
//              must enable and configure the RTCC module before creating or modifying
//              files.                                                                 
#define USEREALTIMECLOCK

// Summary: A macro to enable manual timestamp generation
// Description: The USERDEFINEDCLOCK macro will allow the user to manually set
//              timestamp information using the SetClockVars function. The user will
//              need to set the time variables immediately before creating or closing a
//              file or directory.                                                    
//#define USERDEFINEDCLOCK

// Summary: A macro to enable don't-care timestamp generation
// Description: The INCREMENTTIMESTAMP macro will set the create time of a file to a
//              static value and increment it when a file is updated. This timestamp
//              generation method should only be used in applications where file times
//              are not necessary.                                                    
//#define INCREMENTTIMESTAMP


#ifdef __18CXX
	#ifdef USEREALTIMECLOCK
		#error The PIC18 architecture does not have a Real-time clock and calander module
	#endif
#endif

#ifdef ALLOW_PGMFUNCTIONS
	#ifndef __18CXX
		#error The pgm functions are unneccessary when not using PIC18
	#endif
#endif

#ifndef USEREALTIMECLOCK
    #ifndef USERDEFINEDCLOCK
        #ifndef INCREMENTTIMESTAMP
            #error Please enable USEREALTIMECLOCK, USERDEFINEDCLOCK, or INCREMENTTIMESTAMP
        #endif
    #endif
#endif

/************************************************************************/
// Set this preprocessor option to '1' to use dynamic FSFILE object allocation.  It will
// be necessary to allocate a heap when dynamically allocating FSFILE objects.
// Set this option to '0' to use static FSFILE object allocation.
/************************************************************************/

#if 0
    // Summary: A macro indicating that FSFILE objects will be allocated dynamically
    // Description: The FS_DYNAMIC_MEM macro will cause FSFILE objects to be allocated from a dynamic heap.  If it is undefined,
    //              the file objects will be allocated using a static array.
	#define FS_DYNAMIC_MEM
	#ifdef __18CXX
        // Description: Function pointer to a dynamic memory allocation function
		#define FS_malloc	SRAMalloc
        // Description: Function pointer to a dynamic memory free function
		#define FS_free		SRAMfree
	#else
		#define FS_malloc	malloc
		#define FS_free		free
	#endif
#endif

// Function definitions
// Associate the physical layer functions with the correct physical layer
#ifdef USE_SD_INTERFACE_WITH_SPI       // SD-SPI.c and .h

    // Description: Function pointer to the Media Initialize Physical Layer function
    #define MDD_MediaInitialize     MDD_SDSPI_MediaInitialize

    // Description: Function pointer to the Media Detect Physical Layer function
    #define MDD_MediaDetect         MDD_SDSPI_MediaDetect

    // Description: Function pointer to the Sector Read Physical Layer function
    #define MDD_SectorRead          MDD_SDSPI_SectorRead

    // Description: Function pointer to the Sector Write Physical Layer function
    #define MDD_SectorWrite         MDD_SDSPI_SectorWrite

    // Description: Function pointer to the I/O Initialization Physical Layer function
    #define MDD_InitIO              MDD_SDSPI_InitIO

    // Description: Function pointer to the Media Shutdown Physical Layer function
    #define MDD_ShutdownMedia       MDD_SDSPI_ShutdownMedia

    // Description: Function pointer to the Write Protect Check Physical Layer function
    #define MDD_WriteProtectState   MDD_SDSPI_WriteProtectState

    // Description: Function pointer to the Read Capacity Physical Layer function
    #define MDD_ReadCapacity        MDD_SDSPI_ReadCapacity

    // Description: Function pointer to the Read Sector Size Physical Layer Function
    #define MDD_ReadSectorSize      MDD_SDSPI_ReadSectorSize

#elif defined USE_CF_INTERFACE_WITH_PMP       // CF-PMP.c and .h

    // Description: Function pointer to the Media Initialize Physical Layer function
    #define MDD_MediaInitialize     MDD_CFPMP_MediaInitialize

    // Description: Function pointer to the Media Detect Physical Layer function
    #define MDD_MediaDetect         MDD_CFPMP_MediaDetect

    // Description: Function pointer to the Sector Read Physical Layer function
    #define MDD_SectorRead          MDD_CFPMP_SectorRead

    // Description: Function pointer to the Sector Write Physical Layer function
    #define MDD_SectorWrite         MDD_CFPMP_SectorWrite

    // Description: Function pointer to the I/O Initialization Physical Layer function
    #define MDD_InitIO              MDD_CFPMP_InitIO

    // Description: Function pointer to the Media Shutdown Physical Layer function
    #define MDD_ShutdownMedia       MDD_CFPMP_ShutdownMedia

    // Description: Function pointer to the Write Protect Check Physical Layer function
    #define MDD_WriteProtectState   MDD_CFPMP_WriteProtectState

    // Description: Function pointer to the CompactFlash Wait Physical Layer function
    #define MDD_CFwait              MDD_CFPMP_CFwait

    // Description: Function pointer to the CompactFlash Write Physical Layer function
    #define MDD_CFwrite             MDD_CFPMP_CFwrite

    // Description: Function pointer to the CompactFlash Read Physical Layer function
    #define MDD_CFread              MDD_CFPMP_CFread

#elif defined USE_MANUAL_CF_INTERFACE         // CF-Bit transaction.c and .h

    // Description: Function pointer to the Media Initialize Physical Layer function
    #define MDD_MediaInitialize     MDD_CFBT_MediaInitialize

    // Description: Function pointer to the Media Detect Physical Layer function
    #define MDD_MediaDetect         MDD_CFBT_MediaDetect

    // Description: Function pointer to the Sector Read Physical Layer function
    #define MDD_SectorRead          MDD_CFBT_SectorRead

    // Description: Function pointer to the Sector Write Physical Layer function
    #define MDD_SectorWrite         MDD_CFBT_SectorWrite

    // Description: Function pointer to the I/O Initialization Physical Layer function
    #define MDD_InitIO              MDD_CFBT_InitIO

    // Description: Function pointer to the Media Shutdown Physical Layer function
    #define MDD_ShutdownMedia       MDD_CFBT_ShutdownMedia

    // Description: Function pointer to the Write Protect Check Physical Layer function
    #define MDD_WriteProtectState   MDD_CFBT_WriteProtectState

    // Description: Function pointer to the CompactFlash Wait Physical Layer function
    #define MDD_CFwait              MDD_CFBT_CFwait

    // Description: Function pointer to the CompactFlash Write Physical Layer function
    #define MDD_CFwrite             MDD_CFBT_CFwrite

    // Description: Function pointer to the CompactFlash Read Physical Layer function
    #define MDD_CFread              MDD_CFBT_CFread

#elif defined USE_USB_INTERFACE               // USB host MSD library

    // Description: Function pointer to the Media Initialize Physical Layer function
    #define MDD_MediaInitialize     USBHostMSDSCSIMediaInitialize

    // Description: Function pointer to the Media Detect Physical Layer function
    #define MDD_MediaDetect         USBHostMSDSCSIMediaDetect

    // Description: Function pointer to the Sector Read Physical Layer function
    #define MDD_SectorRead          USBHostMSDSCSISectorRead

    // Description: Function pointer to the Sector Write Physical Layer function
    #define MDD_SectorWrite         USBHostMSDSCSISectorWrite

    // Description: Function pointer to the I/O Initialization Physical Layer function
    #define MDD_InitIO();              

    // Description: Function pointer to the Media Shutdown Physical Layer function
    #define MDD_ShutdownMedia       USBHostMSDSCSIMediaReset

    // Description: Function pointer to the Write Protect Check Physical Layer function
    #define MDD_WriteProtectState   USBHostMSDSCSIWriteProtectState

#endif

#endif
