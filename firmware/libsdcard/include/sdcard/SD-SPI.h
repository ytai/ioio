/******************************************************************************
 *
 *                Microchip Memory Disk Drive File System
 *
 ******************************************************************************
 * FileName:        SD-SPI.h
 * Dependencies:    GenericTypeDefs.h
 *                  FSconfig.h
 *                  FSDefs.h
 * Processor:       PIC18/PIC24/dsPIC30/dsPIC33/PIC32
 * Compiler:        C18/C30/C32
 * Company:         Microchip Technology, Inc.
 * Version:         1.3.0
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
*****************************************************************************/

#ifndef SDMMC_H
#define SDMMC_H

#include "GenericTypeDefs.h"
#include "FSconfig.h"
#include "FSDefs.h"


#ifdef __18CXX
    // Description: This macro is used to initialize a PIC18 SPI module with a 4x prescale divider
    #define   SYNC_MODE_FAST    0x00
    // Description: This macro is used to initialize a PIC18 SPI module with a 16x prescale divider
    #define   SYNC_MODE_MED     0x01
    // Description: This macro is used to initialize a PIC18 SPI module with a 64x prescale divider
    #define   SYNC_MODE_SLOW    0x02
#elif defined __PIC32MX__
    // Description: This macro is used to initialize a PIC32 SPI module
    #define   SYNC_MODE_FAST    0x3E
    // Description: This macro is used to initialize a PIC32 SPI module
    #define   SYNC_MODE_SLOW    0x3C
#else
    // Description: This macro indicates the SPI enable bit for 16-bit PICs
    #ifndef MASTER_ENABLE_ON
        #define  MASTER_ENABLE_ON       0x0020
    #endif

    // Description: This macro is used to initialize a 16-bit PIC SPI module
    #ifndef SYNC_MODE_FAST
        #define   SYNC_MODE_FAST    0x3E
    #endif
    // Description: This macro is used to initialize a 16-bit PIC SPI module
    #ifndef SYNC_MODE_SLOW
        #define   SYNC_MODE_SLOW    0x3C
    #endif

    // Description: This macro is used to initialize a 16-bit PIC SPI module secondary prescaler
    #ifndef SEC_PRESCAL_1_1
        #define  SEC_PRESCAL_1_1        0x001c
    #endif
    // Description: This macro is used to initialize a 16-bit PIC SPI module primary prescaler
    #ifndef PRI_PRESCAL_1_1
        #define  PRI_PRESCAL_1_1        0x0003
    #endif
#endif



/*****************************************************************/
/*                  Strcutures and defines                       */
/*****************************************************************/


// Description: This macro represents an SD card start single data block token (used for single block writes)
#define DATA_START_TOKEN            0xFE

// Description: This macro represents an SD card start multi-block data token (used for multi-block writes)
#define DATA_START_MULTI_BLOCK_TOKEN    0xFC

// Description: This macro represents an SD card stop transmission token.  This is used when finishing a multi block write sequence.
#define DATA_STOP_TRAN_TOKEN        0xFD

// Description: This macro represents an SD card data accepted token
#define DATA_ACCEPTED               0x05

// Description: This macro indicates that the SD card expects to transmit or receive more data
#define MOREDATA    !0

// Description: This macro indicates that the SD card does not expect to transmit or receive more data
#define NODATA      0

// Description: This macro represents a floating SPI bus condition
#define MMC_FLOATING_BUS    0xFF

// Description: This macro represents a bad SD card response byte
#define MMC_BAD_RESPONSE    MMC_FLOATING_BUS

// The SDMMC Commands

// Description: This macro defines the command code to reset the SD card
#define     cmdGO_IDLE_STATE        0
// Description: This macro defines the command code to initialize the SD card
#define     cmdSEND_OP_COND         1        
// Description: This macro defined the command code to check for sector addressing
#define     cmdSEND_IF_COND         8
// Description: This macro defines the command code to get the Card Specific Data
#define     cmdSEND_CSD             9
// Description: This macro defines the command code to get the Card Information
#define     cmdSEND_CID             10
// Description: This macro defines the command code to stop transmission during a multi-block read
#define     cmdSTOP_TRANSMISSION    12
// Description: This macro defines the command code to get the card status information
#define     cmdSEND_STATUS          13
// Description: This macro defines the command code to set the block length of the card
#define     cmdSET_BLOCKLEN         16
// Description: This macro defines the command code to read one block from the card
#define     cmdREAD_SINGLE_BLOCK    17
// Description: This macro defines the command code to read multiple blocks from the card
#define     cmdREAD_MULTI_BLOCK     18
// Description: This macro defines the command code to tell the media how many blocks to pre-erase (for faster multi-block writes to follow)
//Note: This is an "application specific" command.  This tells the media how many blocks to pre-erase for the subsequent WRITE_MULTI_BLOCK
#define     cmdSET_WR_BLK_ERASE_COUNT   23
// Description: This macro defines the command code to write one block to the card
#define     cmdWRITE_SINGLE_BLOCK   24    
// Description: This macro defines the command code to write multiple blocks to the card
#define     cmdWRITE_MULTI_BLOCK    25
// Description: This macro defines the command code to set the address of the start of an erase operation
#define     cmdTAG_SECTOR_START     32
// Description: This macro defines the command code to set the address of the end of an erase operation
#define     cmdTAG_SECTOR_END       33
// Description: This macro defines the command code to erase all previously selected blocks
#define     cmdERASE                38
//Description: This macro defines the command code to intitialize an SD card and provide the CSD register value.
//Note: this is an "application specific" command (specific to SD cards) and must be preceded by cmdAPP_CMD.
#define     cmdSD_SEND_OP_COND      41
// Description: This macro defines the command code to begin application specific command inputs
#define     cmdAPP_CMD              55
// Description: This macro defines the command code to get the OCR register information from the card
#define     cmdREAD_OCR             58
// Description: This macro defines the command code to disable CRC checking
#define     cmdCRC_ON_OFF           59


// Description: Enumeration of different SD response types
typedef enum
{
    R1,     // R1 type response
    R1b,    // R1b type response
    R2,     // R2 type response
    R3,     // R3 type response 
    R7      // R7 type response 
}RESP;

// Summary: SD card command data structure
// Description: The typMMC_CMD structure is used to create a command table of information needed for each relevant SD command
typedef struct
{
    BYTE      CmdCode;          // The command code
    BYTE      CRC;              // The CRC value for that command
    RESP    responsetype;       // The response type
    BYTE    moredataexpected;   // Set to MOREDATA or NODATA, depending on whether more data is expected or not
} typMMC_CMD;


// Summary: An SD command packet
// Description: This union represents different ways to access an SD card command packet
typedef union
{
    // This structure allows array-style access of command bytes
    struct
    {
        #ifdef __18CXX
            BYTE field[6];      // BYTE array
        #else
            BYTE field[7];
        #endif
    };
    // This structure allows byte-wise access of packet command bytes
    struct
    {
        BYTE crc;               // The CRC byte
        #if defined __C30__
            BYTE c30filler;     // Filler space (since bitwise declarations can't cross a WORD boundary)
        #elif defined __C32__
            BYTE c32filler[3];  // Filler space (since bitwise declarations can't cross a DWORD boundary)
        #endif
        
        BYTE addr0;             // Address byte 0
        BYTE addr1;             // Address byte 1
        BYTE addr2;             // Address byte 2
        BYTE addr3;             // Address byte 3
        BYTE cmd;               // Command code byte
    };
    // This structure allows bitwise access to elements of the command bytes
    struct
    {
        BYTE  END_BIT:1;        // Packet end bit
        BYTE  CRC7:7;           // CRC value
        DWORD     address;      // Address
        BYTE  CMD_INDEX:6;      // Command code
        BYTE  TRANSMIT_BIT:1;   // Transmit bit
        BYTE  START_BIT:1;      // Packet start bit
    };
} CMD_PACKET;


// Summary: The format of an R1 type response
// Description: This union represents different ways to access an SD card R1 type response packet.
typedef union
{
    BYTE _byte;                         // Byte-wise access
    // This structure allows bitwise access of the response
    struct
    {
        unsigned IN_IDLE_STATE:1;       // Card is in idle state
        unsigned ERASE_RESET:1;         // Erase reset flag
        unsigned ILLEGAL_CMD:1;         // Illegal command flag
        unsigned CRC_ERR:1;             // CRC error flag
        unsigned ERASE_SEQ_ERR:1;       // Erase sequence error flag
        unsigned ADDRESS_ERR:1;         // Address error flag
        unsigned PARAM_ERR:1;           // Parameter flag   
        unsigned B7:1;                  // Unused bit 7
    };
} RESPONSE_1;

// Summary: The format of an R2 type response
// Description: This union represents different ways to access an SD card R2 type response packet
typedef union
{
    WORD _word;
    struct
    {
        BYTE      _byte0;
        BYTE      _byte1;
    };
    struct
    {
        unsigned IN_IDLE_STATE:1;
        unsigned ERASE_RESET:1;
        unsigned ILLEGAL_CMD:1;
        unsigned CRC_ERR:1;
        unsigned ERASE_SEQ_ERR:1;
        unsigned ADDRESS_ERR:1;
        unsigned PARAM_ERR:1;
        unsigned B7:1;
        unsigned CARD_IS_LOCKED:1;
        unsigned WP_ERASE_SKIP_LK_FAIL:1;
        unsigned ERROR:1;
        unsigned CC_ERROR:1;
        unsigned CARD_ECC_FAIL:1;
        unsigned WP_VIOLATION:1;
        unsigned ERASE_PARAM:1;
        unsigned OUTRANGE_CSD_OVERWRITE:1;
    };
} RESPONSE_2;

// Summary: The format of an R7 or R3 type response
// Description: This union represents different ways to access an SD card R7 type response packet.
typedef union
{
    struct
    {
        BYTE _byte;                         // Byte-wise access
        union
        {
            //Note: The SD card argument response field is 32-bit, big endian format.
            //However, the C compiler stores 32-bit values in little endian in RAM.
            //When writing to the _returnVal/argument bytes, make sure to byte
            //swap the order from which it arrived over the SPI from the SD card.
            DWORD _returnVal;
            struct
            {
                BYTE _byte0;
                BYTE _byte1;
                BYTE _byte2;
                BYTE _byte3;
            };    
        }argument;    
    } bytewise;
    // This structure allows bitwise access of the response
    struct
    {
        struct
        {
            unsigned IN_IDLE_STATE:1;       // Card is in idle state
            unsigned ERASE_RESET:1;         // Erase reset flag
            unsigned ILLEGAL_CMD:1;         // Illegal command flag
            unsigned CRC_ERR:1;             // CRC error flag
            unsigned ERASE_SEQ_ERR:1;       // Erase sequence error flag
            unsigned ADDRESS_ERR:1;         // Address error flag
            unsigned PARAM_ERR:1;           // Parameter flag   
            unsigned B7:1;                  // Unused bit 7
        }bits;
        DWORD _returnVal;
    } bitwise;
} RESPONSE_7;

// Summary: A union of responses from an SD card
// Description: The MMC_RESPONSE union represents any of the possible responses that an SD card can return after
//              being issued a command.
typedef union
{
    RESPONSE_1  r1;  
    RESPONSE_2  r2;
    RESPONSE_7  r7;
}MMC_RESPONSE;


// Summary: A description of the card specific data register
// Description: This union represents different ways to access information in a packet with SD card CSD informaiton.  For more
//              information on the CSD register, consult an SD card user's manual.
typedef union
{
    struct
    {
        DWORD _u320;
        DWORD _u321;
        DWORD _u322;
        DWORD _u323;
    };
    struct
    {
        BYTE _byte[16];
    };
    struct
    {
        unsigned NOT_USED           :1;
        unsigned CRC                :7;
        unsigned ECC                :2;
        unsigned FILE_FORMAT        :2;
        unsigned TMP_WRITE_PROTECT  :1;
        unsigned PERM_WRITE_PROTECT :1;
        unsigned COPY               :1;
        unsigned FILE_FORMAT_GRP    :1;
        unsigned RESERVED_1         :5;
        unsigned WRITE_BL_PARTIAL   :1;
        unsigned WRITE_BL_LEN_L     :2;
        unsigned WRITE_BL_LEN_H     :2;
        unsigned R2W_FACTOR         :3;
        unsigned DEFAULT_ECC        :2;
        unsigned WP_GRP_ENABLE      :1;
        unsigned WP_GRP_SIZE        :5;
        unsigned ERASE_GRP_SIZE_L   :3;
        unsigned ERASE_GRP_SIZE_H   :2;
        unsigned SECTOR_SIZE        :5;
        unsigned C_SIZE_MULT_L      :1;
        unsigned C_SIZE_MULT_H      :2;
        unsigned VDD_W_CURR_MAX     :3;
        unsigned VDD_W_CUR_MIN      :3;
        unsigned VDD_R_CURR_MAX     :3;
        unsigned VDD_R_CURR_MIN     :3;
        unsigned C_SIZE_L           :2;
        unsigned C_SIZE_H           :8;
        unsigned C_SIZE_U           :2;
        unsigned RESERVED_2         :2;
        unsigned DSR_IMP            :1;
        unsigned READ_BLK_MISALIGN  :1;
        unsigned WRITE_BLK_MISALIGN :1;
        unsigned READ_BL_PARTIAL    :1;
        unsigned READ_BL_LEN        :4;
        unsigned CCC_L              :4;
        unsigned CCC_H              :8;
        unsigned TRAN_SPEED         :8;
        unsigned NSAC               :8;
        unsigned TAAC               :8;
        unsigned RESERVED_3         :2;
        unsigned SPEC_VERS          :4;
        unsigned CSD_STRUCTURE      :2;
    };
} CSD;


// Summary: A description of the card information register
// Description: This union represents different ways to access information in a packet with SD card CID register informaiton.  For more
//              information on the CID register, consult an SD card user's manual.
typedef union
{
    struct
    {
        DWORD _u320;
        DWORD _u321;
        DWORD _u322;
        DWORD _u323;
    };
    struct
    {
        BYTE _byte[16];
    };
    struct
    {
        unsigned    NOT_USED            :1;
        unsigned    CRC                 :7;
        unsigned    MDT                 :8;
        DWORD       PSN;
        unsigned    PRV                 :8;
        char        PNM[6];
        WORD        OID;
        unsigned    MID                 :8;
    };
} CID;

#ifndef FALSE
    #define FALSE   0
#endif
#ifndef TRUE
    #define TRUE    !FALSE
#endif

#define INPUT   1
#define OUTPUT  0


// Description: A delay prescaler
#define DELAY_PRESCALER   (BYTE)      8

// Description: An approximation of the number of cycles per delay loop of overhead
#define DELAY_OVERHEAD    (BYTE)      5

// Description: An approximate calculation of how many times to loop to delay 1 ms in the Delayms function
#define MILLISECDELAY   (WORD)      ((GetInstructionClock()/DELAY_PRESCALER/(WORD)1000) - DELAY_OVERHEAD)


// Desription: Media Response Delay Timeouts 
#define NCR_TIMEOUT     (WORD)20        //Byte times before command response is expected (must be at least 8)
#define NAC_TIMEOUT     (DWORD)0x40000  //SPI byte times we should wait when performing read operations (should be at least 100ms for SD cards)
#define WRITE_TIMEOUT   (DWORD)0xA0000  //SPI byte times to wait before timing out when the media is performing a write operation (should be at least 250ms for SD cards).

// Summary: An enumeration of SD commands
// Description: This enumeration corresponds to the position of each command in the sdmmc_cmdtable array
//              These macros indicate to the SendMMCCmd function which element of the sdmmc_cmdtable array
//              to retrieve command code information from.
typedef enum
{
    GO_IDLE_STATE,
    SEND_OP_COND,
    SEND_IF_COND,
    SEND_CSD,
    SEND_CID,
    STOP_TRANSMISSION,
    SEND_STATUS,
    SET_BLOCKLEN,
    READ_SINGLE_BLOCK,
    READ_MULTI_BLOCK,
    WRITE_SINGLE_BLOCK,
    WRITE_MULTI_BLOCK,
    TAG_SECTOR_START,
    TAG_SECTOR_END,
    ERASE,
    APP_CMD,
    READ_OCR,
    CRC_ON_OFF,
    SD_SEND_OP_COND,
    SET_WR_BLK_ERASE_COUNT
}sdmmc_cmd;


#define SD_MODE_NORMAL  0
#define SD_MODE_HC      1


//Definition for a structure used when calling either MDD_SDSPI_AsyncReadTasks() 
//function, or the MDD_SDSPI_AsyncWriteTasks() function.
typedef struct
{
    WORD wNumBytes;         //Number of bytes to attempt to read or write in the next call to MDD_SDSPI_AsyncReadTasks() or MDD_SDSPI_AsyncWriteTasks.  May be updated between calls to the handler.
    DWORD dwBytesRemaining; //Should be initialized to the total number of bytes that you wish to read or write.  This value is allowed to be greater than a single block size of the media.
    BYTE* pBuffer;          //Pointer to where the read/written bytes should be copied to/from.  May be updated between calls to the handler function.
    DWORD dwAddress;        //Starting block address to read or to write to on the media.  Should only get initialized, do not modify after that.
    BYTE bStateVariable;    //State machine variable.  Should get initialized to ASYNC_READ_QUEUED or ASYNC_WRITE_QUEUED to start an operation.  After that, do not modify until the read or write is complete.
}ASYNC_IO;   


//Response codes for the MDD_SDSPI_AsyncReadTasks() function.
#define ASYNC_READ_COMPLETE             0x00
#define ASYNC_READ_BUSY                 0x01
#define ASYNC_READ_NEW_PACKET_READY     0x02
#define ASYNC_READ_ERROR                0xFF

//MDD_SDSPI_AsyncReadTasks() state machine variable values.  These are used internally to SD-SPI.c.
#define ASYNC_READ_COMPLETE             0x00
#define ASYNC_READ_QUEUED               0x01    //Initialize to this to start a read sequence
#define ASYNC_READ_WAIT_START_TOKEN     0x03
#define ASYNC_READ_NEW_PACKET_READY     0x02
#define ASYNC_READ_ABORT                0xFE
#define ASYNC_READ_ERROR                0xFF

//Possible return values when calling MDD_SDSPI_AsyncWriteTasks()
#define ASYNC_WRITE_COMPLETE        0x00
#define ASYNC_WRITE_SEND_PACKET     0x02
#define ASYNC_WRITE_BUSY            0x03
#define ASYNC_WRITE_ERROR           0xFF

//MDD_SDSPI_AsyncWriteTasks() state machine variable values.  These are used internally to SD-SPI.c.
#define ASYNC_WRITE_COMPLETE            0x00
#define ASYNC_WRITE_QUEUED              0x01    //Initialize to this to start a write sequence
#define ASYNC_WRITE_TRANSMIT_PACKET     0x02
#define ASYNC_WRITE_MEDIA_BUSY          0x03
#define ASYNC_STOP_TOKEN_SENT_WAIT_BUSY 0x04
#define ASYNC_WRITE_ABORT               0xFE
#define ASYNC_WRITE_ERROR               0xFF


//Constants
#define MEDIA_BLOCK_SIZE            512u  //Should always be 512 for v1 and v2 devices.
#define WRITE_RESPONSE_TOKEN_MASK   0x1F  //Bit mask to AND with the write token response byte from the media, to clear the don't care bits.



/***************************************************************************/
/*                               Macros                                    */
/***************************************************************************/

// Description: A macro to send clock cycles to dummy-read the CRC
#define mReadCRC()              WriteSPIM(0xFF);WriteSPIM(0xFF);

// Description: A macro to send clock cycles to dummy-write the CRC
#define mSendCRC()              WriteSPIM(0xFF);WriteSPIM(0xFF);

// Description: A macro to send 8 clock cycles for SD timing requirements
#define mSend8ClkCycles()       WriteSPIM(0xFF);

/*****************************************************************************/
/*                                 Public Prototypes                         */
/*****************************************************************************/

//These are the public API functions provided by SD-SPI.c
BYTE MDD_SDSPI_MediaDetect(void);
MEDIA_INFORMATION * MDD_SDSPI_MediaInitialize(void);
DWORD MDD_SDSPI_ReadCapacity(void);
WORD MDD_SDSPI_ReadSectorSize(void);
void MDD_SDSPI_InitIO(void);
BYTE MDD_SDSPI_SectorRead(DWORD sector_addr, BYTE* buffer);
BYTE MDD_SDSPI_SectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero);
BYTE MDD_SDSPI_AsyncReadTasks(ASYNC_IO*);
BYTE MDD_SDSPI_AsyncWriteTasks(ASYNC_IO*);
BYTE MDD_SDSPI_WriteProtectState(void);
BYTE MDD_SDSPI_ShutdownMedia(void);


#if defined __C30__ || defined __C32__
    extern BYTE ReadByte( BYTE* pBuffer, WORD index );
    extern WORD ReadWord( BYTE* pBuffer, WORD index );
    extern DWORD ReadDWord( BYTE* pBuffer, WORD index );
#endif

#endif
