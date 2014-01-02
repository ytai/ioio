/******************************************************************************
 *
 *                Microchip Memory Disk Drive File System
 *
 ******************************************************************************
 * FileName:        FSIO.h
 * Dependencies:    GenericTypeDefs.h
 *                  FSconfig.h
 *                  FSDefs.h
 *                  stddef.h
 * Processor:       PIC18/PIC24/dsPIC30/dsPIC33/PIC32
 * Compiler:        C18/C30/C32
 * Company:         Microchip Technology, Inc.
 * Version:         1.4.0
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

#ifndef  FS_DOT_H
#define  FS_DOT_H

#include "GenericTypeDefs.h"
#include "FSconfig.h"
#include "FSDefs.h" 
#include <stddef.h>

#ifdef USE_SD_INTERFACE_WITH_SPI
#include    "SD-SPI.h"
#endif
#ifdef USE_CF_INTERFACE_WITH_PMP
    #include    "MDD File System/CF-PMP.h"
#endif
#ifdef USE_MANUAL_CF_INTERFACE
    #include    "MDD File System/CF- Bit transaction.h"
#endif
#ifdef USE_USB_INTERFACE
    #include    "USB/usb_host_msd_scsi.h"
#endif
#ifdef USE_INTERNAL_FLASH
    #include    "MDD File System/Internal Flash.h"
#endif


/*******************************************************************/
/*                     Strunctures and defines                     */
/*******************************************************************/

#ifndef FALSE
    // Summary: False value
    // Description:  This macro will indicate that a condition is false.
    #define FALSE   0
#endif
#ifndef TRUE
    // Summary: True value
    // Description: This macro will indicate that a condition is true.
    #define TRUE    !FALSE  // True value
#endif




#ifndef SEEK_SET
    // Summary: Macro for the FSfseek SEEK_SET base location.
    // Description: Functions as an input for FSfseek that specifies that the position in the file will be changed 
    //              relative to the beginning of the file.
    #define SEEK_SET 0

#endif
#ifndef SEEK_CUR

    // Summary: Macro for the FSfseek SEEK_CUR base location.
    // Description: Functions as an input for FSfseek that specifies that the position in the file will be changed
    //              relative to the current location of the file
    #define SEEK_CUR 1

#endif
#ifndef SEEK_END

    // Summary: Macro for the FSfseek SEEK_END base location
    // Description: Functions as an input for FSfseek that specifies that the position in the file will be changed
    //              relative to the end of the file.  For this macro, the offset value will be subtracted from
    //              the end location of the file by default.
    #define SEEK_END 2

#endif

// Summary: Macro for the FSfopen FS_APPEND mode
// Description: If this macro is specified as the mode argument in a call of FSfopen, the file being opened will
//              be created if it doesn't exist.  If it does exist, it's file information will be loaded and the
//              current location in the file will be set to the end.  The user will then be able to write to the file.
#define FS_APPEND   "a"

// Summary: Macro for the FSfopen FS_WRITE mode
// Description: If this macro is specified as the mode argument in a call of FSfopen, the file being opened will
//              be created if it doesn't exist.  If it does exist, it will be erased and replaced by an empty file
//              of the same name.  The user will then be able to write to the file.
#define FS_WRITE    "w"

// Summary: Macro for the FSfopen FS_READ mode
// Description: If this macro is specified as the mode argument in a call of FSfopen, the file information for the 
//              specified file will be loaded.  If the file does not exist, the FSfopen function will fail.  The user 
//              will then be able to read from the file.
#define FS_READ "r"

// Summary: Macro for the FSfopen FS_APPEND+ mode
// Description: If this macro is specified as the mode argument in a call of FSfopen, the file being opened will
//              be created if it doesn't exist.  If it does exist, it's file information will be loaded and the
//              current location in the file will be set to the end.  The user will then be able to write to the file
//              or read from the file.
#define FS_APPENDPLUS   "a+"

// Summary: Macro for the FSfopen FS_WRITE+ mode
// Description: If this macro is specified as the mode argument in a call of FSfopen, the file being opened will
//              be created if it doesn't exist.  If it does exist, it will be erased and replaced by an empty file
//              of the same name.  The user will then be able to write to the file or read from the file.
#define FS_WRITEPLUS    "w+"

// Summary: Macro for the FSfopen FS_READ+ mode
// Description: If this macro is specified as the mode argument in a call of FSfopen, the file information for the 
//              specified file will be loaded.  If the file does not exist, the FSfopen function will fail.  The user 
//              will then be able to read from the file or write to the file.
#define FS_READPLUS     "r+"

#ifndef intmax_t
    #ifdef __PIC24F__
        // Summary: A data type indicating the maximum integer size in an architecture
        // Description: The intmax_t data type refers to the maximum-sized data type on any given architecture.  This
        //              data type can be specified as a format specifier size specification for the FSfprintf function.
        #define intmax_t long long
    #elif defined __PIC24H__
        #define intmax_t long long
    #elif defined __dsPIC30F__
        #define intmax_t long long
    #elif defined __dsPIC33F__
        #define intmax_t long long
    #endif
#endif



// Summary:  Indicates flag conditions for a file object
// Description: The FILEFLAGS structure is used to indicate conditions in a file.  It contains three flags: 'write' indicates
//              that the file was opened in a mode that allows writes, 'read' indicates that the file was opened in a mode
//              that allows reads, and 'FileWriteEOF' indicates that additional data that is written to the file will increase
//              the file size.
typedef struct
{
    unsigned    write :1;           // Indicates a file was opened in a mode that allows writes
    unsigned    read :1;            // Indicates a file was opened in a mode that allows reads
    unsigned    FileWriteEOF :1;    // Indicates the current position in a file is at the end of the file
}FILEFLAGS;



// Summary: Indicates how to search for file entries in the FILEfind function
// Description: The values in the SEARCH_TYPE enumeration are used internally by the library to indicate how the FILEfind function
//              how to perform a search.  The 'LOOK_FOR_EMPTY_ENTRY' value indicates that FILEfind should search for an empty file entry.
//              The 'LOOK_FOR_MATCHING_ENTRY' value indicates that FILEfind should search for an entry that matches the FSFILE object
//              that was passed into the FILEfind function.
typedef enum{
    LOOK_FOR_EMPTY_ENTRY = 0,
    LOOK_FOR_MATCHING_ENTRY
} SEARCH_TYPE;



// Summary: Macro indicating the length of a 8.3 file name
// Description: The TOTAL_FILE_SIZE_8P3 macro indicates the maximum number of characters in an 8.3 file name.  This value includes
//              8 characters for the name, three for the extentsion, and 1 for the radix ('.')
#define TOTAL_FILE_SIZE_8P3             (8+3+1)
#define TOTAL_FILE_SIZE                 TOTAL_FILE_SIZE_8P3

// Summary: Macro indicating the max length of a LFN file name
// Description: The MAX_FILE_NAME_LENGTH_LFN macro indicates the maximum number of characters in an LFN file name.
#define MAX_FILE_NAME_LENGTH_LFN             256

// Summary: A mask that indicates the limit of directory entries in a sector
// Description: The MASK_MAX_FILE_ENTRY_LIMIT_BITS is used to indicate to the Cache_File_Entry function that a new sector needs to
//              be loaded.
#define MASK_MAX_FILE_ENTRY_LIMIT_BITS          0x0f

// Summary: Value used for shift operations to calculate the sector offset in a directory
// Description: The VALUE_BASED_ON_ENTRIES_PER_CLUSTER macro is used to calculate sector offsets for directories.  The position of the
//              entry is shifted by 4 bits (divided by 16, since there are 16 entries in a sector) to calculate how many sectors a
//              specified entry is offset from the beginning of the directory.
#define VALUE_BASED_ON_ENTRIES_PER_CLUSTER      4

// Summary: A value that will indicate that a dotdot directory entry points to the root.
// Description: The VALUE_DOTDOT_CLUSTER_VALUE_FOR_ROOT macro is used as an absolute address when writing information to a dotdot entry
//              in a newly created directory.  If a dotdot entry points to the root directory, it's cluster value must be set to 0,
//              regardless of the actual cluster number of the root directory.
#define VALUE_DOTDOT_CLUSTER_VALUE_FOR_ROOT     0

// Summary: MAcro indicating the length of an 8.3 file name in a directory entry
// Description: The FILE_NAME_SIZE_8P3 macro indicates the number of characters that an 8.3 file name will take up when packed in
//              a directory entry.  This value includes 8 characters for the name and 3 for the extension.  Note that the radix is not
//              stored in the directory entry.
#define FILE_NAME_SIZE_8P3           11
#define FILE_NAME_SIZE               FILE_NAME_SIZE_8P3


// Summary: Contains file information and is used to indicate which file to access.
// Description: The FSFILE structure is used to hold file information for an open file as it's being modified or accessed.  A pointer to 
//              an open file's FSFILE structure will be passeed to any library function that will modify that file.
typedef struct
{
    DISK    *       dsk;            // Pointer to a DISK structure
    DWORD           cluster;        // The first cluster of the file
    DWORD           ccls;           // The current cluster of the file
    WORD            sec;            // The current sector in the current cluster of the file
    WORD            pos;            // The position in the current sector
    DWORD           seek;           // The absolute position in the file
    DWORD           size;           // The size of the file
    FILEFLAGS       flags;          // A structure containing file flags
    WORD            time;           // The file's last update time
    WORD            date;           // The file's last update date
    char            name[FILE_NAME_SIZE_8P3];       // The short name of the file
	#ifdef SUPPORT_LFN
    	BOOL			AsciiEncodingType;          // Ascii file name or Non-Ascii file name indicator
		unsigned short int *utf16LFNptr;	        // Pointer to long file name in UTF16 format
		unsigned short int utf16LFNlength;          // LFN length in terms of words excluding the NULL word at the last.
	#endif
    WORD            entry;          // The position of the file's directory entry in it's directory
    WORD            chk;            // File structure checksum
    WORD            attributes;     // The file attributes
    DWORD           dirclus;        // The base cluster of the file's directory
    DWORD           dirccls;        // The current cluster of the file's directory
} FSFILE;

/* Summary: Possible results of the FSGetDiskProperties() function.
** Description: See the FSGetDiskProperties() function for more information.
*/
typedef enum
{
    FS_GET_PROPERTIES_NO_ERRORS = 0,
    FS_GET_PROPERTIES_DISK_NOT_MOUNTED,
    FS_GET_PROPERTIES_CLUSTER_FAILURE,
    FS_GET_PROPERTIES_STILL_WORKING = 0xFF
} FS_DISK_ERRORS;


/* Summary: Contains the disk search information, intermediate values, and results
** Description: This structure is used in conjunction with the FSGetDiskProperties()
**              function.  See that function for more information about the usage.
*/
typedef struct
{
    DISK *  disk;           /* pointer to the disk we are searching */
    BOOL    new_request;    /* is this a new request or a continued request */
    FS_DISK_ERRORS properties_status;  /* status of the last call of the function */

    struct
    {
        BYTE disk_format;           /* disk format: FAT12, FAT16, FAT32 */
        WORD sector_size;           /* sector size of the drive */
        BYTE sectors_per_cluster;   /* number of sectors per cluster */
        DWORD total_clusters;       /* the number of total clusters on the drive */
        DWORD free_clusters;        /* the number of free (unused) clusters on drive */
    } results;                      /* the results of the current search */

    struct
    {
        DWORD   c;     
        DWORD   curcls;
        DWORD   EndClusterLimit;
        DWORD   ClusterFailValue;
    } private;      /* intermediate values used to continue searches.  This
                         member should be used only by the FSGetDiskProperties()
                         function */

} FS_DISK_PROPERTIES;

// Summary: A structure used for searching for files on a device.
// Description: The SearchRec structure is used when searching for file on a device.  It contains parameters that will be loaded with
//              file information when a file is found.  It also contains the parameters that the user searched for, allowing further
//              searches to be perfomed in the same directory for additional files that meet the specified criteria.
typedef struct
{
    char            filename[FILE_NAME_SIZE_8P3 + 2];   // The name of the file that has been found
    unsigned char   attributes;                     // The attributes of the file that has been found
    unsigned long   filesize;                       // The size of the file that has been found
    unsigned long   timestamp;                      // The last modified time of the file that has been found (create time for directories)
	#ifdef SUPPORT_LFN
		BOOL			AsciiEncodingType;          // Ascii file name or Non-Ascii file name indicator
		unsigned short int *utf16LFNfound;		    // Pointer to long file name found in UTF16 format
		unsigned short int utf16LFNfoundLength;     // LFN Found length in terms of words including the NULL word at the last.
	#endif
    unsigned int    entry;                          // The directory entry of the last file found that matches the specified attributes. (Internal use only)
    char            searchname[FILE_NAME_SIZE_8P3 + 2]; // The 8.3 format name specified when the user began the search. (Internal use only)
    unsigned char   searchattr;                     // The attributes specified when the user began the search. (Internal use only)
    unsigned long   cwdclus;                        // The directory that this search was performed in. (Internal use only)
    unsigned char   initialized;                    // Check to determine if the structure was initialized by FindFirst (Internal use only)
} SearchRec;


/***************************************************************************
* Prototypes                                                               *
***************************************************************************/




/*************************************************************************
  Function:
    int FSInit(void)
  Summary:
    Function to initialize the device.
  Conditions:
    The physical device should be connected to the microcontroller.
  Input:
    None
  Return Values:
    TRUE -  Initialization successful
    FALSE - Initialization unsuccessful
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    This function initializes the file system stack & the interfacing device.
    Initializes the static or dynamic memory slots for holding file
    structures. Initializes the device with the DISKmount function. Loads 
    MBR and boot sector information. Initializes the current working
    directory to the root directory for the device if directory support
    is enabled.
  Remarks:
    None
  *************************************************************************/

int FSInit(void);


/*********************************************************************
  Function:
    FSFILE * FSfopen (const char * fileName, const char *mode)
  Summary:
    Opens a file with ascii input 'fileName' on PIC24/PIC32/dsPIC MCU's.
  Conditions:
    For read modes, file exists; FSInit performed
  Input:
    fileName -  The name of the file to open
    mode -
         - FS_WRITE -      Create a new file or replace an existing file
         - FS_READ -       Read data from an existing file
         - FS_APPEND -     Append data to an existing file
         - FS_WRITEPLUS -  Create a new file or replace an existing file (reads also enabled)
         - FS_READPLUS -   Read data from an existing file (writes also enabled)
         - FS_APPENDPLUS - Append data to an existing file (reads also enabled)
  Return Values:
    FSFILE * - The pointer to the file object
    NULL -     The file could not be opened
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    This function will open a ascii name file or directory on PIC24/PIC32/dsPIC MCU's.
    First, RAM in the dynamic heap or static array will be allocated to a
    new FSFILE object. Then, the specified file name will be formatted to
    ensure that it's in 8.3 format or LFN format. Next, the FILEfind function
    will be used to search for the specified file name. If the name is found,
    one of three things will happen: if the file was opened in read mode, its
    file info will be loaded using the FILEopen function; if it was opened in
    write mode, it will be erased, and a new file will be constructed in
    its place; if it was opened in append mode, its file info will be
    loaded with FILEopen and the current location will be moved to the
    end of the file using the FSfseek function.  If the file was not
    found by FILEfind, a new file will be created if the mode was specified as
    a write or append mode.  In these cases, a pointer to the heap or
    static FSFILE object array will be returned. If the file was not
    found and the mode was specified as a read mode, the memory
    allocated to the file will be freed and the NULL pointer value
    will be returned.
  Remarks:
    None.
  *********************************************************************/

FSFILE * FSfopen(const char * fileName, const char *mode);

#ifdef SUPPORT_LFN
/*********************************************************************
  Function:
    FSFILE * wFSfopen (const unsigned short int * fileName, const char *mode)
  Summary:
    Opens a file with UTF16 input 'fileName' on PIC24/PIC32/dsPIC MCU's.
  Conditions:
    For read modes, file exists; FSInit performed
  Input:
    fileName -  The name of the file to open
    mode -
         - FS_WRITE -      Create a new file or replace an existing file
         - FS_READ -       Read data from an existing file
         - FS_APPEND -     Append data to an existing file
         - FS_WRITEPLUS -  Create a new file or replace an existing file (reads also enabled)
         - FS_READPLUS -   Read data from an existing file (writes also enabled)
         - FS_APPENDPLUS - Append data to an existing file (reads also enabled)
  Return Values:
    FSFILE * - The pointer to the file object
    NULL -     The file could not be opened
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    This function opens a file with UTF16 input 'fileName' on PIC24/PIC32/dsPIC MCU's.
    First, RAM in the dynamic heap or static array will be allocated to a
    new FSFILE object. Then, the specified file name will be formatted to
    ensure that it's in 8.3 format or LFN format. Next, the FILEfind function
    will be used to search for the specified file name. If the name is found,
    one of three things will happen: if the file was opened in read mode, its
    file info will be loaded using the FILEopen function; if it was opened in
    write mode, it will be erased, and a new file will be constructed in
    its place; if it was opened in append mode, its file info will be
    loaded with FILEopen and the current location will be moved to the
    end of the file using the FSfseek function.  If the file was not
    found by FILEfind, a new file will be created if the mode was specified as
    a write or append mode.  In these cases, a pointer to the heap or
    static FSFILE object array will be returned. If the file was not
    found and the mode was specified as a read mode, the memory
    allocated to the file will be freed and the NULL pointer value
    will be returned.
  Remarks:
    None.
  *********************************************************************/

FSFILE * wFSfopen(const unsigned short int * fileName, const char *mode);
#endif

#ifdef ALLOW_PGMFUNCTIONS

/******************************************************************************
  Function:
    FSFILE * FSfopenpgm(const rom char * fileName, const rom char *mode)
  Summary:
    Opens a file on PIC18 Microcontrollers where 'fileName' ROM string is given
    in Ascii format.
  Conditions:
    For read modes, file exists; FSInit performed
  Input:
    fileName -  The name of the file to be opened (ROM)
    mode -      The mode the file will be opened in (ROM)
  Return Values:
    FSFILE * - A pointer to the file object
    NULL -     File could not be opened
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    This function opens a file on PIC18 Microcontrollers where 'fileName' ROM string
    is given in Ascii format.The FSfopenpgm function will copy a PIC18 ROM fileName and
    mode argument into RAM arrays, and then pass those arrays to the FSfopen function.
  Remarks:
    This function is for use with PIC18 when passing arguments in ROM.
  ******************************************************************************/

    FSFILE * FSfopenpgm(const rom char * fileName, const rom char *mode);


/**************************************************************************************
  Function:
    int FindFirstpgm (const char * fileName, unsigned int attr, SearchRec * rec)
  Summary:
    Find a file named with a ROM string on PIC18
  Conditions:
    None
  Input:
    fileName -  The name of the file to be found (ROM)
    attr -      The attributes of the file to be found
    rec -       Pointer to a search record to store the file info in
  Return Values:
    0 -  File was found
    -1 - No file matching the given parameters was found
  Side Effects:
    Search criteria from previous FindFirstpgm call on passed SearchRec object
    will be lost.The FSerrno variable will be changed.
  Description:
    This function finds a file named with 'fileName' on PIC18. The FindFirstpgm
    function will copy a PIC18 ROM fileName argument into a RAM array, and then
    pass that array to the FindFirst function.
  Remarks:
    Call FindFirstpgm or FindFirst before calling FindNext.
    This function is for use with PIC18 when passing arguments in ROM.
  **************************************************************************************/

    int FindFirstpgm (const rom char * fileName, unsigned int attr, SearchRec * rec);


/**************************************************************************
  Function:
    int FSchdirpgm (const rom char * path)
  Summary:
    Changes the CWD to the input path on PIC18
  Conditions:
    None
  Input:
    path - The path of the directory to change to (ROM)
  Return Values:
    0 -   The current working directory was changed successfully
    EOF - The current working directory could not be changed
  Side Effects:
    The current working directory may be changed. The FSerrno variable will
    be changed.
  Description:
    Changes the CWD to the input path on PIC18.The FSchdirpgm function
    passes a PIC18 ROM path pointer to the chdirhelper function.
  Remarks:
    This function is for use with PIC18 when passing arguments in ROM
  **************************************************************************/

    int FSchdirpgm (const rom char * path);

    #ifdef ALLOW_WRITES


/*************************************************************
  Function:
    int FSremovepgm (const rom char * fileName)
  Summary:
    Deletes the file on PIC18 device
  Conditions:
    File not opened; file exists
  Input:
    fileName -  The name of the file to be deleted (ROM)
  Return Values:
    0 -  File was removed successfully
    -1 - File could not be removed
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    Deletes the file on PIC18 device.The FSremovepgm function will copy a
    PIC18 ROM fileName argument into a RAM array, and then pass that array
    to the FSremove function.
  Remarks:
    This function is for use with PIC18 when passing arguments in ROM.
  *************************************************************/

        int FSremovepgm (const rom char * fileName);


/**************************************************************************
  Function:
    int FSmkdirpgm (const rom char * path)
  Summary:
    Creates a directory as per the path mentioned in the input string on 
    PIC18 devices.
  Conditions:
    None
  Input:
    path - The path of directories to create (ROM)
  Return Values:
    0 -   The specified directory was created successfully
    EOF - The specified directory could not be created
  Side Effects:
    Will create all non-existent directories in the path. The FSerrno 
    variable will be changed.
  Description:
    Creates a directory as per the path mentioned in the input string on 
    PIC18 devices.'FSmkdirpgm' creates the directories as per the input
    string path.This function doesn't move the current working
    directory setting.
  Remarks:
    This function is for use with PIC18 when passing arugments in ROM
  **************************************************************************/

        int FSmkdirpgm (const rom char * path);


/**************************************************************************
  Function:
    int FSrmdirpgm (const rom char * path)
  Summary:
    Deletes the directory as per the ascii input path (PIC18).
  Conditions:
    None.
  Input:
    path -      The path of the directory to remove (ROM)
    rmsubdirs - 
              - TRUE -  All sub-dirs and files in the target dir will be removed
              - FALSE - FSrmdir will not remove non-empty directories
  Return Values:
    0 -   The specified directory was deleted successfully
    EOF - The specified directory could not be deleted
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    Deletes the directory as per the ascii input path (PIC18).
    This function deletes the directory as specified in the path.
    This function wont delete the current working directory.
  Remarks:
    This function is for use with PIC18 when passing arguments in ROM.
  **************************************************************************/

        int FSrmdirpgm (const rom char * path, unsigned char rmsubdirs);


/*****************************************************************
  Function:
    int FSrenamepgm(const rom char * fileName, FSFILE * fo)
  Summary:
    Renames the file with the ascii ROM string(PIC18)
  Conditions:
    File opened.
  Input:
    fileName -  The new name of the file (in ROM)
    fo -        The file to rename
  Return Values:
    0 -  File renamed successfully
    -1 - File could not be renamed
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    Renames the file with the ascii ROM string(PIC18).The Fsrenamepgm
    function will copy the rom fileName specified by the user into a 
    RAM array and pass that array into the FSrename function.
  Remarks:
    This function is for use with PIC18 when passing arguments in ROM.                       
  *****************************************************************/

        int FSrenamepgm (const rom char * fileName, FSFILE * fo);
    #endif
#endif


/************************************************************
  Function:
    int FSfclose(FSFILE *fo)
  Summary:
    Update file information and free FSFILE objects
  Conditions:
    File opened
  Input:
    fo -  Pointer to the file to close
  Return Values:
    0 -   File closed successfully 
    EOF - Error closing the file
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    This function will update the directory entry for the 
    file pointed to by 'fo' with the information contained
    in 'fo,' including the new file size and attributes.
    Timestamp information will also be loaded based on the
    method selected by the user and written to the entry
    as the last modified time and date.  The file entry will
    then be written to the device.  Finally, the memory
    used for the specified file object will be freed from
    the dynamic heap or the array of FSFILE objects.
  Remarks:
    A function to flush data to the device without closing the
    file can be created by removing the portion of this
    function that frees the memory and the line that clears
    the write flag.
  ************************************************************/

int FSfclose(FSFILE *fo);


/*********************************************************
  Function:
    void FSrewind (FSFILE * fo)
  Summary:
    Set the current position in a file to the beginning
  Conditions:
    File opened.
  Input:
    fo -  Pointer to file structure
  Return Values:
    None
  Side Effects:
    None.
  Description:
    The FSrewind funciton will reset the position of the
    specified file to the beginning of the file.  This
    functionality is faster than using FSfseek to reset
    the position in the file.
  Remarks:
    None.
  *********************************************************/

void FSrewind (FSFILE *fo);


/**************************************************************************
  Function:
    size_t FSfread(void *ptr, size_t size, size_t n, FSFILE *stream)
  Summary:
    Read data from a file
  Conditions:
    File is opened in a read mode
  Input:
    ptr -     Destination buffer for read bytes
    size -    Size of units in bytes
    n -       Number of units to be read
    stream -  File to be read from
  Return:
    size_t - number of units read
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    The FSfread function will read data from the specified file.  First,
    the appropriate sector of the file is loaded.  Then, data is read into
    the specified buffer until the specified number of bytes have been read.
    When a cluster boundary is reached, a new cluster will be loaded.  The
    parameters 'size' and 'n' indicate how much data to read.  'Size'
    refers to the size of one object to read (in bytes), and 'n' will refer 
    to the number of these objects to read.  The value returned will be equal 
    to 'n' unless an error occured or the user tried to read beyond the end
    of the file.
  Remarks:
    None.
  **************************************************************************/

size_t FSfread(void *ptr, size_t size, size_t n, FSFILE *stream);


/**********************************************************************
  Function:
    int FSfseek(FSFILE *stream, long offset, int whence)
  Summary:
    Change the current position in a file
  Conditions:
    File opened
  Input:
    stream -    Pointer to file structure
    offset -    Offset from base location
    whence -    
           - SEEK_SET -  Seek from start of file
           - SEEK_CUR -  Seek from current location
           - SEEK_END -  Seek from end of file (subtract offset)
  Return Values:
    0 -  Operation successful 
    -1 - Operation unsuccesful
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    The FSfseek function will change the current position in the file to
    one specified by the user.  First, an absolute offset is calculated
    using the offset and base location passed in by the user.  Then, the
    position variables are updated, and the sector number that corresponds
    to the new location.  That sector is then loaded.  If the offset
    falls exactly on a cluster boundary, a new cluster will be allocated
    to the file and the position will be set to the first byte of that
    cluster.
  Remarks:
    None                                                               
  **********************************************************************/

int FSfseek(FSFILE *stream, long offset, int whence);


/*******************************************************************
  Function:
    long FSftell (FSFILE * fo)
  Summary:
    Determine the current location in a file
  Conditions:
    File opened
  Input:
    fo -  Pointer to file structure
  Return: Current location in the file
  Side Effects:
    The FSerrno variable will be changed
  Description:
    The FSftell function will return the current position in the
    file pointed to by 'fo' by returning the 'seek' variable in the
    FSFILE object, which is used to keep track of the absolute
    location of the current position in the file.
  Remarks:
    None                                                            
  *******************************************************************/

long FSftell(FSFILE *fo);


/****************************************************
  Function:
    int FSfeof( FSFILE * stream )
  Summary:
    Indicate whether the current file position is at the end
  Conditions:
    File is open in a read mode
  Input:
    stream -  Pointer to the target file
  Return Values:
    Non-Zero - EOF reached 
    0 - Not at end of File
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    The FSfeof function will indicate that the end-of-
    file has been reached for the specified file by
    comparing the absolute location in the file to the
    size of the file.
  Remarks:
    None.
  ****************************************************/

int FSfeof( FSFILE * stream );


#ifdef ALLOW_FORMATS
/*******************************************************************
  Function:
    int FSformat (char mode, long int serialNumber, char * volumeID)
  Summary:
    Formats a device
  Conditions:
    The device must possess a valid master boot record.
  Input:
    mode -          - 0 - Just erase the FAT and root
                    - 1 - Create a new boot sector
    serialNumber -  Serial number to write to the card
    volumeID -      Name of the card
  Return Values:
    0 -    Format was successful
    EOF -  Format was unsuccessful
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    The FSformat function can be used to create a new boot sector
    on a device, based on the information in the master boot record.
    This function will first initialize the I/O pins and the device,
    and then attempts to read the master boot record.  If the MBR
    cannot be loaded successfully, the function will fail.  Next, if
    the 'mode' argument is specified as '0' the existing boot sector
    information will be loaded.  If the 'mode' argument is '1' an
    entirely new boot sector will be constructed using the disk
    values from the master boot record.  Once the boot sector has
    been successfully loaded/created, the locations of the FAT and
    root will be loaded from it, and they will be completely
    erased.  If the user has specified a volumeID parameter, a 
    VOLUME attribute entry will be created in the root directory
    to name the device.

    FAT12, FAT16 and FAT32 formatting are supported.

    Based on the number of sectors, the format function automatically
    compute the smallest possible value for the cluster size in order to
    accommodate the physical size of the media. In this case, if a media 
    with a big capacity is formatted, the format function may take a very
    long time to write all the FAT tables. 

    Therefore, the FORMAT_SECTORS_PER_CLUSTER macro may be used to 
    specify the exact cluster size (in multiples of sector size). This 
    macro can be defined in FSconfig.h

  Remarks:
    Only devices with a sector size of 512 bytes are supported by the 
    format function                      
  *******************************************************************/

int FSformat (char mode, long int serialNumber, char * volumeID);
#endif


#ifdef ALLOW_WRITES
/***************************************************************************
  Function:
    int FSattrib (FSFILE * file, unsigned char attributes)
  Summary:
    Change the attributes of a file
  Conditions:
    File opened
  Input:
    file -        Pointer to file structure
    attributes -  The attributes to set for the file
               -  Attribute -      Value - Indications 
               -  ATTR_READ_ONLY - 0x01  - The read-only attribute
               -  ATTR_HIDDEN -    0x02  - The hidden attribute 
               -  ATTR_SYSTEM -    0x04  - The system attribute 
               -  ATTR_ARCHIVE -   0x20  - The archive attribute
  Return Values:
    0 -  Attribute change was successful 
    -1 - Attribute change was unsuccessful
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    The FSattrib funciton will set the attributes of the specified file
    to the attributes passed in by the user.  This function will load the
    file entry, replace the attributes with the ones specified, and write
    the attributes back.  If the specified file is a directory, the
    directory attribute will be preserved.
  Remarks:
    None                                                                
  ***************************************************************************/

int FSattrib (FSFILE * file, unsigned char attributes);


/***************************************************************
  Function:
    int FSrename (const rom char * fileName, FSFILE * fo)
  Summary:
    Renames the Ascii name of the file or directory on PIC24/PIC32/dsPIC devices
  Conditions:
    File opened.
  Input:
    fileName -  The new name of the file
    fo -        The file to rename
  Return Values:
    0 -   File was renamed successfully
    EOF - File was not renamed
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    Renames the Ascii name of the file or directory on PIC24/PIC32/dsPIC devices.
    First, it will search through the current working directory to ensure the
    specified new filename is not already in use. If it isn't, the new filename
    will be written to the file entry of the file pointed to by 'fo.'
  Remarks:
    None                                                        
  ***************************************************************/

int FSrename (const char * fileName, FSFILE * fo);

#ifdef SUPPORT_LFN
/***************************************************************
  Function:
    int wFSrename (const rom unsigned short int * fileName, FSFILE * fo)
  Summary:
    Renames the name of the file or directory to the UTF16 input fileName
    on PIC24/PIC32/dsPIC devices
  Conditions:
    File opened.
  Input:
    fileName -  The new name of the file
    fo -        The file to rename
  Return Values:
    0 -   File was renamed successfully
    EOF - File was not renamed
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    Renames the name of the file or directory to the UTF16 input fileName
    on PIC24/PIC32/dsPIC devices. First, it will search through the current
    working directory to ensure the specified new UTF16 filename is not
    already in use.  If it isn't, the new filename will be written to the
    file entry of the file pointed to by 'fo.'
  Remarks:
    None
  ***************************************************************/

int wFSrename (const unsigned short int * fileName, FSFILE * fo);
#endif

/*********************************************************************
  Function:
    int FSremove (const char * fileName)
  Summary:
    Deletes the file on PIC24/PIC32/dsPIC device.The 'fileName' is in ascii format.
  Conditions:
    File not opened, file exists
  Input:
    fileName -  Name of the file to erase
  Return Values:
    0 -   File removed 
    EOF - File was not removed
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    Deletes the file on PIC24/PIC32/dsPIC device.The 'fileName' is in ascii format.
    The FSremove function will attempt to find the specified file with the FILEfind
    function.  If the file is found, it will be erased using the FILEerase function.
    The user can also provide ascii alias name of the ascii long file name as the
    input to this function to get it erased from the memory.
  Remarks:
    None                                       
  **********************************************************************/

int FSremove (const char * fileName);

#ifdef SUPPORT_LFN
/*********************************************************************
  Function:
    int wFSremove (const unsigned short int * fileName)
  Summary:
    Deletes the file on PIC24/PIC32/dsPIC device.The 'fileName' is in UTF16 format.
  Conditions:
    File not opened, file exists
  Input:
    fileName -  Name of the file to erase
  Return Values:
    0 -   File removed
    EOF - File was not removed
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    Deletes the file on PIC24/PIC32/dsPIC device.The 'fileName' is in UTF16 format.
    The wFSremove function will attempt to find the specified UTF16 file
    name with the FILEfind function. If the file is found, it will be erased
    using the FILEerase function.
  Remarks:
    None
  **********************************************************************/

int wFSremove (const unsigned short int * fileName);
#endif

/*********************************************************************************
  Function:
    size_t FSfwrite(const void *data_to_write, size_t size, size_t n, FSFILE *stream)
  Summary:
    Write data to a file
  Conditions:
    File opened in FS_WRITE, FS_APPEND, FS_WRITE+, FS_APPEND+, FS_READ+ mode
  Input:
    data_to_write -     Pointer to source buffer
    size -              Size of units in bytes
    n -                 Number of units to transfer
    stream -            Pointer to file structure
  Return:
    size_t - number of units written
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    The FSfwrite function will write data to a file.  First, the sector that
    corresponds to the current position in the file will be loaded (if it hasn't
    already been cached in the global data buffer).  Data will then be written to
    the device from the specified buffer until the specified amount has been written.
    If the end of a cluster is reached, the next cluster will be loaded, unless
    the end-of-file flag for the specified file has been set.  If it has, a new
    cluster will be allocated to the file.  Finally, the new position and filesize
    will be stored in the FSFILE object.  The parameters 'size' and 'n' indicate how 
    much data to write.  'Size' refers to the size of one object to write (in bytes), 
    and 'n' will refer to the number of these objects to write.  The value returned 
    will be equal  to 'n' unless an error occured.
  Remarks:
    None.
  *********************************************************************************/

size_t FSfwrite(const void *data_to_write, size_t size, size_t n, FSFILE *stream);

#endif

#ifdef ALLOW_DIRS


/**************************************************************************
  Function:
    int FSchdir (char * path)
  Summary:
    Changes the current working directory to the ascii input path(PIC24/PIC32/dsPIC)
  Conditions:
    None
  Input:
    path - The path of the directory to change to.
  Return Values:
    0 -   The current working directory was changed successfully
    EOF - The current working directory could not be changed
  Side Effects:
    The current working directory may be changed. The FSerrno variable will
    be changed.
  Description:
    Changes the current working directory to the ascii input path(PIC24/PIC32/dsPIC).
    The FSchdir function passes a RAM pointer to the path to the chdirhelper function.
  Remarks:
    None                                            
  **************************************************************************/

int FSchdir (char * path);

#ifdef SUPPORT_LFN
/**************************************************************************
  Function:
    int wFSchdir (unsigned short int * path)
  Summary:
    Change the current working directory as per the path specified in
    UTF16 format (PIC24/PIC32/dsPIC)
  Conditions:
    None
  Input:
    path - The path of the directory to change to.
  Return Values:
    0 -   The current working directory was changed successfully
    EOF - The current working directory could not be changed
  Side Effects:
    The current working directory may be changed. The FSerrno variable will
    be changed.
  Description:
    Change the current working directory as per the path specified in
    UTF16 format (PIC24/PIC32/dsPIC).The FSchdir function passes a RAM
    pointer to the path to the chdirhelper function.
  Remarks:
    None                                            
  **************************************************************************/

int wFSchdir (unsigned short int * path);
#endif

/**************************************************************
  Function:
    char * FSgetcwd (char * path, int numchars)
  Summary:
    Get the current working directory path in Ascii format
  Conditions:
    None
  Input:
    path -      Pointer to the array to return the cwd name in
    numchars -  Number of chars in the path
  Return Values:
    char * - The cwd name string pointer (path or defaultArray)
    NULL -   The current working directory name could not be loaded.
  Side Effects:
    The FSerrno variable will be changed
  Description:
    Get the current working directory path in Ascii format.
    The FSgetcwd function will get the name of the current
    working directory and return it to the user.  The name
    will be copied into the buffer pointed to by 'path,'
    starting at the root directory and copying as many chars
    as possible before the end of the buffer.  The buffer
    size is indicated by the 'numchars' argument.  The first
    thing this function will do is load the name of the current
    working directory, if it isn't already present.  This could
    occur if the user switched to the dotdot entry of a
    subdirectory immediately before calling this function.  The
    function will then copy the current working directory name 
    into the buffer backwards, and insert a backslash character.  
    Next, the function will continuously switch to the previous 
    directories and copy their names backwards into the buffer
    until it reaches the root.  If the buffer overflows, it
    will be treated as a circular buffer, and data will be
    copied over existing characters, starting at the beginning.
    Once the root directory is reached, the text in the buffer
    will be swapped, so that the buffer contains as much of the
    current working directory name as possible, starting at the 
    root.
  Remarks:
    None                                                       
  **************************************************************/

char * FSgetcwd (char * path, int numbchars);

#ifdef SUPPORT_LFN
/**************************************************************
  Function:
    char * wFSgetcwd (unsigned short int * path, int numchars)
  Summary:
    Get the current working directory path in UTF16 format
  Conditions:
    None
  Input:
    path -      Pointer to the array to return the cwd name in
    numchars -  Number of chars in the path
  Return Values:
    char * - The cwd name string pointer (path or defaultArray)
    NULL -   The current working directory name could not be loaded.
  Side Effects:
    The FSerrno variable will be changed
  Description:
    Get the current working directory path in UTF16 format.
    The FSgetcwd function will get the name of the current
    working directory and return it to the user.  The name
    will be copied into the buffer pointed to by 'path,'
    starting at the root directory and copying as many chars
    as possible before the end of the buffer.  The buffer
    size is indicated by the 'numchars' argument.  The first
    thing this function will do is load the name of the current
    working directory, if it isn't already present.  This could
    occur if the user switched to the dotdot entry of a
    subdirectory immediately before calling this function.  The
    function will then copy the current working directory name 
    into the buffer backwards, and insert a backslash character.  
    Next, the function will continuously switch to the previous 
    directories and copy their names backwards into the buffer
    until it reaches the root.  If the buffer overflows, it
    will be treated as a circular buffer, and data will be
    copied over existing characters, starting at the beginning.
    Once the root directory is reached, the text in the buffer
    will be swapped, so that the buffer contains as much of the
    current working directory name as possible, starting at the 
    root.
  Remarks:
    None                                                       
  **************************************************************/

char * wFSgetcwd (unsigned short int * path, int numbchars);
#endif

#ifdef ALLOW_WRITES

/**************************************************************************
  Function:
    int FSmkdir (char * path)
  Summary:
    Creates a directory as per the ascii input path (PIC24/PIC32/dsPIC)
  Conditions:
    None
  Input:
    path - The path of directories to create.
  Return Values:
    0 -   The specified directory was created successfully
    EOF - The specified directory could not be created
  Side Effects:
    Will create all non-existent directories in the path. The FSerrno 
    variable will be changed.
  Description:
    Creates a directory as per the ascii input path (PIC24/PIC32/dsPIC).
    This function doesn't move the current working directory setting.
  Remarks:
    None                                            
  **************************************************************************/

int FSmkdir (char * path);

#ifdef SUPPORT_LFN
/**************************************************************************
  Function:
    int wFSmkdir (unsigned short int * path)
  Summary:
    Creates a directory as per the UTF16 input path (PIC24/PIC32/dsPIC)
  Conditions:
    None
  Input:
    path - The path of directories to create.
  Return Values:
    0 -   The specified directory was created successfully
    EOF - The specified directory could not be created
  Side Effects:
    Will create all non-existent directories in the path. The FSerrno 
    variable will be changed.
  Description:
    Creates a directory as per the UTF16 input path (PIC24/PIC32/dsPIC).
    This function doesn't move the current working directory setting.
  Remarks:
    None                                            
  **************************************************************************/

int wFSmkdir (unsigned short int * path);
#endif

/**************************************************************************
  Function:
    int FSrmdir (char * path)
  Summary:
    Deletes the directory as per the ascii input path (PIC24/PIC32/dsPIC).
  Conditions:
    None
  Input:
    path -      The path of the directory to remove
    rmsubdirs - 
              - TRUE -  All sub-dirs and files in the target dir will be removed
              - FALSE - FSrmdir will not remove non-empty directories
  Return Values:
    0 -   The specified directory was deleted successfully
    EOF - The specified directory could not be deleted
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    Deletes the directory as per the ascii input path (PIC24/PIC32/dsPIC).
    This function wont delete the current working directory.
  Remarks:
    None.
  **************************************************************************/

int FSrmdir (char * path, unsigned char rmsubdirs);

#ifdef SUPPORT_LFN
/**************************************************************************
  Function:
    int wFSrmdir (unsigned short int * path, unsigned char rmsubdirs)
  Summary:
    Deletes the directory as per the UTF16 input path (PIC24/PIC32/dsPIC).
  Conditions:
    None
  Input:
    path -      The path of the directory to remove
    rmsubdirs - 
              - TRUE -  All sub-dirs and files in the target dir will be removed
              - FALSE - FSrmdir will not remove non-empty directories
  Return Values:
    0 -   The specified directory was deleted successfully
    EOF - The specified directory could not be deleted
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    Deletes the directory as per the UTF16 input path (PIC24/PIC32/dsPIC).
    This function wont delete the current working directory.
  Remarks:
    None.
  **************************************************************************/

int wFSrmdir (unsigned short int * path, unsigned char rmsubdirs);
#endif

#endif

#endif

#ifdef USERDEFINEDCLOCK


/***********************************************************************************************************
  Function:
    int SetClockVars (unsigned int year, unsigned char month, unsigned char day, unsigned char hour, unsigned char minute, unsigned char second)
  Summary:
    Manually set timestamp variables
  Conditions:
    USERDEFINEDCLOCK macro defined in FSconfig.h.
  Input:
    year -     The year (1980\-2107)
    month -   The month (1\-12)
    day -     The day of the month (1\-31)
    hour -    The hour (0\-23)
    minute -  The minute (0\-59)
    second -  The second (0\-59)
  Return Values:
    None
  Side Effects:
    Modifies global timing variables
  Description:
    Lets the user manually set the timing variables.  The values passed in will be converted to the format
    used by the FAT timestamps.
  Remarks:
    Call this before creating a file or directory (set create time) and
    before closing a file (set last access time, last modified time)                                        
  ***********************************************************************************************************/

int SetClockVars (unsigned int year, unsigned char month, unsigned char day, unsigned char hour, unsigned char minute, unsigned char second);
#endif


#ifdef ALLOW_FILESEARCH

/***********************************************************************************
  Function:
    int FindFirst (const char * fileName, unsigned int attr, SearchRec * rec)
  Summary:
    Initial search function for the input Ascii fileName on PIC24/PIC32/dsPIC devices.
  Conditions:
    None
  Input:
    fileName - The name to search for
             - Parital string search characters
             - * - Indicates the rest of the filename or extension can vary (e.g. FILE.*)
             - ? - Indicates that one character in a filename can vary (e.g. F?LE.T?T)
    attr -            The attributes that a found file may have
         - ATTR_READ_ONLY -  File may be read only
         - ATTR_HIDDEN -     File may be a hidden file
         - ATTR_SYSTEM -     File may be a system file
         - ATTR_VOLUME -     Entry may be a volume label
         - ATTR_DIRECTORY -  File may be a directory
         - ATTR_ARCHIVE -    File may have archive attribute
         - ATTR_MASK -       All attributes
    rec -             pointer to a structure to put the file information in
  Return Values:
    0 -  File was found
    -1 - No file matching the specified criteria was found
  Side Effects:
    Search criteria from previous FindFirst call on passed SearchRec object
    will be lost. "utf16LFNfound" is overwritten after subsequent FindFirst/FindNext
    operations.It is the responsibility of the application to read the "utf16LFNfound"
    before it is lost.The FSerrno variable will be changed.
  Description:
    Initial search function for the input Ascii fileName on PIC24/PIC32/dsPIC devices.
    The FindFirst function will search for a file based on parameters passed in
    by the user.  This function will use the FILEfind function to parse through
    the current working directory searching for entries that match the specified
    parameters.  If a file is found, its parameters are copied into the SearchRec
    structure, as are the initial parameters passed in by the user and the position
    of the file entry in the current working directory.If the return value of the 
    function is 0 then "utf16LFNfoundLength" indicates whether the file found was 
    long file name or short file name(8P3 format). The "utf16LFNfoundLength" is non-zero
    for long file name and is zero for 8P3 format."utf16LFNfound" points to the
    address of long file name if found during the operation.
  Remarks:
    Call FindFirst or FindFirstpgm before calling FindNext
  ***********************************************************************************/

int FindFirst (const char * fileName, unsigned int attr, SearchRec * rec);

#ifdef SUPPORT_LFN
/***********************************************************************************
  Function:
    int wFindFirst (const unsigned short int * fileName, unsigned int attr, SearchRec * rec)
  Summary:
    Initial search function for the 'fileName' in UTF16 format on PIC24/PIC32/dsPIC devices.
  Conditions:
    None
  Input:
    fileName - The name to search for
             - Parital string search characters
             - * - Indicates the rest of the filename or extension can vary (e.g. FILE.*)
             - ? - Indicates that one character in a filename can vary (e.g. F?LE.T?T)
    attr -            The attributes that a found file may have
         - ATTR_READ_ONLY -  File may be read only
         - ATTR_HIDDEN -     File may be a hidden file
         - ATTR_SYSTEM -     File may be a system file
         - ATTR_VOLUME -     Entry may be a volume label
         - ATTR_DIRECTORY -  File may be a directory
         - ATTR_ARCHIVE -    File may have archive attribute
         - ATTR_MASK -       All attributes
    rec -             pointer to a structure to put the file information in
  Return Values:
    0 -  File was found
    -1 - No file matching the specified criteria was found
  Side Effects:
    Search criteria from previous wFindFirst call on passed SearchRec object
    will be lost. "utf16LFNfound" is overwritten after subsequent wFindFirst/FindNext
    operations.It is the responsibility of the application to read the "utf16LFNfound"
    before it is lost.The FSerrno variable will be changed.
  Description:
    Initial search function for the 'fileName' in UTF16 format on PIC24/PIC32/dsPIC devices.
    The wFindFirst function will search for a file based on parameters passed in
    by the user.  This function will use the FILEfind function to parse through
    the current working directory searching for entries that match the specified
    parameters.  If a file is found, its parameters are copied into the SearchRec
    structure, as are the initial parameters passed in by the user and the position
    of the file entry in the current working directory.If the return value of the 
    function is 0 then "utf16LFNfoundLength" indicates whether the file found was 
    long file name or short file name(8P3 format). The "utf16LFNfoundLength" is non-zero
    for long file name and is zero for 8P3 format."utf16LFNfound" points to the
    address of long file name if found during the operation.
  Remarks:
    Call FindFirst or FindFirstpgm before calling FindNext
  ***********************************************************************************/

int wFindFirst (const unsigned short int * fileName, unsigned int attr, SearchRec * rec);
#endif

/**********************************************************************
  Function:
    int FindNext (SearchRec * rec)
  Summary:
    Sequential search function
  Conditions:
    None
  Input:
    rec -  The structure to store the file information in
  Return Values:
    0 -  File was found
    -1 - No additional files matching the specified criteria were found
  Side Effects:
    Search criteria from previous FindNext call on passed SearchRec object
    will be lost. "utf16LFNfound" is overwritten after subsequent FindFirst/FindNext
    operations.It is the responsibility of the application to read the "utf16LFNfound"
    before it is lost.The FSerrno variable will be changed.
  Description:
    The FindNext function performs the same function as the FindFirst
    funciton, except it does not copy any search parameters into the
    SearchRec structure (only info about found files) and it begins
    searching at the last directory entry offset at which a file was
    found, rather than at the beginning of the current working
    directory.If the return value of the function is 0 then "utf16LFNfoundLength"
    indicates whether the file found was long file name or short file
    name(8P3 format). The "utf16LFNfoundLength" is non-zero for long file name
    and is zero for 8P3 format."utf16LFNfound" points to the address of long 
    file name if found during the operation.
  Remarks:
    Call FindFirst or FindFirstpgm before calling this function
  **********************************************************************/

int FindNext (SearchRec * rec); 
#endif


/**********************************************************************
  Function:
    // PIC24/30/33/32
    int FSfprintf (FSFILE * fptr, const char * fmt, ...)
    // PIC18
    int FSfpritnf (FSFILE * fptr, const rom char * fmt, ...)
  Summary:
    Function to write formatted strings to a file
  Conditions:
    For PIC18, integer promotion must be enabled in the project build
    options menu.  File opened in a write mode.
  Input:
    fptr - A pointer to the file to write to.
    fmt -  A string of characters and format specifiers to write to
           the file
    ... -  Additional arguments inserted in the string by format
           specifiers
  Returns:
    The number of characters written to the file
  Side Effects:
    The FSerrno variable will be changed.
  Description:
    Writes a specially formatted string to a file.
  Remarks:
    Consult AN1045 for a full description of how to use format
    specifiers.        
  **********************************************************************/

#ifdef ALLOW_FSFPRINTF
    #ifdef __18CXX
        int FSfprintf (FSFILE *fptr, const rom char *fmt, ...);
    #else
        int FSfprintf (FSFILE *fptr, const char * fmt, ...);
    #endif
#endif


/**************************************************************************
  Function:
    int FSerror (void)
  Summary:
    Return an error code for the last function call
  Conditions:
    The return value depends on the last function called.
  Input:
    None
  Side Effects:
    None.
  Return Values:
    FSInit       - 
                 - CE_GOOD –                  No Error 
                 - CE_INIT_ERROR –            The physical media could not be initialized
                 - CE_BAD_SECTOR_READ –       The MBR or the boot sector could not be
                                              read correctly
                 - CE_BAD_PARITION –          The MBR signature code was incorrect.
                 - CE_NOT_FORMATTED –         The boot sector signature code was incorrect or
                                              indicates an invalid number of bytes per sector. 
                 - CE_CARDFAT32 –             The physical media is FAT32 type (only an error 
                                              when FAT32 support is disabled). 
                 - CE_UNSUPPORTED_FS –        The device is formatted with an unsupported file 
                                              system (not FAT12 or 16).
    FSfopen      -
                 - CE_GOOD –                  No Error 
                 - CE_NOT_INIT –              The device has not been initialized. 
                 - CE_TOO_MANY_FILES_OPEN –   The function could not allocate any 
                                              additional file information to the array 
                                              of FSFILE structures or the heap. 
                 - CE_INVALID_FILENAME –      The file name argument was invalid.
                 - CE_INVALID_ARGUMENT –      The user attempted to open a directory in a 
                                              write mode or specified an invalid mode argument. 
                 - CE_FILE_NOT_FOUND –        The specified file (which was to be opened in read 
                                              mode) does not exist on the device. 
                 - CE_BADCACHEREAD –          A read from the device failed.
                 - CE_ERASE_FAIL –            The existing file could not be erased (when opening 
                                              a file in FS_WRITE mode). 
                 - CE_DIR_FULL –              The directory is full. 
                 - CE_DISK_FULL–              The data memory section is full. 
                 - CE_WRITE_ERROR –           A write to the device failed. 
                 - CE_SEEK_ERROR –            The current position in the file could not be set to 
                                              the end (when the file was opened in FS_APPEND mode).
    FSfclose     - 
                 - CE_GOOD –                  No Error 
                 - CE_WRITE_ERROR –           The existing data in the data buffer or the new file
                                              entry information could not be written to the device.
                 - CE_BADCACHEREAD –          The file entry information could not be cached
    FSfread      - 
                 - CE_GOOD –                  No Error 
                 - CE_WRITEONLY –             The file was opened in a write-only mode.
                 - CE_WRITE_ERROR –           The existing data in the data buffer could not be 
                                              written to the device. 
                 - CE_BAD_SECTOR_READ –       The data sector could not be read. 
                 - CE_EOF –                   The end of the file was reached.
                 - CE_COULD_NOT_GET_CLUSTER – Additional clusters in the file could not be loaded.
    FSfwrite     - 
                 - CE_GOOD –                  No Error 
                 - CE_READONLY –              The file was opened in a read-only mode. 
                 - CE_WRITE_PROTECTED –       The device write-protect check function indicated 
                                              that the device has been write-protected.
                 - CE_WRITE_ERROR –           There was an error writing data to the device.
                 - CE_BADCACHEREAD –          The data sector to be modified could not be read from
                                              the device. 
                 - CE_DISK_FULL –             All data clusters on the device are in use.
    FSfseek      -
                 - CE_GOOD –                  No Error 
                 - CE_WRITE_ERROR –           The existing data in the data buffer could not be
                                              written to the device. 
                 - CE_INVALID_ARGUMENT –      The specified offset exceeds the size of the file. 
                 - CE_BADCACHEREAD –          The sector that contains the new current position 
                                              could not be loaded.
                 - CE_COULD_NOT_GET_CLUSTER – Additional clusters in the file could not be 
                                              loaded/allocated.
    FSftell      - 
                 - CE_GOOD –                  No Error
    FSattrib     -
                 - CE_GOOD –                  No Error 
                 - CE_INVALID_ARGUMENT –      The attribute argument was invalid. 
                 - CE_BADCACHEREAD –          The existing file entry information could not be 
                                              loaded. 
                 - CE_WRITE_ERROR –           The file entry information could not be written to 
                                              the device.
    FSrename     -
                 - CE_GOOD –                  No Error 
                 - CE_FILENOTOPENED –         A null file pointer was passed into the function. 
                 - CE_INVALID_FILENAME –      The file name passed into the function was invalid. 
                 - CE_BADCACHEREAD –          A read from the device failed. 
                 - CE_FILENAME_EXISTS –       A file with the specified name already exists. 
                 - CE_WRITE_ERROR –           The new file entry data could not be written to the 
                                              device.
    FSfeof       -
                 - CE_GOOD –                  No Error
    FSformat     -
                 - CE_GOOD –                  No Error 
                 - CE_INIT_ERROR –            The device could not be initialized. 
                 - CE_BADCACHEREAD –          The master boot record or boot sector could not be 
                                              loaded successfully. 
                 - CE_INVALID_ARGUMENT –      The user selected to create their own boot sector on 
                                              a device that has no master boot record, or the mode 
                                              argument was invalid. 
                 - CE_WRITE_ERROR –           The updated MBR/Boot sector could not be written to 
                                              the device.
                 - CE_BAD_PARTITION –         The calculated number of sectors per clusters was 
                                              invalid. 
                 - CE_NONSUPPORTED_SIZE –     The card has too many sectors to be formatted as 
                                              FAT12 or FAT16.
    FSremove     - 
                 - CE_GOOD –                  No Error 
                 - CE_WRITE_PROTECTED –       The device write-protect check function indicated 
                                              that the device has been write-protected. 
                 - CE_INVALID_FILENAME –      The specified filename was invalid. 
                 - CE_FILE_NOT_FOUND –        The specified file could not be found.
                 - CE_ERASE_FAIL –            The file could not be erased.
    FSchdir      -
                 - CE_GOOD –                  No Error 
                 - CE_INVALID_ARGUMENT –      The path string was mis-formed or the user tried to 
                                              change to a non-directory file.
                 - CE_BADCACHEREAD –          A directory entry could not be cached.
                 - CE_DIR_NOT_FOUND –         Could not find a directory in the path.
    FSgetcwd     -
                 - CE_GOOD –                  No Error 
                 - CE_INVALID_ARGUMENT –      The user passed a 0-length buffer into the function.
                 - CE_BADCACHEREAD –          A directory entry could not be cached. 
                 - CE_BAD_SECTOR_READ –       The function could not determine a previous directory 
                                              of the current working directory.
    FSmkdir      -
                 - CE_GOOD –                  No Error 
                 - CE_WRITE_PROTECTED –       The device write-protect check function indicated 
                                              that the device has been write-protected. 
                 - CE_INVALID_ARGUMENT –      The path string was mis-formed.
                 - CE_BADCACHEREAD –          Could not successfully change to a recently created 
                                              directory to store its dir entry information, or 
                                              could not cache directory entry information. 
                 - CE_INVALID_FILENAME –      One or more of the directory names has an invalid 
                                              format. 
                 - CE_WRITE_ERROR –           The existing data in the data buffer could not be 
                                              written to the device or the dot/dotdot entries could 
                                              not be written to a newly created directory.
                 - CE_DIR_FULL –              There are no available dir entries in the CWD.
                 - CE_DISK_FULL –             There are no available clusters in the data region of 
                                              the device.    
    FSrmdir      - 
                 - CE_GOOD –                  No Error 
                 - CE_DIR_NOT_FOUND –         The directory specified could not be found or the 
                                              function could not change to a subdirectory within 
                                              the directory to be deleted (when recursive delete is 
                                              enabled).
                 - CE_INVALID_ARGUMENT –      The user tried to remove the CWD or root directory. 
                 - CE_BADCACHEREAD –          A directory entry could not be cached.
                 - CE_DIR_NOT_EMPTY –         The directory to be deleted was not empty and 
                                              recursive subdirectory removal was disabled. 
                 - CE_ERASE_FAIL –            The directory or one of the directories or files 
                                              within it could not be deleted. 
                 - CE_BAD_SECTOR_READ –       The function could not determine a previous directory 
                                              of the CWD.
    SetClockVars -
                 - CE_GOOD –                  No Error 
                 - CE_INVALID_ARGUMENT –      The time values passed into the function were 
                                              invalid.    
    FindFirst    - 
                 - CE_GOOD –                  No Error 
                 - CE_INVALID_FILENAME –      The specified filename was invalid. 
                 - CE_FILE_NOT_FOUND –        No file matching the specified criteria was found. 
                 - CE_BADCACHEREAD –          The file information for the file that was found 
                                              could not be cached.
    FindNext     - 
                 - CE_GOOD –                  No Error 
                 - CE_NOT_INIT –              The SearchRec object was not initialized by a call to 
                                              FindFirst. 
                 - CE_INVALID_ARGUMENT –      The SearchRec object was initialized in a different 
                                              directory from the CWD.
                 - CE_INVALID_FILENAME –      The filename is invalid. 
                 - CE_FILE_NOT_FOUND –        No file matching the specified criteria was found.
    FSfprintf    - 
                 - CE_GOOD –                  No Error 
                 - CE_WRITE_ERROR –           Characters could not be written to the file.                                                   
  Description:
    The FSerror function will return the FSerrno variable.  This global
    variable will have been set to an error value during the last call of a
    library function.
  Remarks:
    None
  **************************************************************************/

int FSerror (void);


/*********************************************************************************
  Function:
    int FSCreateMBR (unsigned long firstSector, unsigned long numSectors)
  Summary:
    Creates a master boot record
  Conditions:
    The I/O pins for the device have been initialized by the InitIO function.
  Input:
    firstSector -  The first sector of the partition on the device (cannot
                   be 0; that's the MBR)
    numSectors -   The number of sectors available in memory (including the
                   MBR)
  Return Values:
    0 -   MBR was created successfully
    EOF - MBR could not be created
  Side Effects:
    None
  Description:  
    This function can be used to create a master boot record for a device.  Note
    that this function should not be used on a device that is already formatted
    with a master boot record (i.e. most SD cards, CF cards, USB keys).  This
    function will fill the global data buffer with appropriate partition information
    for a FAT partition with a type determined by the number of sectors available
    to the partition.  It will then write the MBR information to the first sector
    on the device.  This function should be followed by a call to FSformat, which
    will create a boot sector, root dir, and FAT appropriate the the information
    contained in the new master boot record.  Note that FSformat only supports
    FAT12 and FAT16 formatting at this time, and so cannot be used to format a
    device with more than 0x3FFD5F sectors.
  Remarks:
    This function can damage the device being used, and should not be called
    unless the user is sure about the size of the device and the first sector value.
  *********************************************************************************/

int FSCreateMBR (unsigned long firstSector, unsigned long numSectors);


#ifdef ALLOW_GET_DISK_PROPERTIES
/*********************************************************************************
  Function:
    void FSGetDiskProperties(FS_DISK_PROPERTIES* properties)
  Summary:
    Allows user to get the disk properties (size of disk, free space, etc)
  Conditions:
    1) ALLOW_GET_DISK_PROPERTIES must be defined in FSconfig.h
    2) a FS_DISK_PROPERTIES object must be created before the function is called
    3) the new_request member of the FS_DISK_PROPERTIES object must be set before
        calling the function for the first time.  This will start a new search.
    4) this function should not be called while there is a file open.  Close all
        files before calling this function.
  Input:
    properties - a pointer to a FS_DISK_PROPERTIES object where the results should
      be stored.
  Return Values:
    This function returns void.  The properties_status of the previous call of 
      this function is located in the properties.status field.  This field has 
      the following possible values:

    FS_GET_PROPERTIES_NO_ERRORS - operation completed without error.  Results
      are in the properties object passed into the function.
    FS_GET_PROPERTIES_DISK_NOT_MOUNTED - there is no mounted disk.  Results in
      properties object is not valid
    FS_GET_PROPERTIES_CLUSTER_FAILURE - there was a failure trying to read a 
      cluster from the drive.  The results in the properties object is a partial
      result up until the point of the failure.
    FS_GET_PROPERTIES_STILL_WORKING - the search for free sectors is still in
      process.  Continue calling this function with the same properties pointer 
      until either the function completes or until the partial results meets the
      application needs.  The properties object contains the partial results of
      the search and can be used by the application.  
  Side Effects:
    Can cause errors if called when files are open.  Close all files before
    calling this function.

    Calling this function without setting the new_request member on the first
    call can result in undefined behavior and results.

    Calling this function after a result is returned other than
    FS_GET_PROPERTIES_STILL_WORKING can result in undefined behavior and results.
  Description:  
    This function returns the information about the mounted drive.  The results 
    member of the properties object passed into the function is populated with 
    the information about the drive.    

    Before starting a new request, the new_request member of the properties
    input parameter should be set to TRUE.  This will initiate a new search
    request.

    This function will return before the search is complete with partial results.
    All of the results except the free_clusters will be correct after the first
    call.  The free_clusters will contain the number of free clusters found up
    until that point, thus the free_clusters result will continue to grow until
    the entire drive is searched.  If an application only needs to know that a 
    certain number of bytes is available and doesn't need to know the total free 
    size, then this function can be called until the required free size is
    verified.  To continue a search, pass a pointer to the same FS_DISK_PROPERTIES
    object that was passed in to create the search.

    A new search request sould be made once this function has returned a value 
    other than FS_GET_PROPERTIES_STILL_WORKING.  Continuing a completed search
    can result in undefined behavior or results.

    Typical Usage:
    <code>
    FS_DISK_PROPERTIES disk_properties;

    disk_properties.new_request = TRUE;

    do
    {
        FSGetDiskProperties(&disk_properties);
    } while (disk_properties.properties_status == FS_GET_PROPERTIES_STILL_WORKING);
    </code>

    results.disk_format - contains the format of the drive.  Valid results are 
      FAT12(1), FAT16(2), or FAT32(3).

    results.sector_size - the sector size of the mounted drive.  Valid values are
      512, 1024, 2048, and 4096.

    results.sectors_per_cluster - the number sectors per cluster.

    results.total_clusters - the number of total clusters on the drive.  This 
      can be used to calculate the total disk size (total_clusters * 
      sectors_per_cluster * sector_size = total size of drive in bytes)

    results.free_clusters - the number of free (unallocated) clusters on the drive.
      This can be used to calculate the total free disk size (free_clusters * 
      sectors_per_cluster * sector_size = total size of drive in bytes)

  Remarks:
    PIC24F size estimates:
      Flash - 400 bytes (-Os setting)

    PIC24F speed estimates:
      Search takes approximately 7 seconds per Gigabyte of drive space.  Speed
        will vary based on the number of sectors per cluster and the sector size.
  *********************************************************************************/
void FSGetDiskProperties(FS_DISK_PROPERTIES* properties);
#endif

/*************************************************************************
  Function:
    BYTE FILEget_next_cluster(FSFILE *fo, DWORD n)
  Summary:
    Step through a chain of clusters
  Conditions:
    None
  Input:
    fo - The file to get the next cluster of
    n -  Number of links in the FAT cluster chain to jump through
  Return Values:
    CE_GOOD - Operation successful
    CE_BAD_SECTOR_READ - A bad read occured of a sector
    CE_INVALID_CLUSTER - Invalid cluster value \> maxcls
    CE_FAT_EOF - Fat attempt to read beyond EOF
  Side Effects:
    None
  Description:
    This function will load 'n' proximate clusters for a file from
    the FAT on the device.  It will stop checking for clusters if the
    ReadFAT function returns an error, if it reaches the last cluster in
    a file, or if the device tries to read beyond the last cluster used
    by the device.
  Remarks:
    None
  *************************************************************************/

BYTE FILEget_next_cluster(FSFILE *fo, DWORD n);

#endif
