//
// Sample usage and test of the ADB layer.
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "timer.h"
#include "GenericTypeDefs.h"
#include "adb.h"
#include "HardwareProfile.h"
#include "logging.h"

// *****************************************************************************
// *****************************************************************************
// Configuration Bits
// *****************************************************************************
// *****************************************************************************

#ifdef __C30__
  #if defined(__PIC24FJ256DA206__)
      _CONFIG1(FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
      _CONFIG2(POSCMOD_NONE & IOL1WAY_ON & OSCIOFNC_OFF & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL96MHZ_ON & PLLDIV_NODIV & IESO_OFF)
      _CONFIG3(0xFFFF)
  #else
      #error Procesor undefined
  #endif

#elif defined(__PIC32MX__)
  #pragma config UPLLEN   = ON            // USB PLL Enabled
  #pragma config FPLLMUL  = MUL_15        // PLL Multiplier
  #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
  #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
  #pragma config FPLLODIV = DIV_1         // PLL Output Divider
  #pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
  #pragma config FWDTEN   = OFF           // Watchdog Timer
  #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
  #pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
  #pragma config OSCIOFNC = OFF           // CLKO Enable
  #pragma config POSCMOD  = HS            // Primary Oscillator
  #pragma config IESO     = OFF           // Internal/External Switch-over
  #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable (KLO was off)
  #pragma config FNOSC    = PRIPLL        // Oscillator Selection
  #pragma config CP       = OFF           // Code Protect
  #pragma config BWP      = OFF           // Boot Flash Write Protect
  #pragma config PWP      = OFF           // Program Flash Write Protect
  #pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
  #pragma config DEBUG    = ON            // Background Debugger Enable
#else
  #error Cannot define configuration bits.
#endif


BOOL InitializeSystem(void) {
#if defined(__PIC24FJ256DA206__)
  #ifdef DEBUG_MODE
    iPPSInput(IN_FN_PPS_U2RX,IN_PIN_PPS_RP2);       //Assign U2RX to pin RP2 (42)
    iPPSOutput(OUT_PIN_PPS_RP4,OUT_FN_PPS_U2TX);    //Assign U2TX to pin RP4 (43)
    UART2Init();
  #endif
#elif defined(__PIC32MX__)
  {
    int  value = SYSTEMConfigWaitStatesAndPB(GetSystemClock());
    // Enable the cache for the best performance
    CheKseg0CacheOn();
    INTEnableSystemMultiVectoredInt();
    value = OSCCON;
    while (!(value & 0x00000020)) {
        value = OSCCON;    // Wait for PLL lock to stabilize
    }
  }
#endif

    mInitAllLEDs();

    return TRUE;
}  // InitializeSystem


typedef enum {
  MAIN_STATE_WAIT_CONNECT,
  MAIN_STATE_WAIT_OPEN,
  MAIN_STATE_RUN
} MAIN_STATE;

static char filepath[] = "/data/data/ioio.filegen/files/test_file";
static MAIN_STATE state = MAIN_STATE_WAIT_CONNECT;

#define MKID(a,b,c,d) (((UINT32) (a)) | (((UINT32) (b)) << 8) | (((UINT32) (c)) << 16) | (((UINT32) (d)) << 24))

#define ID_STAT MKID('S','T','A','T')
#define ID_LIST MKID('L','I','S','T')
#define ID_ULNK MKID('U','L','N','K')
#define ID_SEND MKID('S','E','N','D')
#define ID_RECV MKID('R','E','C','V')
#define ID_DENT MKID('D','E','N','T')
#define ID_DONE MKID('D','O','N','E')
#define ID_DATA MKID('D','A','T','A')
#define ID_OKAY MKID('O','K','A','Y')
#define ID_FAIL MKID('F','A','I','L')
#define ID_QUIT MKID('Q','U','I','T')

typedef union {
    UINT32 id;
    struct {
        UINT32 id;
        UINT32 namelen;
    } req;
    struct {
        UINT32 id;
        UINT32 mode;
        UINT32 size;
        UINT32 time;
    } stat;
    struct {
        UINT32 id;
        UINT32 mode;
        UINT32 size;
        UINT32 time;
        UINT32 namelen;
    } dent;
    struct {
        UINT32 id;
        UINT32 size;
    } data;
    struct {
        UINT32 id;
        UINT32 msglen;
    } status;    
} syncmsg;

static syncmsg msg;


#define BUFFER_LEN 128
static BYTE buffer[BUFFER_LEN];
static UINT16 buffer_read = 0;
static UINT16 buffer_write = 0;
static UINT16 buffer_used = 0;

void BufferEnqueue(const void* data, UINT16 len) {
  assert(BUFFER_LEN - buffer_used >= len);
  if (buffer_write + len > BUFFER_LEN) {
    memcpy(buffer + buffer_write, data, BUFFER_LEN - buffer_write);
    buffer_write += len - BUFFER_LEN;
    memcpy(buffer, ((const BYTE*) data) + len - buffer_write, buffer_write);
  } else {
    memcpy(buffer + buffer_write, data, len);
    buffer_write += len;
    if (buffer_write == BUFFER_LEN) buffer_write = 0;
  }
  buffer_used += len;
}

void BufferDequeue(void* data, UINT16 len) {
  assert(len <= buffer_used);
  if (buffer_read + len > BUFFER_LEN) {
    memcpy(data, buffer + buffer_read, BUFFER_LEN - buffer_read);
    buffer_read += len - BUFFER_LEN;
    memcpy(((BYTE*) data) + len - buffer_read, buffer, buffer_read);
  } else {
    memcpy(data, buffer + buffer_read, len);
    buffer_read += len;
    if (buffer_read == BUFFER_LEN) buffer_read = 0;
  }
  buffer_used -= len;
}

void ChannelRecv(ADB_CHANNEL_HANDLE h, const void* data, UINT32 data_len) {
  BufferEnqueue(data, (UINT16) data_len);
}

void BlockingRecv(void* data, UINT32 len) {
  while(buffer_used < len && ADBTasks()); 
  BufferDequeue(data, len);
}

void BlockingSend(ADB_CHANNEL_HANDLE h, const void* data, UINT32 len) {
  ADBWrite(h, data, len);
  while (!ADBChannelReady(h) && ADBTasks());
}

static BYTE data[128];

int main(void) {
  ADB_CHANNEL_HANDLE h;
  // Initialize the processor and peripherals.
  if (!InitializeSystem()) {
    UART2PrintString("\r\n\r\nCould not initialize USB Custom Demo App - system.  Halting.\r\n\r\n");
    while (1);
  }
  ADBInit();

  while (1) {
    BOOL connected = ADBTasks();
    if (!connected) {
      state = MAIN_STATE_WAIT_CONNECT;
    }
    
    switch(state) {
     case MAIN_STATE_WAIT_CONNECT:
      if (connected) {
        print0("ADB connected!");
        h = ADBOpen("sync:", &ChannelRecv);
        state = MAIN_STATE_WAIT_OPEN;
      }
      break;

     case MAIN_STATE_WAIT_OPEN:
      if (ADBChannelReady(h)) {
        state = MAIN_STATE_RUN;
      }
      break;

     case MAIN_STATE_RUN:
      msg.req.id = ID_RECV;
      msg.req.namelen = strlen(filepath);
      BlockingSend(h, &msg, sizeof msg.req);
      BlockingSend(h, filepath, msg.req.namelen);
      do {
        BlockingRecv(&msg.data, sizeof msg.data);
        assert(msg.data.id == ID_DATA || msg.data.id == ID_DONE);
        assert(msg.data.size <= 4096);
        BlockingRecv(data, msg.data.size);
        print_message(data, msg.data.size);
      } while (msg.id != ID_DONE);
      break;
    }
  }
  return 0;
}  // main
