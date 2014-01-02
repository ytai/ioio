/******************************************************************************
 *
 *               Microchip Memory Disk Drive File System
 *
 ******************************************************************************
 * FileName:        SD-SPI.c
 * Dependencies:    SD-SPI.h
 *                  string.h
 *                  FSIO.h
 *                  FSDefs.h
 * Processor:       PIC18/PIC24/dsPIC30/dsPIC33/PIC32
 * Compiler:        C18/C30/C32
 * Company:         Microchip Technology, Inc.
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
*****************************************************************************
 File Description:

 Change History:
  Rev     Description
  -----   -----------
  1.2.5   Fixed bug in the calculation of the capacity for v1.0 devices
  1.3.0   Improved media initialization sequence, for better card compatibility
          (especially with SDHC cards).
          Implemented SPI optimizations for data transfer rate improvement.
          Added new MDD_SDSPI_AsyncReadTasks() and MDD_SDSPI_AsyncWriteTasks() 
          API functions.  These are non-blocking, state machine based read/write
          handlers capable of considerably improved data throughput, particularly
          for multi-block reads and multi-block writes.
  1.3.2   Modified MDD_SDSPI_AsyncWriteTasks() so pre-erase command only gets
          used for multi-block write scenarios.   
  1.3.4   1) Added support for dsPIC33E & PIC24E controllers.
          2) #include "HardwareProfile.h" is moved up in the order.
          3) "SPI_INTERRUPT_FLAG_ASM" macro has to be defined in "HardwareProfile.h" file
             for PIC18 microcontrollers.Or else an error is generated while building
             the code.
                       "#define SPI_INTERRUPT_FLAG_ASM  PIR1, 3" is removed from SD-SPI.c
          4) Replaced "__C30" usage with "__C30__" .
  1.3.6   1) Modified "FSConfig.h" to "FSconfig.h" in '#include' directive.
          2) Moved 'spiconvalue' variable definition to only C30 usage, as C32
             is not using it.
          3) Modified 'MDD_SDSPI_MediaDetect' function to ensure that CMD0 is sent freshly
             after CS is asserted low. This minimizes the risk of SPI clock pulse master/slave
             syncronization problems.

********************************************************************/

#include "Compiler.h"
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "FSIO.h"
#include "FSDefs.h"
#include "SD-SPI.h"
#include <string.h>
#include "FSconfig.h"

/******************************************************************************
 * Global Variables
 *****************************************************************************/

// Description:  Used for the mass-storage library to determine capacity
DWORD MDD_SDSPI_finalLBA;
WORD gMediaSectorSize;
BYTE gSDMode;
static MEDIA_INFORMATION mediaInformation;
static ASYNC_IO ioInfo; //Declared global context, for fast/code efficient access


#ifdef __18CXX
    // Summary: Table of SD card commands and parameters
    // Description: The sdmmc_cmdtable contains an array of SD card commands, the corresponding CRC code, the
    //              response type that the card will return, and a parameter indicating whether to expect
    //              additional data from the card.
    const rom typMMC_CMD sdmmc_cmdtable[] =
#else
    const typMMC_CMD sdmmc_cmdtable[] =
#endif
{
    // cmd                      crc     response
    {cmdGO_IDLE_STATE,          0x95,   R1,     NODATA},
    {cmdSEND_OP_COND,           0xF9,   R1,     NODATA},
    {cmdSEND_IF_COND,      		0x87,   R7,     NODATA},
    {cmdSEND_CSD,               0xAF,   R1,     MOREDATA},
    {cmdSEND_CID,               0x1B,   R1,     MOREDATA},
    {cmdSTOP_TRANSMISSION,      0xC3,   R1b,    NODATA},
    {cmdSEND_STATUS,            0xAF,   R2,     NODATA},
    {cmdSET_BLOCKLEN,           0xFF,   R1,     NODATA},
    {cmdREAD_SINGLE_BLOCK,      0xFF,   R1,     MOREDATA},
    {cmdREAD_MULTI_BLOCK,       0xFF,   R1,     MOREDATA},
    {cmdWRITE_SINGLE_BLOCK,     0xFF,   R1,     MOREDATA},
    {cmdWRITE_MULTI_BLOCK,      0xFF,   R1,     MOREDATA}, 
    {cmdTAG_SECTOR_START,       0xFF,   R1,     NODATA},
    {cmdTAG_SECTOR_END,         0xFF,   R1,     NODATA},
    {cmdERASE,                  0xDF,   R1b,    NODATA},
    {cmdAPP_CMD,                0x73,   R1,     NODATA},
    {cmdREAD_OCR,               0x25,   R7,     NODATA},
    {cmdCRC_ON_OFF,             0x25,   R1,     NODATA},
    {cmdSD_SEND_OP_COND,        0xFF,   R7,     NODATA}, //Actual response is R3, but has same number of bytes as R7.
    {cmdSET_WR_BLK_ERASE_COUNT, 0xFF,   R1,     NODATA}
};




/******************************************************************************
 * Prototypes
 *****************************************************************************/
extern void Delayms(BYTE milliseconds);
BYTE MDD_SDSPI_ReadMedia(void);
MEDIA_INFORMATION * MDD_SDSPI_MediaInitialize(void);
MMC_RESPONSE SendMMCCmd(BYTE cmd, DWORD address);

#if defined __C30__ || defined __C32__
    void OpenSPIM ( unsigned int sync_mode);
    void CloseSPIM( void );
    unsigned char WriteSPIM( unsigned char data_out );
#elif defined __18CXX
    void OpenSPIM ( unsigned char sync_mode);
    void CloseSPIM( void );
    unsigned char WriteSPIM( unsigned char data_out );

    unsigned char WriteSPIManual(unsigned char data_out);
    BYTE ReadMediaManual (void);
    MMC_RESPONSE SendMMCCmdManual(BYTE cmd, DWORD address);
#endif
void InitSPISlowMode(void);

#if defined __18CXX
//Private function prototypes
static void PIC18_Optimized_SPI_Write_Packet(void);
static void PIC18_Optimized_SPI_Read_Packet(void);
#endif

//-------------Function name redirects------------------------------------------
//During the media initialization sequence, it is
//necessary to clock the media at no more than 400kHz SPI speeds, since some
//media types power up in open drain output mode and cannot run fast initially.
//On PIC18 devices, when the CPU is run at full frequency, the standard SPI 
//prescalars cannot reach a low enough SPI frequency.  Therefore, we initialize
//the card at low speed using bit-banged SPI on PIC18 devices.  On 
//PIC32/PIC24/dsPIC devices, the SPI module is flexible enough to reach <400kHz
//speeds regardless of CPU frequency, and therefore bit-banged code is not 
//necessary.  Therefore, we use function redirects where necessary, to point to
//the proper SPI related code, given the processor type.

#if defined __18CXX
    #define SendMediaSlowCmd    SendMMCCmdManual
    #define WriteSPISlow        WriteSPIManual
#else
    #define SendMediaSlowCmd    SendMMCCmd
    #define WriteSPISlow        WriteSPIM
#endif
//------------------------------------------------------------------------------


#ifdef __PIC32MX__
/*********************************************************
  Function:
    static inline __attribute__((always_inline)) unsigned char SPICacutateBRG (unsigned int pb_clk, unsigned int spi_clk)
  Summary:
    Calculate the PIC32 SPI BRG value
  Conditions:
    None
  Input:
    pb_clk -  The value of the PIC32 peripheral clock
    spi_clk - The desired baud rate
  Return:
    The corresponding BRG register value.
  Side Effects:
    None.
  Description:
    The SPICalutateBRG function is used to determine an appropriate BRG register value for the PIC32 SPI module.
  Remarks:
    None                                                  
  *********************************************************/

static inline __attribute__((always_inline)) unsigned char SPICalutateBRG(unsigned int pb_clk, unsigned int spi_clk)
{
    unsigned int brg;

    brg = pb_clk / (2 * spi_clk);

    if(pb_clk % (2 * spi_clk))
        brg++;

    if(brg > 0x100)
        brg = 0x100;

    if(brg)
        brg--;

    return (unsigned char) brg;
}
#endif


/*********************************************************
  Function:
    BYTE MDD_SDSPI_MediaDetect
  Summary:
    Determines whether an SD card is present
  Conditions:
    The MDD_MediaDetect function pointer must be configured
    to point to this function in FSconfig.h
  Input:
    None
  Return Values:
    TRUE -  Card detected
    FALSE - No card detected
  Side Effects:
    None.
  Description:
    The MDD_SDSPI_MediaDetect function determine if an SD card is connected to 
    the microcontroller.
    If the MEDIA_SOFT_DETECT is not defined, the detection is done by polling
    the SD card detect pin.
    The MicroSD connector does not have a card detect pin, and therefore a
    software mechanism must be used. To do this, the SEND_STATUS command is sent 
    to the card. If the card is not answering with 0x00, the card is either not 
    present, not configured, or in an error state. If this is the case, we try
    to reconfigure the card. If the configuration fails, we consider the card not 
    present (it still may be present, but malfunctioning). In order to use the 
    software card detect mechanism, the MEDIA_SOFT_DETECT macro must be defined.
    
  Remarks:
    None                                                  
  *********************************************************/

BYTE MDD_SDSPI_MediaDetect (void)
{
#ifndef MEDIA_SOFT_DETECT
    return(!SD_CD);
#else
	MMC_RESPONSE    response;

    //First check if SPI module is enabled or not.
	if (SPIENABLE == 0)
	{
        unsigned char timeout;

		//If the SPI module is not enabled, then the media has evidently not
		//been initialized.  Try to send CMD0 and CMD13 to reset the device and
		//get it into SPI mode (if present), and then request the status of
		//the media.  If this times out, then the card is presumably not physically
		//present.
		
		InitSPISlowMode();
		
        //Send CMD0 to reset the media
	    //If the card is physically present, then we should get a valid response.
        timeout = 4;
        do
        {
            //Toggle chip select, to make media abandon whatever it may have been doing
            //before.  This ensures the CMD0 is sent freshly after CS is asserted low,
            //minimizing risk of SPI clock pulse master/slave syncronization problems, 
            //due to possible application noise on the SCK line.
            SD_CS = 1;
            WriteSPISlow(0xFF);   //Send some "extraneous" clock pulses.  If a previous
                                  //command was terminated before it completed normally,
                                  //the card might not have received the required clocking
                                  //following the transfer.
            SD_CS = 0;
            timeout--;
    
            //Send CMD0 to software reset the device
            response = SendMediaSlowCmd(GO_IDLE_STATE, 0x0);
        } while((response.r1._byte != 0x01) && (timeout != 0));

	    //Check if response was invalid (R1 response byte should be = 0x01 after GO_IDLE_STATE)
	    if(response.r1._byte != 0x01)
	    {
	        CloseSPIM();
	        return FALSE;
	    }    
	    else
	    {
    	    //Card is presumably present.  The SDI pin should have a pull up resistor on
    	    //it, so the odds of SDI "floating" to 0x01 after sending CMD0 is very
    	    //remote, unless the media is genuinely present.  Therefore, we should
    	    //try to perform a full card initialization sequence now.
    		MDD_SDSPI_MediaInitialize();    //Can block and take a long time to execute.
    		if(mediaInformation.errorCode == MEDIA_NO_ERROR)
    		{
    			/* if the card was initialized correctly, it means it is present */
    			return TRUE;
    		}
    		else 
    		{
        		CloseSPIM();
    			return FALSE;
    		}		

    	}    
	}//if (SPIENABLE == 0)
	else
	{
    	//The SPI module was already enabled.  This most likely means the media is
    	//present and has already been initialized.  However, it is possible that
    	//the user could have unplugged the media, in which case it is no longer
    	//present.  We should send it a command, to check the status.
    	response = SendMMCCmd(SEND_STATUS,0x0);
    	if((response.r2._word & 0xEC0C) != 0x0000)
	    {
    	    //The card didn't respond with the expected result.  This probably
    	    //means it is no longer present.  We can try to re-initialized it,
    	    //just to be doubly sure.
    		CloseSPIM();
    		MDD_SDSPI_MediaInitialize();    //Can block and take a long time to execute.
    		if(mediaInformation.errorCode == MEDIA_NO_ERROR)
    		{
    			/* if the card was initialized correctly, it means it is present */
    			return TRUE;
    		}
    		else 
    		{
        		CloseSPIM();
    			return FALSE;
    		}
    	}
    	else
    	{
        	//The CMD13 response to SEND_STATUS was valid.  This presumably
        	//means the card is present and working normally.
        	return TRUE;
        }   	    

	}

    //Should theoretically never execute to here.  All pathways should have 
    //already returned with the status.
    return TRUE;

#endif

}//end MediaDetect



/*********************************************************
  Function:
    WORD MDD_SDSPI_ReadSectorSize (void)
  Summary:
    Determines the current sector size on the SD card
  Conditions:
    MDD_MediaInitialize() is complete
  Input:
    None
  Return:
    The size of the sectors for the physical media
  Side Effects:
    None.
  Description:
    The MDD_SDSPI_ReadSectorSize function is used by the
    USB mass storage class to return the card's sector
    size to the PC on request.
  Remarks:
    None
  *********************************************************/

WORD MDD_SDSPI_ReadSectorSize(void)
{
    return gMediaSectorSize;
}


/*********************************************************
  Function:
    DWORD MDD_SDSPI_ReadCapacity (void)
  Summary:
    Determines the current capacity of the SD card
  Conditions:
    MDD_MediaInitialize() is complete
  Input:
    None
  Return:
    The capacity of the device
  Side Effects:
    None.
  Description:
    The MDD_SDSPI_ReadCapacity function is used by the
    USB mass storage class to return the total number
    of sectors on the card.
  Remarks:
    None
  *********************************************************/
DWORD MDD_SDSPI_ReadCapacity(void)
{
    return (MDD_SDSPI_finalLBA);
}


/*********************************************************
  Function:
    WORD MDD_SDSPI_InitIO (void)
  Summary:
    Initializes the I/O lines connected to the card
  Conditions:
    MDD_MediaInitialize() is complete.  The MDD_InitIO
    function pointer is pointing to this function.
  Input:
    None
  Return:
    None
  Side Effects:
    None.
  Description:
    The MDD_SDSPI_InitIO function initializes the I/O
    pins connected to the SD card.
  Remarks:
    None
  *********************************************************/

void MDD_SDSPI_InitIO (void)
{
    // Turn off the card
//    SD_CD_TRIS = INPUT;            //Card Detect - input
    SD_CS = 1;                     //Initialize Chip Select line
    SD_CS_TRIS = OUTPUT;           //Card Select - output
//    SD_WE_TRIS = INPUT;            //Write Protect - input

#if defined	(__dsPIC33E__) || defined (__PIC24E__)
    SD_CS_ANSEL = 0;
    SD_SCK_ANSEL = 0;
    SD_SDI_ANSEL = 0;
    SD_SDO_ANSEL = 0;
#endif    
}



/*********************************************************
  Function:
    BYTE MDD_SDSPI_ShutdownMedia (void)
  Summary:
    Disables the SD card
  Conditions:
    The MDD_ShutdownMedia function pointer is pointing 
    towards this function.
  Input:
    None
  Return:
    None
  Side Effects:
    None.
  Description:
    This function will disable the SPI port and deselect
    the SD card.
  Remarks:
    None
  *********************************************************/

BYTE MDD_SDSPI_ShutdownMedia(void)
{
    // close the spi bus
    CloseSPIM();
    
    // deselect the device
    SD_CS = 1;

    return 0;
}


/*****************************************************************************
  Function:
    MMC_RESPONSE SendMMCCmd (BYTE cmd, DWORD address)
  Summary:
    Sends a command packet to the SD card.
  Conditions:
    None.
  Input:
    None.
  Return Values:
    MMC_RESPONSE    - The response from the card
                    - Bit 0 - Idle state
                    - Bit 1 - Erase Reset
                    - Bit 2 - Illegal Command
                    - Bit 3 - Command CRC Error
                    - Bit 4 - Erase Sequence Error
                    - Bit 5 - Address Error
                    - Bit 6 - Parameter Error
                    - Bit 7 - Unused. Always 0.
  Side Effects:
    None.
  Description:
    SendMMCCmd prepares a command packet and sends it out over the SPI interface.
    Response data of type 'R1' (as indicated by the SD/MMC product manual is returned.
  Remarks:
    None.
  *****************************************************************************/

MMC_RESPONSE SendMMCCmd(BYTE cmd, DWORD address)
{
    MMC_RESPONSE    response;
    CMD_PACKET  CmdPacket;
    WORD timeout;
    DWORD longTimeout;
    
    SD_CS = 0;                           //Select card
    
    // Copy over data
    CmdPacket.cmd        = sdmmc_cmdtable[cmd].CmdCode;
    CmdPacket.address    = address;
    CmdPacket.crc        = sdmmc_cmdtable[cmd].CRC;       // Calc CRC here
    
    CmdPacket.TRANSMIT_BIT = 1;             //Set Tranmission bit
    
    WriteSPIM(CmdPacket.cmd);                //Send Command
    WriteSPIM(CmdPacket.addr3);              //Most Significant Byte
    WriteSPIM(CmdPacket.addr2);
    WriteSPIM(CmdPacket.addr1);
    WriteSPIM(CmdPacket.addr0);              //Least Significant Byte
    WriteSPIM(CmdPacket.crc);                //Send CRC

    //Special handling for CMD12 (STOP_TRANSMISSION).  The very first byte after
    //sending the command packet may contain bogus non-0xFF data.  This 
    //"residual data" byte should not be interpreted as the R1 response byte.
    if(cmd == STOP_TRANSMISSION)
    {
        MDD_SDSPI_ReadMedia(); //Perform dummy read to fetch the residual non R1 byte
    } 

    //Loop until we get a response from the media.  Delay (NCR) could be up 
    //to 8 SPI byte times.  First byte of response is always the equivalent of 
    //the R1 byte, even for R1b, R2, R3, R7 responses.
    timeout = NCR_TIMEOUT;
    do
    {
        response.r1._byte = MDD_SDSPI_ReadMedia();
        timeout--;
    }while((response.r1._byte == MMC_FLOATING_BUS) && (timeout != 0));
    
    //Check if we should read more bytes, depending upon the response type expected.  
    if(sdmmc_cmdtable[cmd].responsetype == R2)
    {
        response.r2._byte1 = response.r1._byte; //We already received the first byte, just make sure it is in the correct location in the struct.
        response.r2._byte0 = MDD_SDSPI_ReadMedia(); //Fetch the second byte of the response.
    }
    else if(sdmmc_cmdtable[cmd].responsetype == R1b)
    {
        //Keep trying to read from the media, until it signals it is no longer
        //busy.  It will continuously send 0x00 bytes until it is not busy.
        //A non-zero value means it is ready for the next command.
        //The R1b response is received after a CMD12 STOP_TRANSMISSION
        //command, where the media card may be busy writing its internal buffer
        //to the flash memory.  This can typically take a few milliseconds, 
        //with a recommended maximum timeout of 250ms or longer for SD cards.
        longTimeout = WRITE_TIMEOUT;
        do
        {
            response.r1._byte = MDD_SDSPI_ReadMedia();
            longTimeout--;
        }while((response.r1._byte == 0x00) && (longTimeout != 0));

        response.r1._byte = 0x00;
    }
    else if (sdmmc_cmdtable[cmd].responsetype == R7) //also used for response R3 type
    {
        //Fetch the other four bytes of the R3 or R7 response.
        //Note: The SD card argument response field is 32-bit, big endian format.
        //However, the C compiler stores 32-bit values in little endian in RAM.
        //When writing to the _returnVal/argument bytes, make sure the order it 
        //gets stored in is correct.      
        response.r7.bytewise.argument._byte3 = MDD_SDSPI_ReadMedia();
        response.r7.bytewise.argument._byte2 = MDD_SDSPI_ReadMedia();
        response.r7.bytewise.argument._byte1 = MDD_SDSPI_ReadMedia();
        response.r7.bytewise.argument._byte0 = MDD_SDSPI_ReadMedia();
    }

    WriteSPIM(0xFF);    //Device requires at least 8 clock pulses after 
                             //the response has been sent, before if can process
                             //the next command.  CS may be high or low.

    // see if we are expecting more data or not
    if(!(sdmmc_cmdtable[cmd].moredataexpected))
        SD_CS = 1;

    return(response);
}

#ifdef __18CXX
/*****************************************************************************
  Function:
    MMC_RESPONSE SendMMCCmdManual (BYTE cmd, DWORD address)
  Summary:
    Sends a command packet to the SD card with bit-bang SPI.
  Conditions:
    None.
  Input:
    Need input cmd index into a rom table of implemented commands.
    Also needs 4 bytes of data as address for some commands (also used for
    other purposes in other commands).
  Return Values:
    Assuming an "R1" type of response, each bit will be set depending upon status:
    MMC_RESPONSE    - The response from the card
                    - Bit 0 - Idle state
                    - Bit 1 - Erase Reset
                    - Bit 2 - Illegal Command
                    - Bit 3 - Command CRC Error
                    - Bit 4 - Erase Sequence Error
                    - Bit 5 - Address Error
                    - Bit 6 - Parameter Error
                    - Bit 7 - Unused. Always 0.
    Other response types (ex: R3/R7) have up to 5 bytes of response.  The first
    byte is always the same as the R1 response.  The contents of the other bytes 
    depends on the command.
  Side Effects:
    None.
  Description:
    SendMMCCmd prepares a command packet and sends it out over the SPI interface.
    Response data of type 'R1' (as indicated by the SD/MMC product manual is returned.
    This function is intended to be used when the clock speed of a PIC18 device is
    so high that the maximum SPI divider can't reduce the clock below the maximum
    SD card initialization sequence speed.
  Remarks:
    None.
  ***************************************************************************************/
MMC_RESPONSE SendMMCCmdManual(BYTE cmd, DWORD address)
{
    BYTE index;
    MMC_RESPONSE    response;
    CMD_PACKET  CmdPacket;
    WORD timeout;
    
    SD_CS = 0;                           //Select card
    
    // Copy over data
    CmdPacket.cmd        = sdmmc_cmdtable[cmd].CmdCode;
    CmdPacket.address    = address;
    CmdPacket.crc        = sdmmc_cmdtable[cmd].CRC;       // Calc CRC here
    
    CmdPacket.TRANSMIT_BIT = 1;             //Set Tranmission bit
    
    WriteSPIManual(CmdPacket.cmd);                //Send Command
    WriteSPIManual(CmdPacket.addr3);              //Most Significant Byte
    WriteSPIManual(CmdPacket.addr2);
    WriteSPIManual(CmdPacket.addr1);
    WriteSPIManual(CmdPacket.addr0);              //Least Significant Byte
    WriteSPIManual(CmdPacket.crc);                //Send CRC

    //Special handling for CMD12 (STOP_TRANSMISSION).  The very first byte after
    //sending the command packet may contain bogus non-0xFF data.  This 
    //"residual data" byte should not be interpreted as the R1 response byte.
    if(cmd == STOP_TRANSMISSION)
    {
        ReadMediaManual(); //Perform dummy read to fetch the residual non R1 byte
    } 

    //Loop until we get a response from the media.  Delay (NCR) could be up 
    //to 8 SPI byte times.  First byte of response is always the equivalent of 
    //the R1 byte, even for R1b, R2, R3, R7 responses.
    timeout = NCR_TIMEOUT;
    do
    {
        response.r1._byte = ReadMediaManual();
        timeout--;
    }while((response.r1._byte == MMC_FLOATING_BUS) && (timeout != 0));
    
    
    //Check if we should read more bytes, depending upon the response type expected.  
    if(sdmmc_cmdtable[cmd].responsetype == R2)
    {
        response.r2._byte1 = response.r1._byte; //We already received the first byte, just make sure it is in the correct location in the struct.
        response.r2._byte0 = ReadMediaManual(); //Fetch the second byte of the response.
    }
    else if(sdmmc_cmdtable[cmd].responsetype == R1b)
    {
        //Keep trying to read from the media, until it signals it is no longer
        //busy.  It will continuously send 0x00 bytes until it is not busy.
        //A non-zero value means it is ready for the next command.
        timeout = 0xFFFF;
        do
        {
            response.r1._byte = ReadMediaManual();
            timeout--;
        }while((response.r1._byte == 0x00) && (timeout != 0));

        response.r1._byte = 0x00;
    }
    else if (sdmmc_cmdtable[cmd].responsetype == R7) //also used for response R3 type
    {
        //Fetch the other four bytes of the R3 or R7 response.
        //Note: The SD card argument response field is 32-bit, big endian format.
        //However, the C compiler stores 32-bit values in little endian in RAM.
        //When writing to the _returnVal/argument bytes, make sure the order it 
        //gets stored in is correct.      
        response.r7.bytewise.argument._byte3 = ReadMediaManual();
        response.r7.bytewise.argument._byte2 = ReadMediaManual();
        response.r7.bytewise.argument._byte1 = ReadMediaManual();
        response.r7.bytewise.argument._byte0 = ReadMediaManual();
    }

    WriteSPIManual(0xFF);    //Device requires at least 8 clock pulses after 
                             //the response has been sent, before if can process
                             //the next command.  CS may be high or low.

    // see if we are expecting more data or not
    if(!(sdmmc_cmdtable[cmd].moredataexpected))
        SD_CS = 1;

    return(response);
}
#endif



/*****************************************************************************
  Function:
    BYTE MDD_SDSPI_SectorRead (DWORD sector_addr, BYTE * buffer)
  Summary:
    Reads a sector of data from an SD card.
  Conditions:
    The MDD_SectorRead function pointer must be pointing towards this function.
  Input:
    sector_addr - The address of the sector on the card.
    buffer -      The buffer where the retrieved data will be stored.  If
                  buffer is NULL, do not store the data anywhere.
  Return Values:
    TRUE -  The sector was read successfully
    FALSE - The sector could not be read
  Side Effects:
    None
  Description:
    The MDD_SDSPI_SectorRead function reads a sector of data bytes (512 bytes) 
    of data from the SD card starting at the sector address and stores them in 
    the location pointed to by 'buffer.'
  Remarks:
    The card expects the address field in the command packet to be a byte address.
    The sector_addr value is converted to a byte address by shifting it left nine
    times (multiplying by 512).
    
    This function performs a synchronous read operation.  In other words, this
    function is a blocking function, and will not return until either the data
    has fully been read, or, a timeout or other error occurred.
  ***************************************************************************************/
BYTE MDD_SDSPI_SectorRead(DWORD sector_addr, BYTE* buffer)
{
    ASYNC_IO info;
    BYTE status;
    
    //Initialize info structure for using the MDD_SDSPI_AsyncReadTasks() function.
    info.wNumBytes = 512;
    info.dwBytesRemaining = 512;
    info.pBuffer = buffer;
    info.dwAddress = sector_addr;
    info.bStateVariable = ASYNC_READ_QUEUED;
    
    //Blocking loop, until the state machine finishes reading the sector,
    //or a timeout or other error occurs.  MDD_SDSPI_AsyncReadTasks() will always
    //return either ASYNC_READ_COMPLETE or ASYNC_READ_FAILED eventually 
    //(could take awhile in the case of timeout), so this won't be a totally
    //infinite blocking loop.
    while(1)
    {
        status = MDD_SDSPI_AsyncReadTasks(&info);
        if(status == ASYNC_READ_COMPLETE)
        {
            return TRUE;
        }
        else if(status == ASYNC_READ_ERROR)
        {
            return FALSE;
        } 
    }       

    //Impossible to get here, but we will return a value anyay to avoid possible 
    //compiler warnings.
    return FALSE;
}    

 




/*****************************************************************************
  Function:
    BYTE MDD_SDSPI_AsyncReadTasks(ASYNC_IO* info)
  Summary:
    Speed optimized, non-blocking, state machine based read function that reads 
    data packets from the media, and copies them to a user specified RAM buffer.
  Pre-Conditions:
    The ASYNC_IO structure must be initialized correctly, prior to calling
    this function for the first time.  Certain parameters, such as the user
    data buffer pointer (pBuffer) in the ASYNC_IO struct are allowed to be changed
    by the application firmware, in between each call to MDD_SDSPI_AsyncReadTasks().
    Additionally, the media and microcontroller SPI module should have already 
    been initalized before using this function.  This function is mutually
    exclusive with the MDD_SDSPI_AsyncWriteTasks() function.  Only one operation
    (either one read or one write) is allowed to be in progress at a time, as
    both functions share statically allocated resources and monopolize the SPI bus.
  Input:
    ASYNC_IO* info -        A pointer to a ASYNC_IO structure.  The 
                            structure contains information about how to complete
                            the read operation (ex: number of total bytes to read,
                            where to copy them once read, maximum number of bytes
                            to return for each call to MDD_SDSPI_AsyncReadTasks(), etc.).
  Return Values:
    BYTE - Returns a status byte indicating the current state of the read 
            operation. The possible return values are:
            
            ASYNC_READ_BUSY - Returned when the state machine is busy waiting for
                             a data start token from the media.  The media has a
                             random access time, which can often be quite long
                             (<= ~3ms typ, with maximum of 100ms).  No data
                             has been copied yet in this case, and the application
                             should keep calling MDD_SDSPI_AsyncReadTasks() until either
                             an error/timeout occurs, or ASYNC_READ_NEW_PACKET_READY
                             is returned.
            ASYNC_READ_NEW_PACKET_READY -   Returned after a single packet, of
                                            the specified size (in info->numBytes),
                                            is ready to be read from the 
                                            media and copied to the user 
                                            specified data buffer.  Often, after
                                            receiving this return value, the 
                                            application firmware would want to
                                            update the info->pReceiveBuffer pointer
                                            before calling MDD_SDSPI_AsyncReadTasks()
                                            again.  This way, the application can
                                            begin fetching the next packet worth
                                            of data, while still using/consuming
                                            the previous packet of data.
            ASYNC_READ_COMPLETE - Returned when all data bytes in the read 
                                 operation have been read and returned successfully,
                                 and the media is now ready for the next operation.
            ASYNC_READ_ERROR - Returned when some failure occurs.  This could be
                               either due to a media timeout, or due to some other
                               unknown type of error.  In this case, the 
                               MDD_SDSPI_AsyncReadTasks() handler will terminate
                               the read attempt and will try to put the media 
                               back in a default state, ready for a new command.  
                               The application firmware may then retry the read
                               attempt (if desired) by re-initializing the 
                               ASYNC_IO structure and setting the 
                               bStateVariable = ASYNC_READ_QUEUED.

            
  Side Effects:
    Uses the SPI bus and the media.  The media and SPI bus should not be
    used by any other function until the read operation has either completed
    successfully, or returned with the ASYNC_READ_ERROR condition.
  Description:
    Speed optimized, non-blocking, state machine based read function that reads 
    data packets from the media, and copies them to a user specified RAM buffer.
    This function uses the multi-block read command (and stop transmission command) 
    to perform fast reads of data.  The total amount of data that will be returned 
    on any given call to MDD_SDSPI_AsyncReadTasks() will be the info->numBytes parameter.
    However, if the function is called repeatedly, with info->bytesRemaining set
    to a large number, this function can successfully fetch data sizes >> than
    the block size (theoretically anything up to ~4GB, since bytesRemaining is 
    a 32-bit DWORD).  The application firmware should continue calling 
    MDD_SDSPI_AsyncReadTasks(), until the ASYNC_READ_COMPLETE value is returned 
    (or ASYNC_READ_ERROR), even if it has already received all of the data expected.
    This is necessary, so the state machine can issue the CMD12 (STOP_TRANMISSION) 
    to terminate the multi-block read operation, once the total expected number 
    of bytes have been read.  This puts the media back into the default state 
    ready for a new command.
    
    During normal/successful operations, calls to MDD_SDSPI_AsyncReadTasks() 
    would typically return:
    1. ASYNC_READ_BUSY - repeatedly up to several milliseconds, then 
    2. ASYNC_READ_NEW_PACKET_READY - repeatedly, until 512 bytes [media read 
        block size] is received, then 
    3. Back to ASYNC_READ_BUSY (for awhile, may be short), then
    4. Back to ASYNC_READ_NEW_PACKET_READY (repeatedly, until the next 512 byte
       boundary, then back to #3, etc.
    5. After all data is received successfully, then the function will return 
       ASYNC_READ_COMPLETE, for all subsequent calls (until a new read operation
       is started, by re-initializing the ASYNC_IO structure, and re-calling
       the function).
    
  Remarks:
    This function will monopolize the SPI module during the operation.  Do not
    use the SPI module for any other purpose, while a fetch operation is in
    progress.  Additionally, the ASYNC_IO structure must not be modified
    in a different context, while the MDD_SDSPI_AsyncReadTasks() function is executing.
    In between calls to MDD_SDSPI_AsyncReadTasks(), certain parameters, namely the
    info->numBytes and info->pReceiveBuffer are allowed to change however.
    
    The bytesRemaining value must always be an exact integer multiple of numBytes 
    for the function to operate correctly.  Additionally, it is recommended, although
    not essential, for the bytesRemaining to be an integer multiple of the media
    read block size.
    
    When starting a read operation, the info->stateVariable must be initalized to
    ASYNC_READ_QUEUED.  All other fields in the info structure should also be
    initialized correctly.
    
    The info->wNumBytes must always be less than or equal to the media block size,
    (which is 512 bytes).  Additionally, info->wNumBytes must always be an exact 
    integer factor of the media block size (unless info->dwBytesRemaining is less
    than the media block size).  Example values that are allowed for info->wNumBytes
    are: 1, 2, 4, 8, 16, 32, 64, 128, 256, 512.
  *****************************************************************************/

BYTE MDD_SDSPI_AsyncReadTasks(ASYNC_IO* info)
{
    BYTE bData;
    MMC_RESPONSE response;
    static WORD blockCounter;
    static DWORD longTimeoutCounter;
    static BOOL SingleBlockRead;
    
    //Check what state we are in, to decide what to do.
    switch(info->bStateVariable)
    {
        case ASYNC_READ_COMPLETE:
            return ASYNC_READ_COMPLETE;
        case ASYNC_READ_QUEUED:
            //Start the read request.  
            
            //Initialize some variables we will use later.
            blockCounter = MEDIA_BLOCK_SIZE; //Counter will be used later for block boundary tracking
            ioInfo = *info; //Get local copy of structure, for quicker access with less code size

            //SDHC cards are addressed on a 512 byte block basis.  This is 1:1 equivalent
            //to LBA addressing.  For standard capacity media, the media is expecting
            //a complete byte address.  Therefore, to convert from the LBA address to the
            //byte address, we need to multiply by 512.
            if (gSDMode == SD_MODE_NORMAL)
            {
                ioInfo.dwAddress <<= 9; //Equivalent to multiply by 512
            }  
            if(ioInfo.dwBytesRemaining <= MEDIA_BLOCK_SIZE)
            {
                SingleBlockRead = TRUE;
                response = SendMMCCmd(READ_SINGLE_BLOCK, ioInfo.dwAddress);
            }    
            else
            {
                SingleBlockRead = FALSE;
                response = SendMMCCmd(READ_MULTI_BLOCK, ioInfo.dwAddress);
            }    
            //Note: SendMMCmd() sends 8 SPI clock cycles after getting the
            //response.  This meets the NAC min timing paramemter, so we don't
            //need additional clocking here.
            
            // Make sure the command was accepted successfully
            if(response.r1._byte != 0x00)
            {
                //Perhaps the card isn't initialized or present.
                info->bStateVariable = ASYNC_READ_ABORT;
                return ASYNC_READ_BUSY; 
            }
            
            //We successfully sent the READ_MULTI_BLOCK command to the media.
            //We now need to keep polling the media until it sends us the data
            //start token byte.
            longTimeoutCounter = NAC_TIMEOUT; //prepare timeout counter for next state
            info->bStateVariable = ASYNC_READ_WAIT_START_TOKEN;
            return ASYNC_READ_BUSY;
        case ASYNC_READ_WAIT_START_TOKEN:
            //In this case, we have already issued the READ_MULTI_BLOCK command
            //to the media, and we need to keep polling the media until it sends
            //us the data start token byte.  This could typically take a 
            //couple/few milliseconds, up to a maximum of 100ms.
            if(longTimeoutCounter != 0x00000000)
            {
                longTimeoutCounter--;
                bData = MDD_SDSPI_ReadMedia();
                
                if(bData != MMC_FLOATING_BUS)
                {
                    if(bData == DATA_START_TOKEN)
                    {   
                        //We got the start token.  Ready to receive the data
                        //block now.
                        info->bStateVariable = ASYNC_READ_NEW_PACKET_READY;
                        return ASYNC_READ_NEW_PACKET_READY;
                    }
                    else
                    {
                        //We got an unexpected non-0xFF, non-start token byte back?
                        //Some kind of error must have occurred. 
                        info->bStateVariable = ASYNC_READ_ABORT; 
                        return ASYNC_READ_BUSY;
                    }        
                }
                else
                {
                    //Media is still busy.  Start token not received yet.
                    return ASYNC_READ_BUSY;
                }                    
            } 
            else
            {
                //The media didn't respond with the start data token in the timeout
                //interval allowed.  Operation failed.  Abort the operation.
                info->bStateVariable = ASYNC_READ_ABORT; 
                return ASYNC_READ_BUSY;                
            }       
            //Should never execute to here
            
        case ASYNC_READ_NEW_PACKET_READY:
            //We have sent the READ_MULTI_BLOCK command and have successfully
            //received the data start token byte.  Therefore, we are ready
            //to receive raw data bytes from the media.
            if(ioInfo.dwBytesRemaining != 0x00000000)
            {
                //Re-update local copy of pointer and number of bytes to read in this
                //call.  These parameters are allowed to change between packets.
                ioInfo.wNumBytes = info->wNumBytes;
           	    ioInfo.pBuffer = info->pBuffer;
           	    
           	    //Update counters for state tracking and loop exit condition tracking.
                ioInfo.dwBytesRemaining -= ioInfo.wNumBytes;
                blockCounter -= ioInfo.wNumBytes;

                //Now read a ioInfo.wNumBytes packet worth of SPI bytes, 
                //and place the received bytes in the user specified pBuffer.
                //This operation directly dictates data thoroughput in the 
                //application, therefore optimized code should be used for each 
                //processor type.
            	#if defined __C30__ || defined __C32__
                {
                    //PIC24/dsPIC/PIC32 architecture is efficient with pointers.
                    //Therefore, this code can provide good SPI bus utilization, 
                    //provided the compiler optimization level is 's' or '3'.
                    BYTE* localPointer = ioInfo.pBuffer;
                    WORD localCounter = ioInfo.wNumBytes;
                    
                    if(localCounter != 0x0000)
                    {
                        localPointer--;
                        while(1)
                        {
                            SPIBUF = 0xFF;
                            localPointer++;
                            if((--localCounter) == 0x0000)
                            {
                               break; 
                            } 
                            while(!SPISTAT_RBF);
                            *localPointer = (BYTE)SPIBUF;
                        }
                        while(!SPISTAT_RBF);
                        *localPointer = (BYTE)SPIBUF;  
                    }  
                }    
                #elif defined __18CXX
                    PIC18_Optimized_SPI_Read_Packet();
            	#endif   

                //Check if we have received a multiple of the media block 
                //size (ex: 512 bytes).  If so, the next two bytes are going to 
                //be CRC values, rather than data bytes.  
                if(blockCounter == 0)
                {
                    //Read two bytes to receive the CRC-16 value on the data block.
                    MDD_SDSPI_ReadMedia();
                    MDD_SDSPI_ReadMedia();
                    //Following sending of the CRC-16 value, the media may still
                    //need more access time to internally fetch the next block.
                    //Therefore, it will send back 0xFF idle value, until it is
                    //ready.  Then it will send a new data start token, followed
                    //by the next block of useful data.
                    if(ioInfo.dwBytesRemaining != 0x00000000)
                    {
                        info->bStateVariable = ASYNC_READ_WAIT_START_TOKEN;
                    }
                    blockCounter = MEDIA_BLOCK_SIZE;
                    return ASYNC_READ_BUSY;
                }
                    
                return ASYNC_READ_NEW_PACKET_READY;
            }//if(ioInfo.dwBytesRemaining != 0x00000000)
            else
            {
                //We completed the read operation successfully and have returned
                //all data bytes requested.
                //Send CMD12 to let the media know we are finished reading
                //blocks from it, if we sent a multi-block read request earlier.
                if(SingleBlockRead == FALSE)
                {
                    SendMMCCmd(STOP_TRANSMISSION, 0x00000000);
                }    
                SD_CS = 1;  //De-select media
                mSend8ClkCycles();  
                info->bStateVariable = ASYNC_READ_COMPLETE;
                return ASYNC_READ_COMPLETE;
            }
        case ASYNC_READ_ABORT:
            //If the application firmware wants to cancel a read request.
            info->bStateVariable = ASYNC_READ_ERROR;
            //Send CMD12 to terminate the multi-block read request.
            response = SendMMCCmd(STOP_TRANSMISSION, 0x00000000);
            //Fall through to ASYNC_READ_ERROR/default case.
        case ASYNC_READ_ERROR:
        default:
            //Some error must have happened.
            SD_CS = 1;  //De-select media
            mSend8ClkCycles();  
            return ASYNC_READ_ERROR; 
    }//switch(info->stateVariable)    
    
    //Should never get to here.  All pathways should have already returned.
    return ASYNC_READ_ERROR;
}    




#ifdef __18CXX
/*****************************************************************************
  Function:
    static void PIC18_Optimized_SPI_Read_Packet(void)
  Summary:
    A private function intended for use internal to the SD-SPI.c file.
    This function reads a specified number of bytes from the SPI module,
    at high speed for optimum thoroughput, and copies them into the user
    specified RAM buffer.
    This function is only implemented and used on PIC18 devices.
  Pre-Conditions:
    The ioInfo.wNumBytes must be pre-initialized prior to calling 
    PIC18_Optimized_SPI_Read_Packet().
    Additionally, the ioInfo.pBuffer must also be pre-initialized, prior
    to calling PIC18_Optimized_SPI_Read_Packet().
  Input:
    ioInfo.wNumBytes global variable, initialized to the number of bytes to read.
    ioInfo.pBuffer global variable, initialize to point to the RAM location that
        the read data should be copied to.
  Return Values:
    None (although the ioInfo.pBuffer RAM specified will contain new contents)
  Side Effects:
    None
  Description:
    A private function intended for use internal to the SD-SPI.c file.
    This function reads a specified number of bytes from the SPI module,
    at high speed for optimum thoroughput, and copies them into the user
    specified RAM buffer.
    This function is only implemented and used on PIC18 devices.
  Remarks:
    This function is speed optimized, using inline assembly language code, and
    makes use of C compiler managed resources.  It is currently written to work
    with the Microchip MPLAB C18 compiler, and may need modification is built
    with a different PIC18 compiler.
  *****************************************************************************/
static void PIC18_Optimized_SPI_Read_Packet(void)
{
    static WORD FSR0Save;
    static WORD PRODSave;

    //Make sure the SPI_INTERRUPT_FLAG_ASM has been correctly defined, for the SPI
    //module that is actually being used in the hardware.
    #ifndef SPI_INTERRUPT_FLAG_ASM
        #error "Please define SPI_INTERRUPT_FLAG_ASM.  Double click this message for more info."
        //In the HardwareProfile - [platform name].h file for your project, please
        //add a "#define SPI_INTERRUPT_FLAG_ASM  PIRx, y" definition, where
        //PIRx is the PIR register holding the SSPxIF flag for the SPI module being used
        //to interface with the SD/MMC card, and y is the bit number for the SSPxIF bit (ex: 0-7).
    #endif

    //Make sure at least one byte needs to be read.
    if(ioInfo.wNumBytes == 0)
    {
        return;
    }

    //Context save C compiler managed registers that we will modify in this function.
    FSR0Save = FSR0;    
    PRODSave = PROD;    
    
    //Using PRODH and PRODL as convenient 16-bit access bank counter
    PROD = ioInfo.wNumBytes;    //ioInfo.wNumBytes holds the total number of bytes
                                //this function will read from SPI.
    //Going to use the FSR0 directly.  This is non-conventional, but delivers
    //better performance than using a normal C managed software RAM pointer.
    FSR0 = (WORD)ioInfo.pBuffer;

    //Initiate the first SPI operation
    WREG = SPIBUF;
    SPI_INTERRUPT_FLAG = 0;
    SPIBUF = 0xFF;

    //Highly speed efficient SPI read loop, written in inline assembly
    //language for best performance.  Total number of bytes that will be fetched
    //is exactly == the value of ioInfo.wNumBytes prior to calling this function.
    _asm
        bra     ASMSPIReadLoopEntryPoint
    
ASMSPIReadLoop:
        //Wait until last hardware SPI transaction is complete
        btfss   SPI_INTERRUPT_FLAG_ASM, 0
        bra     -2
        bcf     SPI_INTERRUPT_FLAG_ASM, 0

        //Save received byte and start the next transfer
        movf    SPIBUF, 0, 0    //Copy SPIBUF byte into WREG
        setf    SPIBUF, 0       //Write 0xFF to SPIBUF, to start a SPI transaction
        movwf   POSTINC0, 0     //Write the last received byte to the user's RAM buffer
    
ASMSPIReadLoopEntryPoint:
        //Now decrement 16-bit counter for loop exit test condition
        movlw   0x00
        decf    PRODL, 1, 0     //Decrement LSB
        subwfb  PRODH, 1, 0     //Decrement MSB, only if borrow from LSB decrement
        //Check if anymore bytes remain to be sent
        movf    PRODL, 0, 0     //copy PRODL to WREG
        iorwf   PRODH, 0, 0     //Z bit will be set if both PRODL and PRODH == 0x00
        bnz     ASMSPIReadLoop  //Go back and loop if our counter isn't = 0x0000.

        //Wait until the very last SPI transaction is complete and save the byte
        btfss   SPI_INTERRUPT_FLAG_ASM, 0
        bra     -2
        movff   SPIBUF, POSTINC0
    _endasm

    SPI_INTERRUPT_FLAG = 0;	 

    //Context restore C compiler managed registers
    PROD = PRODSave;
    FSR0 = FSR0Save;    
}    
#endif





/*****************************************************************************
  Function:
    BYTE MDD_SDSPI_AsyncWriteTasks(ASYNC_IO* info)
  Summary:
    Speed optimized, non-blocking, state machine based write function that writes
    data from the user specified buffer, onto the media, at the specified 
    media block address.
  Pre-Conditions:
    The ASYNC_IO structure must be initialized correctly, prior to calling
    this function for the first time.  Certain parameters, such as the user
    data buffer pointer (pBuffer) in the ASYNC_IO struct are allowed to be changed
    by the application firmware, in between each call to MDD_SDSPI_AsyncWriteTasks().
    Additionally, the media and microcontroller SPI module should have already 
    been initalized before using this function.  This function is mutually
    exclusive with the MDD_SDSPI_AsyncReadTasks() function.  Only one operation
    (either one read or one write) is allowed to be in progress at a time, as
    both functions share statically allocated resources and monopolize the SPI bus.
  Input:
    ASYNC_IO* info -        A pointer to a ASYNC_IO structure.  The 
                            structure contains information about how to complete
                            the write operation (ex: number of total bytes to write,
                            where to obtain the bytes from, number of bytes
                            to write for each call to MDD_SDSPI_AsyncWriteTasks(), etc.).
  Return Values:
    BYTE - Returns a status byte indicating the current state of the write 
            operation. The possible return values are:
            
            ASYNC_WRITE_BUSY - Returned when the state machine is busy waiting for
                             the media to become ready to accept new data.  The 
                             media has write time, which can often be quite long
                             (a few ms typ, with maximum of 250ms).  The application
                             should keep calling MDD_SDSPI_AsyncWriteTasks() until either
                             an error/timeout occurs, ASYNC_WRITE_SEND_PACKET
                             is returned, or ASYNC_WRITE_COMPLETE is returned.
            ASYNC_WRITE_SEND_PACKET -   Returned when the MDD_SDSPI_AsyncWriteTasks()
                                        handler is ready to consume data and send
                                        it to the media.  After ASYNC_WRITE_SEND_PACKET
                                        is returned, the application should make certain
                                        that the info->wNumBytes and pBuffer parameters
                                        are correct, prior to calling 
                                        MDD_SDSPI_AsyncWriteTasks() again.  After
                                        the function returns, the application is
                                        then free to write new data into the pBuffer
                                        RAM location. 
            ASYNC_WRITE_COMPLETE - Returned when all data bytes in the write
                                 operation have been written to the media successfully,
                                 and the media is now ready for the next operation.
            ASYNC_WRITE_ERROR - Returned when some failure occurs.  This could be
                               either due to a media timeout, or due to some other
                               unknown type of error.  In this case, the 
                               MDD_SDSPI_AsyncWriteTasks() handler will terminate
                               the write attempt and will try to put the media 
                               back in a default state, ready for a new command.  
                               The application firmware may then retry the write
                               attempt (if desired) by re-initializing the 
                               ASYNC_IO structure and setting the 
                               bStateVariable = ASYNC_WRITE_QUEUED.

            
  Side Effects:
    Uses the SPI bus and the media.  The media and SPI bus should not be
    used by any other function until the read operation has either completed
    successfully, or returned with the ASYNC_WRITE_ERROR condition.
  Description:
    Speed optimized, non-blocking, state machine based write function that writes 
    data packets to the media, from a user specified RAM buffer.
    This function uses either the single block or multi-block write command 
    to perform fast writes of the data.  The total amount of data that will be 
    written on any given call to MDD_SDSPI_AsyncWriteTasks() will be the 
    info->numBytes parameter.
    However, if the function is called repeatedly, with info->dwBytesRemaining
    set to a large number, this function can successfully write data sizes >> than
    the block size (theoretically anything up to ~4GB, since dwBytesRemaining is 
    a 32-bit DWORD).  The application firmware should continue calling 
    MDD_SDSPI_AsyncWriteTasks(), until the ASYNC_WRITE_COMPLETE value is returned 
    (or ASYNC_WRITE_ERROR), even if it has already sent all of the data expected.
    This is necessary, so the state machine can finish the write process and 
    terminate the multi-block write operation, once the total expected number 
    of bytes have been written.  This puts the media back into the default state 
    ready for a new command.
    
    During normal/successful operations, calls to MDD_SDSPI_AsyncWriteTasks() 
    would typically return:
    1. ASYNC_WRITE_SEND_PACKET - repeatedly, until 512 bytes [media read 
        block size] is received, then 
    2. ASYNC_WRITE_BUSY (for awhile, could be a long time, many milliseconds), then
    3. Back to ASYNC_WRITE_SEND_PACKET (repeatedly, until the next 512 byte
       boundary, then back to #2, etc.
    4. After all data is copied successfully, then the function will return 
       ASYNC_WRITE_COMPLETE, for all subsequent calls (until a new write operation
       is started, by re-initializing the ASYNC_IO structure, and re-calling
       the function).
    
  Remarks:
    When starting a read operation, the info->stateVariable must be initalized to
    ASYNC_WRITE_QUEUED.  All other fields in the info structure should also be
    initialized correctly.

    This function will monopolize the SPI module during the operation.  Do not
    use the SPI module for any other purpose, while a write operation is in
    progress.  Additionally, the ASYNC_IO structure must not be modified
    in a different context, while the MDD_SDSPI_AsyncReadTasks() function is 
    actively executing.
    In between calls to MDD_SDSPI_AsyncWriteTasks(), certain parameters, namely the
    info->wNumBytes and info->pBuffer are allowed to change however.
    
    The dwBytesRemaining value must always be an exact integer multiple of wNumBytes 
    for the function to operate correctly.  Additionally, it is required that
    the wNumBytes parameter, must always be less than or equal to the media block size,
    (which is 512 bytes).  Additionally, info->wNumBytes must always be an exact 
    integer factor of the media block size.  Example values that are allowed for
    info->wNumBytes are: 1, 2, 4, 8, 16, 32, 64, 128, 256, 512.
  *****************************************************************************/
BYTE MDD_SDSPI_AsyncWriteTasks(ASYNC_IO* info)
{
    static BYTE data_byte;
    static WORD blockCounter;
    static DWORD WriteTimeout;
    static BYTE command;
    DWORD preEraseBlockCount;
    MMC_RESPONSE response;

    
    //Check what state we are in, to decide what to do.
    switch(info->bStateVariable)
    {
        case ASYNC_WRITE_COMPLETE:
            return ASYNC_WRITE_COMPLETE;
        case ASYNC_WRITE_QUEUED:
            //Initiate the write sequence.
            blockCounter = MEDIA_BLOCK_SIZE;    //Initialize counter.  Will be used later for block boundary tracking.

            //Copy input structure into a statically allocated global instance 
            //of the structure, for faster local access of the parameters with 
            //smaller code size.
            ioInfo = *info;

            //Check if we are writing only a single block worth of data, or 
            //multiple blocks worth of data.
            if(ioInfo.dwBytesRemaining <= MEDIA_BLOCK_SIZE)
            {
                command = WRITE_SINGLE_BLOCK;
            }    
            else
            {
                command = WRITE_MULTI_BLOCK;
                
                //Compute the number of blocks that we are going to be writing in this multi-block write operation.
                preEraseBlockCount = (ioInfo.dwBytesRemaining >> 9); //Divide by 512 to get the number of blocks to write
                //Always need to erase at least one block.
                if(preEraseBlockCount == 0)
                {
                    preEraseBlockCount++;   
                } 
    
                //Should send CMD55/ACMD23 to let the media know how many blocks it should 
                //pre-erase.  This isn't essential, but it allows for faster multi-block 
                //writes, and probably also reduces flash wear on the media.
                response = SendMMCCmd(APP_CMD, 0x00000000);    //Send CMD55
                if(response.r1._byte == 0x00)   //Check if successful.
                {
                    SendMMCCmd(SET_WR_BLK_ERASE_COUNT , preEraseBlockCount);    //Send ACMD23        
                }
            }    

            //The info->dwAddress parameter is the block address.
            //For standard capacity SD cards, the card expects a complete byte address.
            //To convert the block address into a byte address, we multiply by the block size (512).
            //For SDHC (high capacity) cards, the card expects a block address already, so no
            //address cconversion is needed
            if (gSDMode == SD_MODE_NORMAL)  
            {
                ioInfo.dwAddress <<= 9;   //<< 9 multiplies by 512
            }    

            //Send the write single or write multi command, with the LBA or byte 
            //address (depeding upon SDHC or standard capacity card)
            response = SendMMCCmd(command, ioInfo.dwAddress);    

            //See if it was accepted
            if(response.r1._byte != 0x00)
            {
                //Perhaps the card isn't initialized or present.
                info->bStateVariable = ASYNC_WRITE_ERROR;
                return ASYNC_WRITE_ERROR; 
            }    
            else
            {
                //Card is ready to receive start token and data bytes.
                info->bStateVariable = ASYNC_WRITE_TRANSMIT_PACKET;
            } 
            return ASYNC_WRITE_SEND_PACKET;   

        case ASYNC_WRITE_TRANSMIT_PACKET:
            //Check if we just finished programming a block, or we are starting
            //for the first time.  In this case, we need to send the data start token.
            if(blockCounter == MEDIA_BLOCK_SIZE)
            {
                //Send the correct data start token, based on the type of write we are doing
                if(command == WRITE_MULTI_BLOCK)
                {
                    WriteSPIM(DATA_START_MULTI_BLOCK_TOKEN);   
                }
                else
                {
                    //Else must be a single block write
                    WriteSPIM(DATA_START_TOKEN);   
                }        
            } 
               
            //Update local copy of pointer and byte count.  Application firmware
            //is alllowed to change these between calls to this handler function.
            ioInfo.wNumBytes = info->wNumBytes;
            ioInfo.pBuffer = info->pBuffer;
            
            //Keep track of variables for loop/state exit conditions.
            ioInfo.dwBytesRemaining -= ioInfo.wNumBytes;
            blockCounter -= ioInfo.wNumBytes;
            
            //Now send a packet of raw data bytes to the media, over SPI.
            //This code directly impacts data thoroughput in a significant way.  
            //Special care should be used to make sure this code is speed optimized.
        	#if defined __C30__ || defined __C32__
            {
                //PIC24/dsPIC/PIC32 architecture is efficient with pointers and 
                //local variables due to the large number of WREGs available.
                //Therefore, this code gives good SPI bus utilization, provided
                //the compiler optimization level is 's' or '3'.
                BYTE* localPointer = ioInfo.pBuffer;    
                WORD localCounter = ioInfo.wNumBytes;
                do
                {
                    SPIBUF = *localPointer++;
                    localCounter--;
                    while(!SPISTAT_RBF);
                    data_byte = SPIBUF; //Dummy read to clear SPISTAT_RBF
                }while(localCounter);         
            }                	    
            #elif defined __18CXX   
                PIC18_Optimized_SPI_Write_Packet();
            #endif
 
            //Check if we have finshed sending a 512 byte block.  If so,
            //need to receive 16-bit CRC, and retrieve the data_response token
            if(blockCounter == 0)
            {
                blockCounter = MEDIA_BLOCK_SIZE;    //Re-initialize counter
                
                //Add code to compute CRC, if using CRC. By default, the media 
                //doesn't use CRC unless it is enabled manually during the card 
                //initialization sequence.
                mSendCRC();  //Send 16-bit CRC for the data block just sent
                
                //Read response token byte from media, mask out top three don't 
                //care bits, and check if there was an error
                if((MDD_SDSPI_ReadMedia() & WRITE_RESPONSE_TOKEN_MASK) != DATA_ACCEPTED)
                {
                    //Something went wrong.  Try and terminate as gracefully as 
                    //possible, so as allow possible recovery.
                    info->bStateVariable = ASYNC_WRITE_ABORT; 
                    return ASYNC_WRITE_BUSY;
                }
                
                //The media will now send busy token (0x00) bytes until
                //it is internally ready again (after the block is successfully
                //writen and the card is ready to accept a new block.
                info->bStateVariable = ASYNC_WRITE_MEDIA_BUSY;
                WriteTimeout = WRITE_TIMEOUT;       //Initialize timeout counter
                return ASYNC_WRITE_BUSY;
            }//if(blockCounter == 0)
            
            //If we get to here, we haven't reached a block boundary yet.  Keep 
            //on requesting packets of data from the application.
            return ASYNC_WRITE_SEND_PACKET;   

        case ASYNC_WRITE_MEDIA_BUSY:
            if(WriteTimeout != 0)
            {
                WriteTimeout--;
                mSend8ClkCycles();  //Dummy read to gobble up a byte (ex: to ensure we meet NBR timing parameter)
                data_byte = MDD_SDSPI_ReadMedia();  //Poll the media.  Will return 0x00 if still busy.  Will return non-0x00 is ready for next data block.
                if(data_byte != 0x00)
                {
                    //The media is done and is no longer busy.  Go ahead and
                    //either send the next packet of data to the media, or the stop
                    //token if we are finshed.
                    if(ioInfo.dwBytesRemaining == 0)
                    {
                        WriteTimeout = WRITE_TIMEOUT;
                        if(command == WRITE_MULTI_BLOCK)
                        {
                            //We finished sending all bytes of data.  Send the stop token byte.
                            WriteSPIM(DATA_STOP_TRAN_TOKEN);
                            //After sending the stop transmission token, we need to
                            //gobble up one byte before checking for media busy (0x00).
                            //This is to meet the NBR timing parameter.  During the NBR
                            //interval the SD card may not respond with the busy signal, even
                            //though it is internally busy.
                            mSend8ClkCycles();
                                                
                            //The media still needs to finish internally writing.
                            info->bStateVariable = ASYNC_STOP_TOKEN_SENT_WAIT_BUSY;
                            return ASYNC_WRITE_BUSY;
                        }
                        else
                        {
                            //In this case we were doing a single block write,
                            //so no stop token is necessary.  In this case we are
                            //now fully complete with the write operation.
                            SD_CS = 1;          //De-select media
                            mSend8ClkCycles();  
                            info->bStateVariable = ASYNC_WRITE_COMPLETE;
                            return ASYNC_WRITE_COMPLETE;                            
                        }                            
                        
                    }
                    //Else we have more data to write in the multi-block write.    
                    info->bStateVariable = ASYNC_WRITE_TRANSMIT_PACKET;  
                    return ASYNC_WRITE_SEND_PACKET;                    
                }    
                else
                {
                    //The media is still busy.
                    return ASYNC_WRITE_BUSY;
                }    
            }
            else
            {
                //Timeout occurred.  Something went wrong.  The media should not 
                //have taken this long to finish the write.
                info->bStateVariable = ASYNC_WRITE_ABORT;
                return ASYNC_WRITE_BUSY;
            }        
        
        case ASYNC_STOP_TOKEN_SENT_WAIT_BUSY:
            //We already sent the stop transmit token for the multi-block write 
            //operation.  Now all we need to do, is keep waiting until the card
            //signals it is no longer busy.  Card will keep sending 0x00 bytes
            //until it is no longer busy.
            if(WriteTimeout != 0)
            {
                WriteTimeout--;
                data_byte = MDD_SDSPI_ReadMedia();
                //Check if card is no longer busy.  
                if(data_byte != 0x00)
                {
                    //If we get to here, multi-block write operation is fully
                    //complete now.  

                    //Should send CMD13 (SEND_STATUS) after a programming sequence, 
                    //to confirm if it was successful or not inside the media.
                                
                    //Prepare to receive the next command.
                    SD_CS = 1;          //De-select media
                    mSend8ClkCycles();  //NEC timing parameter clocking
                    info->bStateVariable = ASYNC_WRITE_COMPLETE;
                    return ASYNC_WRITE_COMPLETE;
                }
                //If we get to here, the media is still busy with the write.
                return ASYNC_WRITE_BUSY;    
            }    
            //Timeout occurred.  Something went wrong.  Fall through to ASYNC_WRITE_ABORT.
        case ASYNC_WRITE_ABORT:
            //An error occurred, and we need to stop the write sequence so as to try and allow
            //for recovery/re-attempt later.
            SendMMCCmd(STOP_TRANSMISSION, 0x00000000);
            SD_CS = 1;  //deselect media
            mSend8ClkCycles();  //After raising CS pin, media may not tri-state data out for 1 bit time.
            info->bStateVariable = ASYNC_WRITE_ERROR; 
            //Fall through to default case.
        default:
            //Used for ASYNC_WRITE_ERROR case.
            return ASYNC_WRITE_ERROR; 
    }//switch(info->stateVariable)    
    

    //Should never execute to here.  All pathways should have a hit a return already.
    info->bStateVariable = ASYNC_WRITE_ABORT;
    return ASYNC_WRITE_BUSY;
} 


#ifdef __18CXX   
/*****************************************************************************
  Function:
    static void PIC18_Optimized_SPI_Write_Packet(void)
  Summary:
    A private function intended for use internal to the SD-SPI.c file.
    This function writes a specified number of bytes to the SPI module,
    at high speed for optimum throughput, copied from the user specified RAM
    buffer.
    This function is only implemented and used on PIC18 devices.
  Pre-Conditions:
    The ioInfo.wNumBytes must be pre-initialized prior to calling 
    PIC18_Optimized_SPI_Write_Packet().
    Additionally, the ioInfo.pBuffer must also be pre-initialized, prior
    to calling PIC18_Optimized_SPI_Write_Packet().
  Input:
    ioInfo.wNumBytes global variable, initialized to the number of bytes to send
    ioInfo.pBuffer global variable, initialized to point to the RAM location that
        contains the data to send out the SPI port
  Return Values:
    None
  Side Effects:
    None
  Description:
    A private function intended for use internal to the SD-SPI.c file.
    This function writes a specified number of bytes to the SPI module,
    at high speed for optimum throughput, copied from the user specified RAM
    buffer.
    This function is only implemented and used on PIC18 devices.
  Remarks:
    This function is speed optimized, using inline assembly language code, and
    makes use of C compiler managed resources.  It is currently written to work
    with the Microchip MPLAB C18 compiler, and may need modification if built
    with a different PIC18 compiler.
  *****************************************************************************/
static void PIC18_Optimized_SPI_Write_Packet(void)
{
    static BYTE bData;
    static WORD FSR0Save;
    static WORD PRODSave;

    //Make sure the SPI_INTERRUPT_FLAG_ASM has been correctly defined, for the SPI
    //module that is actually being used in the hardware.
    #ifndef SPI_INTERRUPT_FLAG_ASM
        #error Please add "#define SPI_INTERRUPT_FLAG_ASM  PIRx, Y" to your hardware profile.  Replace x and Y with appropriate numbers for your SPI module interrupt flag.
    #endif
    
    //Make sure at least one byte needs copying.
    if(ioInfo.wNumBytes == 0)
    {
        return;
    }    

    //Context save C compiler managed registers.
    FSR0Save = FSR0; 
    PRODSave = PROD;
    
    //Using PRODH and PRODL as 16 bit loop counter.  These are convenient since
    //they are always in the access bank.
    PROD = ioInfo.wNumBytes;
    //Using FSR0 directly, for optimal SPI loop speed.
    FSR0 = (WORD)ioInfo.pBuffer; 
                              
    _asm
        movf    POSTINC0, 0, 0  //Fetch next byte to send and store in WREG
        bra     ASMSPIXmitLoopEntryPoint
ASMSPIXmitLoop:    
        movf    POSTINC0, 0, 0  //Pre-Fetch next byte to send and temporarily store in WREG
        //Wait until last hardware SPI transaction is complete
        btfss   SPI_INTERRUPT_FLAG_ASM, 0
        bra     -2
        
ASMSPIXmitLoopEntryPoint:
        //Start the next SPI transaction
        bcf     SPI_INTERRUPT_FLAG_ASM, 0   //Clear interrupt flag
        movwf   SPIBUF, 0       //Write next byte to transmit to SSPBUF
        
        //Now decrement byte counter for loop exit condition
        movlw   0x00
        decf    PRODL, 1, 0     //Decrement LSB
        subwfb  PRODH, 1, 0     //Decrement MSB, only if borrow from LSB decrement
        //Check if anymore bytes remain to be sent
        movf    PRODL, 0, 0     //copy PRODL to WREG
        iorwf   PRODH, 0, 0     //Z bit will be set if both PRODL and PRODH == 0x00
        bnz     ASMSPIXmitLoop  //Go back and loop if our counter isn't = 0x0000.
    _endasm

    //Wait until the last SPI transaction is really complete.  
    //Above loop jumps out after the last byte is started, but not finished yet.
    while(!SPI_INTERRUPT_FLAG);

    //Leave SPI module in a "clean" state, ready for next transaction.
    bData = SPIBUF;         //Dummy read to clear BF flag.
    SPI_INTERRUPT_FLAG = 0; //Clear interrupt flag.
    //Restore C compiler managed registers that we modified
    PROD = PRODSave;
    FSR0 = FSR0Save;
}    
#endif    





/*****************************************************************************
  Function:
    BYTE MDD_SDSPI_SectorWrite (DWORD sector_addr, BYTE * buffer, BYTE allowWriteToZero)
  Summary:
    Writes a sector of data to an SD card.
  Conditions:
    The MDD_SectorWrite function pointer must be pointing to this function.
  Input:
    sector_addr -      The address of the sector on the card.
    buffer -           The buffer with the data to write.
    allowWriteToZero -
                     - TRUE -  Writes to the 0 sector (MBR) are allowed
                     - FALSE - Any write to the 0 sector will fail.
  Return Values:
    TRUE -  The sector was written successfully.
    FALSE - The sector could not be written.
  Side Effects:
    None.
  Description:
    The MDD_SDSPI_SectorWrite function writes one sector of data (512 bytes) 
    of data from the location pointed to by 'buffer' to the specified sector of 
    the SD card.
  Remarks:
    The card expects the address field in the command packet to be a byte address.
    The sector_addr value is ocnverted to a byte address by shifting it left nine
    times (multiplying by 512).
  ***************************************************************************************/
BYTE MDD_SDSPI_SectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero)
{
    static ASYNC_IO info;
    BYTE status;
    
    if(allowWriteToZero == FALSE)
    {
        if(sector_addr == 0x00000000)
        {
            return FALSE;
        }    
    }    
    
    //Initialize structure so we write a single sector worth of data.
    info.wNumBytes = 512;
    info.dwBytesRemaining = 512;
    info.pBuffer = buffer;
    info.dwAddress = sector_addr;
    info.bStateVariable = ASYNC_WRITE_QUEUED;
    
    //Repeatedly call the write handler until the operation is complete (or a
    //failure/timeout occurred).
    while(1)
    {
        status = MDD_SDSPI_AsyncWriteTasks(&info);
        if(status == ASYNC_WRITE_COMPLETE)
        {
            return TRUE;
        }    
        else if(status == ASYNC_WRITE_ERROR)
        {
            return FALSE;
        }
    }    
    return TRUE;
}    




/*******************************************************************************
  Function:
    BYTE MDD_SDSPI_WriteProtectState
  Summary:
    Indicates whether the card is write-protected.
  Conditions:
    The MDD_WriteProtectState function pointer must be pointing to this function.
  Input:
    None.
  Return Values:
    TRUE -  The card is write-protected
    FALSE - The card is not write-protected
  Side Effects:
    None.
  Description:
    The MDD_SDSPI_WriteProtectState function will determine if the SD card is
    write protected by checking the electrical signal that corresponds to the
    physical write-protect switch.
  Remarks:
    None
*******************************************************************************/

BYTE MDD_SDSPI_WriteProtectState(void)
{
    return FALSE;
}


/*******************************************************************************
  Function:
    void Delayms (BYTE milliseconds)
  Summary:
    Delay.
  Conditions:
    None.
  Input:
    BYTE milliseconds - Number of ms to delay
  Return:
    None.
  Side Effects:
    None.
  Description:
    The Delayms function will delay a specified number of milliseconds.  Used for SPI
    timing.
  Remarks:
    Depending on compiler revisions, this function may not delay for the exact 
    time specified.  This shouldn't create a significant problem.
*******************************************************************************/

void Delayms(BYTE milliseconds)
{
    BYTE    ms;
    DWORD   count;
    
    ms = milliseconds;
    while (ms--)
    {
        count = MILLISECDELAY;
        while (count--);
    }
    Nop();
    return;
}


/*******************************************************************************
  Function:
    void CloseSPIM (void)
  Summary:
    Disables the SPI module.
  Conditions:
    None.
  Input:
    None.
  Return:
    None.
  Side Effects:
    None.
  Description:
    Disables the SPI module.
  Remarks:
    None.
*******************************************************************************/

void CloseSPIM (void)
{
#if defined __C30__ || defined __C32__

    SPISTAT &= 0x7FFF;

#elif defined __18CXX

    SPICON1 &= 0xDF;

#endif
}



/*****************************************************************************
  Function:
    unsigned char WriteSPIM (unsigned char data_out)
  Summary:
    Writes data to the SD card.
  Conditions:
    None.
  Input:
    data_out - The data to write.
  Return:
    0.
  Side Effects:
    None.
  Description:
    The WriteSPIM function will write a byte of data from the microcontroller to the
    SD card.
  Remarks:
    None.
  ***************************************************************************************/

unsigned char WriteSPIM( unsigned char data_out )
{
#ifdef __PIC32MX__
    BYTE   clear;
    putcSPI((BYTE)data_out);
    clear = getcSPI();
    return ( 0 );                // return non-negative#
#elif defined __18CXX
    BYTE clear;
    clear = SPIBUF;
    SPI_INTERRUPT_FLAG = 0;
    SPIBUF = data_out;
    if (SPICON1 & 0x80)
        return -1;
    else
        while (!SPI_INTERRUPT_FLAG);
    return 0;
#else
    BYTE   clear;
    SPIBUF = data_out;          // write byte to SSP1BUF register
    while( !SPISTAT_RBF ); // wait until bus cycle complete
    clear = SPIBUF;
    return ( 0 );                // return non-negative#
#endif
}



/*****************************************************************************
  Function:
    BYTE MDD_SDSPI_ReadMedia (void)
  Summary:
    Reads a byte of data from the SD card.
  Conditions:
    None.
  Input:
    None.
  Return:
    The byte read.
  Side Effects:
    None.
  Description:
    The MDD_SDSPI_ReadMedia function will read one byte from the SPI port.
  Remarks:
    This function replaces ReadSPI, since some implementations of that function
    will initialize SSPBUF/SPIBUF to 0x00 when reading.  The card expects 0xFF.
  ***************************************************************************************/
BYTE MDD_SDSPI_ReadMedia(void)
{

#ifdef __C32__

    putcSPI((BYTE)0xFF);
    return (BYTE)getcSPI();

#elif defined __18CXX

    BYTE clear;
    clear = SPIBUF;
    SPI_INTERRUPT_FLAG = 0;
    SPIBUF = 0xFF;
    while (!SPI_INTERRUPT_FLAG);
    return SPIBUF;

#else
    SPIBUF = 0xFF;                              //Data Out - Logic ones
    while(!SPISTAT_RBF);                     //Wait until cycle complete
    return(SPIBUF);                             //Return with byte read
#endif
}

/*****************************************************************************
  Function:
    void OpenSPIM (unsigned int sync_mode)
  Summary:
    Initializes the SPI module
  Conditions:
    None.
  Input:
    sync_mode - Input parameter that sets the SPI mode/speed.
  Return:
    None.
  Side Effects:
    None.
  Description:
    The OpenSPIM function will enable and configure the SPI module.
  Remarks:
    None.
  ***************************************************************************************/

#ifdef __18CXX
void OpenSPIM (unsigned char sync_mode)
#else
void OpenSPIM( unsigned int sync_mode)
#endif
{
    SPISTAT = 0x0000;               // power on state 

    //SPI module initilization depends on processor type
    #ifdef __PIC32MX__
        #if (GetSystemClock() <= 20000000)
            SPIBRG = SPICalutateBRG(GetPeripheralClock(), 10000);
        #else
            SPIBRG = SPICalutateBRG(GetPeripheralClock(), SPI_FREQUENCY);
        #endif
        SPICON1bits.CKP = 1;
        SPICON1bits.CKE = 0;
    #elif defined __C30__ //must be PIC24 or dsPIC device
        SPICON1 = 0x0000;              // power on state
        SPICON1 |= sync_mode;          // select serial mode 
        SPICON1bits.CKP = 1;
        SPICON1bits.CKE = 0;
    #else   //must be __18CXX (PIC18 processor)
        SPICON1 = 0x00;         
        SPICON1 |= sync_mode;   
        SPISTATbits.CKE = 1;
    #endif

        // MISO <- RP20
        _SDI1R = 20;
        // RP22 <- MOSI
        _RP22R = 7;
        // RP25 <- SCLK
        _RP25R = 8;
//    SPICLOCK = 0;
//    SPIOUT = 0;                  // define SDO1 as output (master or slave)
//    SPIIN = 1;                  // define SDI1 as input (master or slave)
    SPIENABLE = 1;             // enable synchronous serial port
}


#ifdef __18CXX
// Description: Delay value for the manual SPI clock
#define MANUAL_SPI_CLOCK_VALUE             1
/*****************************************************************************
  Function:
    unsigned char WriteSPIManual (unsigned char data_out)
  Summary:
    Write a character to the SD card with bit-bang SPI.
  Conditions:
    Make sure the SDI pin is pre-configured as a digital pin, if it is 
    multiplexed with analog functionality.
  Input:
    data_out - Data to send.
  Return:
    0.
  Side Effects:
    None.
  Description:
    Writes a character to the SD card.
  Remarks:
    The WriteSPIManual function is for use on a PIC18 when the clock speed is so
    high that the maximum SPI clock divider cannot reduce the SPI clock speed below
    the maximum SD card initialization speed.
  ***************************************************************************************/
unsigned char WriteSPIManual(unsigned char data_out)
{
    unsigned char i;
    unsigned char clock;

    SPICLOCKLAT = 0;
    SPIOUTLAT = 1;
    SPICLOCK = OUTPUT;
    SPIOUT = OUTPUT;

	//Loop to send out 8 bits of SDO data and associated SCK clock.
	for(i = 0; i < 8; i++)
	{
		SPICLOCKLAT = 0;
		if(data_out & 0x80)
			SPIOUTLAT = 1;
		else
			SPIOUTLAT = 0;
		data_out = data_out << 1;				//Bit shift, so next bit to send is in MSb position
    	clock = MANUAL_SPI_CLOCK_VALUE;
    	while (clock--);
    	SPICLOCKLAT = 1;
    	clock = MANUAL_SPI_CLOCK_VALUE;
    	while (clock--);    			
	}	
    SPICLOCKLAT = 0;

    return 0; 
}


/*****************************************************************************
  Function:
    BYTE ReadMediaManual (void)
  Summary:
    Reads a byte of data from the SD card.
  Conditions:
    None.
  Input:
    None.
  Return:
    The byte read.
  Side Effects:
    None.
  Description:
    The MDD_SDSPI_ReadMedia function will read one byte from the SPI port.
  Remarks:
    This function replaces ReadSPI, since some implementations of that function
    will initialize SSPBUF/SPIBUF to 0x00 when reading.  The card expects 0xFF.
    This function is for use on a PIC18 when the clock speed is so high that the
    maximum SPI clock prescaler cannot reduce the SPI clock below the maximum SD card
    initialization speed.
  ***************************************************************************************/
BYTE ReadMediaManual (void)
{
    unsigned char i;
    unsigned char clock;
    unsigned char result = 0x00;

    SPIOUTLAT = 1;
    SPIOUT = OUTPUT;
    SPIIN = INPUT;
    SPICLOCKLAT = 0;
    SPICLOCK = OUTPUT;
 
 	//Loop to send 8 clock pulses and read in the returned bits of data. Data "sent" will be = 0xFF
	for(i = 0; i < 8u; i++)
	{
		SPICLOCKLAT = 0;
    	clock = MANUAL_SPI_CLOCK_VALUE;
    	while (clock--);
    	SPICLOCKLAT = 1;
    	clock = MANUAL_SPI_CLOCK_VALUE;
    	while (clock--);
		result = result << 1;	//Bit shift the previous result.  We receive the byte MSb first. This operation makes LSb = 0.  
    	if(SPIINPORT)
    		result++;			//Set the LSb if we detected a '1' on the SPIINPORT pin, otherwise leave as 0.
	}	
    SPICLOCKLAT = 0;

    return result;
}//end ReadMedia
#endif      // End __18CXX

/*****************************************************************************
  Function:
    void InitSPISlowMode(void)
  Summary:
    Initializes the SPI module to operate at low SPI frequency <= 400kHz.
  Conditions:
    Processor type and GetSystemClock() macro have to be defined correctly
    to get the correct SPI frequency.
  Input:
    Uses GetSystemClock() macro value.  Should be #define in the hardwareprofile.
  Return Values:
    None.  Initializes the hardware SPI module (except on PIC18).  On PIC18,
    The SPI is bit banged to achieve low frequencies, but this function still
    initializes the I/O pins. 
  Side Effects:
    None.
  Description:
    This function initalizes and enables the SPI module, configured for low 
    SPI frequency, so as to be compatible with media cards which require <400kHz
    SPI frequency during initialization.
  Remarks:
    None.
  ***************************************************************************************/
void InitSPISlowMode(void)
{
    #if defined __C30__ || defined __C32__
    	#ifdef __PIC32MX__
    		OpenSPI(SPI_START_CFG_1, SPI_START_CFG_2);
    	    SPIBRG = SPICalutateBRG(GetPeripheralClock(), 400000);
    	#else //else C30 = PIC24/dsPIC devices
    		WORD spiconvalue = 0x0003;
            WORD timeout;
    	    // Calculate the prescaler needed for the clock
    	    timeout = GetSystemClock() / 400000;
    	    // if timeout is less than 400k and greater than 100k use a 1:1 prescaler
    	    if (timeout == 0)
    	    {
    	        OpenSPIM (MASTER_ENABLE_ON | PRI_PRESCAL_1_1 | SEC_PRESCAL_1_1);
    	    }
    	    else
    	    {
    	        while (timeout != 0)
    	        {
    	            if (timeout > 8)
    	            {
    	                spiconvalue--;
    	                // round up
    	                if ((timeout % 4) != 0)
    	                    timeout += 4;
    	                timeout /= 4;
    	            }
    	            else
    	            {
    	                break;
    	            }
    	        }
    	        
    	        timeout--;
    	    
    	        OpenSPIM (MASTER_ENABLE_ON | spiconvalue | ((~(timeout << 2)) & 0x1C));
    	    }
    	#endif   //#ifdef __PIC32MX__ (and corresponding #else)    
    #else //must be PIC18 device
        //Make sure the SPI module doesn't control the bus, will use 
        //bit-banged SPI instead, for slow mode initialization operation
        SPICON1 = 0x00;
        SPICLOCKLAT = 0;
        SPIOUTLAT = 1;
        SPICLOCK = OUTPUT;
        SPIOUT = OUTPUT;
    #endif //#if defined __C30__ || defined __C32__
}    



/*****************************************************************************
  Function:
    MEDIA_INFORMATION *  MDD_SDSPI_MediaInitialize (void)
  Summary:
    Initializes the SD card.
  Conditions:
    The MDD_MediaInitialize function pointer must be pointing to this function.
  Input:
    None.
  Return Values:
    The function returns a pointer to the MEDIA_INFORMATION structure.  The
    errorCode member may contain the following values:
        * MEDIA_NO_ERROR - The media initialized successfully
        * MEDIA_CANNOT_INITIALIZE - Cannot initialize the media.  
  Side Effects:
    None.
  Description:
    This function will send initialization commands to and SD card.
  Remarks:
    Psuedo code flow for the media initialization process is as follows:

-------------------------------------------------------------------------------------------
SD Card SPI Initialization Sequence (for physical layer v1.x or v2.0 device) is as follows:
-------------------------------------------------------------------------------------------
0.  Power up tasks
    a.  Initialize microcontroller SPI module to no more than 400kbps rate so as to support MMC devices.
    b.  Add delay for SD card power up, prior to sending it any commands.  It wants the 
        longer of: 1ms, the Vdd ramp time (time from 2.7V to Vdd stable), and 74+ clock pulses.
1.  Send CMD0 (GO_IDLE_STATE) with CS = 0.  This puts the media in SPI mode and software resets the SD/MMC card.
2.  Send CMD8 (SEND_IF_COND).  This requests what voltage the card wants to run at. 
    Note: Some cards will not support this command.
    a.  If illegal command response is received, this implies either a v1.x physical spec device, or not an SD card (ex: MMC).
    b.  If normal response is received, then it must be a v2.0 or later SD memory card.

If v1.x device:
-----------------
3.  Send CMD1 repeatedly, until initialization complete (indicated by R1 response byte/idle bit == 0)
4.  Basic initialization is complete.  May now switch to higher SPI frequencies.
5.  Send CMD9 to read the CSD structure.  This will tell us the total flash size and other info which will be useful later.
6.  Parse CSD structure bits (based on v1.x structure format) and extract useful information about the media.
7.  The card is now ready to perform application data transfers.

If v2.0+ device:
-----------------
3.  Verify the voltage range is feasible.  If not, unusable card, should notify user that the card is incompatible with this host.
4.  Send CMD58 (Read OCR).
5.  Send CMD55, then ACMD41 (SD_SEND_OP_COND, with HCS = 1).
    a.  Loop CMD55/ACMD41 until R1 response byte == 0x00 (indicating the card is no longer busy/no longer in idle state).  
6.  Send CMD58 (Get CCS).
    a.  If CCS = 1 --> SDHC card.
    b.  If CCS = 0 --> Standard capacity SD card (which is v2.0+).
7.  Basic initialization is complete.  May now switch to higher SPI frequencies.
8.  Send CMD9 to read the CSD structure.  This will tell us the total flash size and other info which will be useful later.
9.  Parse CSD structure bits (based on v2.0 structure format) and extract useful information about the media.
10. The card is now ready to perform application data transfers.
--------------------------------------------------------------------------------
********************************************************************************/

MEDIA_INFORMATION *  MDD_SDSPI_MediaInitialize(void)
{
    WORD timeout;
    MMC_RESPONSE response;
	BYTE CSDResponse[20];
	BYTE count, index;
	DWORD c_size;
	BYTE c_size_mult;
	BYTE block_len;
	
	#ifdef __DEBUG_UART
	InitUART();
	#endif
 
    //Initialize global variables.  Will get updated later with valid data once
    //the data is known.
    mediaInformation.errorCode = MEDIA_NO_ERROR;
    mediaInformation.validityFlags.value = 0;
    MDD_SDSPI_finalLBA = 0x00000000;	//Will compute a valid size later, from the CSD register values we get from the card
    gSDMode = SD_MODE_NORMAL;           //Will get updated later with real value, once we know based on initialization flow.

    SD_CS = 1;               //Initialize Chip Select line (1 = card not selected)

    //MMC media powers up in the open-drain mode and cannot handle a clock faster
    //than 400kHz. Initialize SPI port to <= 400kHz
    InitSPISlowMode();    
    
    #ifdef __DEBUG_UART  
    PrintROMASCIIStringUART("\r\n\r\nInitializing Media\r\n"); 
    #endif

    //Media wants the longer of: Vdd ramp time, 1 ms fixed delay, or 74+ clock pulses.
    //According to spec, CS should be high during the 74+ clock pulses.
    //In practice it is preferrable to wait much longer than 1ms, in case of
    //contact bounce, or incomplete mechanical insertion (by the time we start
    //accessing the media). 
    Delayms(30);
    SD_CS = 1;
    //Generate 80 clock pulses.
    for(timeout=0; timeout<10u; timeout++)
        WriteSPISlow(0xFF);


    // Send CMD0 (with CS = 0) to reset the media and put SD cards into SPI mode.
    timeout = 100;
    do
    {
        //Toggle chip select, to make media abandon whatever it may have been doing
        //before.  This ensures the CMD0 is sent freshly after CS is asserted low,
        //minimizing risk of SPI clock pulse master/slave syncronization problems, 
        //due to possible application noise on the SCK line.
        SD_CS = 1;
        WriteSPISlow(0xFF);   //Send some "extraneous" clock pulses.  If a previous
                              //command was terminated before it completed normally,
                              //the card might not have received the required clocking
                              //following the transfer.
        SD_CS = 0;
        timeout--;

        //Send CMD0 to software reset the device
        response = SendMediaSlowCmd(GO_IDLE_STATE, 0x0);
    }while((response.r1._byte != 0x01) && (timeout != 0));
    //Check if all attempts failed and we timed out.  Normally, this won't happen,
    //unless maybe the SD card was busy, because it was previously performing a
    //read or write operation, when it was interrupted by the microcontroller getting
    //reset or power cycled, without also resetting or power cycling the SD card.
    //In this case, the SD card may still be busy (ex: trying to respond with the 
    //read request data), and may not be ready to process CMD0.  In this case,
    //we can try to recover by issuing CMD12 (STOP_TRANSMISSION).
    if(timeout == 0)
    {
        #ifdef __DEBUG_UART  
        PrintROMASCIIStringUART("Media failed CMD0 too many times. R1 response byte = ");
        PrintRAMBytesUART(((unsigned char*)&response + 1), 1);
        UARTSendLineFeedCarriageReturn();
        PrintROMASCIIStringUART("Trying CMD12 to recover.\r\n");
        #endif

        SD_CS = 1;
        WriteSPISlow(0xFF);       //Send some "extraneous" clock pulses.  If a previous
                                  //command was terminated before it completed normally,
                                  //the card might not have received the required clocking
                                  //following the transfer.
        SD_CS = 0;

        //Send CMD12, to stop any read/write transaction that may have been in progress
        response = SendMediaSlowCmd(STOP_TRANSMISSION, 0x0);    //Blocks until SD card signals non-busy
        //Now retry to send send CMD0 to perform software reset on the media
        response = SendMediaSlowCmd(GO_IDLE_STATE, 0x0);        
        if(response.r1._byte != 0x01) //Check if card in idle state now.
        {
            //Card failed to process CMD0 yet again.  At this point, the proper thing
            //to do would be to power cycle the card and retry, if the host 
            //circuitry supports disconnecting the SD card power.  Since the
            //SD/MMC PICtail+ doesn't support software controlled power removal
            //of the SD card, there is nothing that can be done with this hardware.
            //Therefore, we just give up now.  The user needs to physically 
            //power cycle the media and/or the whole board.
            #ifdef __DEBUG_UART  
            PrintROMASCIIStringUART("Media still failed CMD0. Cannot initialize card, returning.\r\n");
            #endif   
            mediaInformation.errorCode = MEDIA_CANNOT_INITIALIZE;
            return &mediaInformation;
        }            
        else
        {
            //Card successfully processed CMD0 and is now in the idle state.
            #ifdef __DEBUG_UART  
            PrintROMASCIIStringUART("Media successfully processed CMD0 after CMD12.\r\n");
            #endif        
        }    
    }//if(timeout == 0) [for the CMD0 transmit loop]
    else
    {
        #ifdef __DEBUG_UART  
        PrintROMASCIIStringUART("Media successfully processed CMD0.\r\n");
        #endif        
    }       
    

    //Send CMD8 (SEND_IF_COND) to specify/request the SD card interface condition (ex: indicate what voltage the host runs at).
    //0x000001AA --> VHS = 0001b = 2.7V to 3.6V.  The 0xAA LSB is the check pattern, and is arbitrary, but 0xAA is recommended (good blend of 0's and '1's).
    //The SD card has to echo back the check pattern correctly however, in the R7 response.
    //If the SD card doesn't support the operating voltage range of the host, then it may not respond.
    //If it does support the range, it will respond with a type R7 reponse packet (6 bytes/48 bits).	        
    //Additionally, if the SD card is MMC or SD card v1.x spec device, then it may respond with
    //invalid command.  If it is a v2.0 spec SD card, then it is mandatory that the card respond
    //to CMD8.
    response = SendMediaSlowCmd(SEND_IF_COND, 0x1AA);   //Note: If changing "0x1AA", CRC value in table must also change.
    if(((response.r7.bytewise.argument._returnVal & 0xFFF) == 0x1AA) && (!response.r7.bitwise.bits.ILLEGAL_CMD))
   	{
        //If we get to here, the device supported the CMD8 command and didn't complain about our host
        //voltage range.
        //Most likely this means it is either a v2.0 spec standard or high capacity SD card (SDHC)
        #ifdef __DEBUG_UART  
        PrintROMASCIIStringUART("Media successfully processed CMD8. Response = ");
        PrintRAMBytesUART(((unsigned char*)&response + 1), 4);
        UARTSendLineFeedCarriageReturn();
        #endif

		//Send CMD58 (Read OCR [operating conditions register]).  Reponse type is R3, which has 5 bytes.
		//Byte 4 = normal R1 response byte, Bytes 3-0 are = OCR register value.
        #ifdef __DEBUG_UART  
        PrintROMASCIIStringUART("Sending CMD58.\r\n");
        #endif
        response = SendMediaSlowCmd(READ_OCR, 0x0);
        //Now that we have the OCR register value in the reponse packet, we could parse
        //the register contents and learn what voltage the SD card wants to run at.
        //If our host circuitry has variable power supply capability, it could 
        //theoretically adjust the SD card Vdd to the minimum of the OCR to save power.
		
		//Now send CMD55/ACMD41 in a loop, until the card is finished with its internal initialization.
		//Note: SD card specs recommend >= 1 second timeout while waiting for ACMD41 to signal non-busy.
		for(timeout = 0; timeout < 0xFFFF; timeout++)
		{				
			//Send CMD55 (lets SD card know that the next command is application specific (going to be ACMD41)).
			SendMediaSlowCmd(APP_CMD, 0x00000000);
			
			//Send ACMD41.  This is to check if the SD card is finished booting up/ready for full frequency and all
			//further commands.  Response is R3 type (6 bytes/48 bits, middle four bytes contain potentially useful data).
            //Note: When sending ACMD41, the HCS bit is bit 30, and must be = 1 to tell SD card the host supports SDHC
			response = SendMediaSlowCmd(SD_SEND_OP_COND,0x40000000); //bit 30 set
			
			//The R1 response should be = 0x00, meaning the card is now in the "standby" state, instead of
			//the "idle" state (which is the default initialization state after CMD0 reset is issued).  Once
			//in the "standby" state, the SD card is finished with basic intitialization and is ready 
			//for read/write and other commands.
			if(response.r1._byte == 0)
			{
    		    #ifdef __DEBUG_UART  
                PrintROMASCIIStringUART("Media successfully processed CMD55/ACMD41 and is no longer busy.\r\n");
				#endif
				break;  //Break out of for() loop.  Card is finished initializing.
            }				
		}		
		if(timeout >= 0xFFFF)
		{
            #ifdef __DEBUG_UART  
            PrintROMASCIIStringUART("Media Timeout on CMD55/ACMD41.\r\n");
            #endif
    		mediaInformation.errorCode = MEDIA_CANNOT_INITIALIZE;
        }				
		
		
        //Now send CMD58 (Read OCR register).  The OCR register contains important
        //info we will want to know about the card (ex: standard capacity vs. SDHC).
        response = SendMediaSlowCmd(READ_OCR, 0x0); 

		//Now check the CCS bit (OCR bit 30) in the OCR register, which is in our response packet.
		//This will tell us if it is a SD high capacity (SDHC) or standard capacity device.
		if(response.r7.bytewise.argument._returnVal & 0x40000000)    //Note the HCS bit is only valid when the busy bit is also set (indicating device ready).
		{
			gSDMode = SD_MODE_HC;
			
		    #ifdef __DEBUG_UART  
            PrintROMASCIIStringUART("Media successfully processed CMD58: SD card is SDHC v2.0 (or later) physical spec type.\r\n");
            #endif
        }				
        else
        {
            gSDMode = SD_MODE_NORMAL;

            #ifdef __DEBUG_UART  
            PrintROMASCIIStringUART("Media successfully processed CMD58: SD card is standard capacity v2.0 (or later) spec.\r\n");
            #endif
        } 
        //SD Card should now be finished with initialization sequence.  Device should be ready
        //for read/write commands.

	}//if(((response.r7.bytewise._returnVal & 0xFFF) == 0x1AA) && (!response.r7.bitwise.bits.ILLEGAL_CMD))
    else
	{
        //The CMD8 wasn't supported.  This means the card is not a v2.0 card.
        //Presumably the card is v1.x device, standard capacity (not SDHC).

        #ifdef __DEBUG_UART  
        PrintROMASCIIStringUART("CMD8 Unsupported: Media is most likely MMC or SD 1.x device.\r\n");
        #endif


        SD_CS = 1;                              // deselect the devices
        Delayms(1);
        SD_CS = 0;                              // select the device

        //The CMD8 wasn't supported.  This means the card is definitely not a v2.0 SDHC card.
        gSDMode = SD_MODE_NORMAL;
    	// According to the spec CMD1 must be repeated until the card is fully initialized
    	timeout = 0x1FFF;
        do
        {
            //Send CMD1 to initialize the media.
            response = SendMediaSlowCmd(SEND_OP_COND, 0x00000000);    //When argument is 0x00000000, this queries MMC cards for operating voltage range
            timeout--;
        }while((response.r1._byte != 0x00) && (timeout != 0));
        // see if it failed
        if(timeout == 0)
        {
            #ifdef __DEBUG_UART  
            PrintROMASCIIStringUART("CMD1 failed.\r\n");
            #endif

            mediaInformation.errorCode = MEDIA_CANNOT_INITIALIZE;
            SD_CS = 1;                              // deselect the devices
        }
        else
        {
            #ifdef __DEBUG_UART  
            PrintROMASCIIStringUART("CMD1 Successfully processed, media is no longer busy.\r\n");
            #endif
            
            //Set read/write block length to 512 bytes.  Note: commented out since
            //this theoretically isn't necessary, since all cards v1 and v2 are 
            //required to support 512 byte block size, and this is supposed to be
            //the default size selected on cards that support other sizes as well.
            //response = SendMediaSlowCmd(SET_BLOCKLEN, 0x00000200);    //Set read/write block length to 512 bytes
        }
       
	}


    //Temporarily deselect device
    SD_CS = 1;
    
    //Basic initialization of media is now complete.  The card will now use push/pull
    //outputs with fast drivers.  Therefore, we can now increase SPI speed to 
    //either the maximum of the microcontroller or maximum of media, whichever 
    //is slower.  MMC media is typically good for at least 20Mbps SPI speeds.  
    //SD cards would typically operate at up to 25Mbps or higher SPI speeds.
    OpenSPIM(SYNC_MODE_FAST);

	SD_CS = 0;

	/* Send the CMD9 to read the CSD register */
    timeout = NCR_TIMEOUT;
    do
    {
        //Send CMD9: Read CSD data structure.
		response = SendMMCCmd(SEND_CSD, 0x00);
        timeout--;
    }while((response.r1._byte != 0x00) && (timeout != 0));
    if(timeout != 0x00)
    {
        #ifdef __DEBUG_UART  
        PrintROMASCIIStringUART("CMD9 Successfully processed: Read CSD register.\r\n");
        PrintROMASCIIStringUART("CMD9 response R1 byte = ");
        PrintRAMBytesUART((unsigned char*)&response, 1); 
        UARTSendLineFeedCarriageReturn();
        #endif
    }    
    else
    {
        //Media failed to respond to the read CSD register operation.
        #ifdef __DEBUG_UART  
        PrintROMASCIIStringUART("Timeout occurred while processing CMD9 to read CSD register.\r\n");
        #endif
        
        mediaInformation.errorCode = MEDIA_CANNOT_INITIALIZE;
        SD_CS = 1;
        return &mediaInformation;
    }    

	/* According to the simplified spec, section 7.2.6, the card will respond
	with a standard response token, followed by a data block of 16 bytes
	suffixed with a 16-bit CRC.*/
	index = 0;
	for (count = 0; count < 20u; count ++)
	{
		CSDResponse[index] = MDD_SDSPI_ReadMedia();
		index++;			
		/* Hopefully the first byte is the datatoken, however, some cards do
		not send the response token before the CSD register.*/
		if((count == 0) && (CSDResponse[0] == DATA_START_TOKEN))
		{
			/* As the first byte was the datatoken, we can drop it. */
			index = 0;
		}
	}

    #ifdef __DEBUG_UART  
    PrintROMASCIIStringUART("CSD data structure contains: ");
    PrintRAMBytesUART((unsigned char*)&CSDResponse, 20); 
    UARTSendLineFeedCarriageReturn();
    #endif
    


	//Extract some fields from the response for computing the card capacity.
	//Note: The structure format depends on if it is a CSD V1 or V2 device.
	//Therefore, need to first determine version of the specs that the card 
	//is designed for, before interpreting the individual fields.

	//-------------------------------------------------------------
	//READ_BL_LEN: CSD Structure v1 cards always support 512 byte
	//read and write block lengths.  Some v1 cards may optionally report
	//READ_BL_LEN = 1024 or 2048 bytes (and therefore WRITE_BL_LEN also 
	//1024 or 2048).  However, even on these cards, 512 byte partial reads
	//and 512 byte write are required to be supported.
	//On CSD structure v2 cards, it is always required that READ_BL_LEN 
	//(and therefore WRITE_BL_LEN) be 512 bytes, and partial reads and
	//writes are not allowed.
	//Therefore, all cards support 512 byte reads/writes, but only a subset
	//of cards support other sizes.  For best compatibility with all cards,
	//and the simplest firmware design, it is therefore preferrable to 
	//simply ignore the READ_BL_LEN and WRITE_BL_LEN values altogether,
	//and simply hardcode the read/write block size as 512 bytes.
	//-------------------------------------------------------------
	gMediaSectorSize = 512u;
	//mediaInformation.sectorSize = gMediaSectorSize;
	mediaInformation.sectorSize = 512u;
	mediaInformation.validityFlags.bits.sectorSize = TRUE;
	//-------------------------------------------------------------

	//Calculate the MDD_SDSPI_finalLBA (see SD card physical layer simplified spec 2.0, section 5.3.2).
	//In USB mass storage applications, we will need this information to 
	//correctly respond to SCSI get capacity requests.  Note: method of computing 
	//MDD_SDSPI_finalLBA depends on CSD structure spec version (either v1 or v2).
	if(CSDResponse[0] & 0xC0)	//Check CSD_STRUCTURE field for v2+ struct device
	{
		//Must be a v2 device (or a reserved higher version, that doesn't currently exist)

		//Extract the C_SIZE field from the response.  It is a 22-bit number in bit position 69:48.  This is different from v1.  
		//It spans bytes 7, 8, and 9 of the response.
		c_size = (((DWORD)CSDResponse[7] & 0x3F) << 16) | ((WORD)CSDResponse[8] << 8) | CSDResponse[9];
		
		MDD_SDSPI_finalLBA = ((DWORD)(c_size + 1) * (WORD)(1024u)) - 1; //-1 on end is correction factor, since LBA = 0 is valid.
	}
	else //if(CSDResponse[0] & 0xC0)	//Check CSD_STRUCTURE field for v1 struct device
	{
		//Must be a v1 device.
		//Extract the C_SIZE field from the response.  It is a 12-bit number in bit position 73:62.  
		//Although it is only a 12-bit number, it spans bytes 6, 7, and 8, since it isn't byte aligned.
		c_size = ((DWORD)CSDResponse[6] << 16) | ((WORD)CSDResponse[7] << 8) | CSDResponse[8];	//Get the bytes in the correct positions
		c_size &= 0x0003FFC0;	//Clear all bits that aren't part of the C_SIZE
		c_size = c_size >> 6;	//Shift value down, so the 12-bit C_SIZE is properly right justified in the DWORD.
		
		//Extract the C_SIZE_MULT field from the response.  It is a 3-bit number in bit position 49:47.
		c_size_mult = ((WORD)((CSDResponse[9] & 0x03) << 1)) | ((WORD)((CSDResponse[10] & 0x80) >> 7));

        //Extract the BLOCK_LEN field from the response. It is a 4-bit number in bit position 83:80.
        block_len = CSDResponse[5] & 0x0F;

        block_len = 1 << (block_len - 9); //-9 because we report the size in sectors of 512 bytes each
		
		//Calculate the MDD_SDSPI_finalLBA (see SD card physical layer simplified spec 2.0, section 5.3.2).
		//In USB mass storage applications, we will need this information to 
		//correctly respond to SCSI get capacity requests (which will cause MDD_SDSPI_ReadCapacity() to get called).
		MDD_SDSPI_finalLBA = ((DWORD)(c_size + 1) * (WORD)((WORD)1 << (c_size_mult + 2)) * block_len) - 1;	//-1 on end is correction factor, since LBA = 0 is valid.		
	}	

    //Turn off CRC7 if we can, might be an invalid cmd on some cards (CMD59)
    //Note: POR default for the media is normally with CRC checking off in SPI 
    //mode anyway, so this is typically redundant.
    SendMMCCmd(CRC_ON_OFF,0x0);

    //Now set the block length to media sector size. It should be already set to this.
    SendMMCCmd(SET_BLOCKLEN,gMediaSectorSize);

    //Deselect media while not actively accessing the card.
    SD_CS = 1;

    #ifdef __DEBUG_UART  
    PrintROMASCIIStringUART("Returning from MediaInitialize() function.\r\n");
    #endif


    return &mediaInformation;
}//end MediaInitialize

