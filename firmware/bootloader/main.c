// Bootloader main

#include "GenericTypeDefs.h"
#include "bootloader_private.h"
#include "blapi/adb.h"
#include "blapi/adb_file.h"
#include "HardwareProfile.h"
#include "logging.h"
#include "ioio_file.h"

#ifdef ENABLE_LOGGING
#include "uart2.h"
#endif

// *****************************************************************************
// *****************************************************************************
// Configuration Bits
// *****************************************************************************
// *****************************************************************************

#if defined(__PIC24FJ256DA206__) || defined(__PIC24FJ128DA106__)
  _CONFIG1(FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
  _CONFIG2(POSCMOD_NONE & IOL1WAY_ON & OSCIOFNC_OFF & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL96MHZ_ON & PLLDIV_NODIV & IESO_OFF)
  _CONFIG3(WPDIS_WPEN | WPFP_WPFP31 | WPCFG_WPCFGEN | WPEND_WPSTARTMEM)
#else
  #error Unsupported target
#endif

void InitializeSystem() {
#ifdef ENABLE_LOGGING
  iPPSInput(IN_FN_PPS_U2RX,IN_PIN_PPS_RP2);       //Assign U2RX to pin RP2 (42)
  iPPSOutput(OUT_PIN_PPS_RP4,OUT_FN_PPS_U2TX);    //Assign U2TX to pin RP4 (43)
  UART2Init();
#endif
  mInitAllLEDs();
}

typedef enum {
  MAIN_STATE_WAIT_CONNECT,
  MAIN_STATE_RECV,
  MAIN_STATE_DONE,
  MAIN_STATE_ERROR
} MAIN_STATE;

static MAIN_STATE state = MAIN_STATE_WAIT_CONNECT;


void FileRecv(ADB_FILE_HANDLE h, const void* data, UINT32 data_len) {
  if (data) {
    if (!IOIOFileHandleBuffer(data, data_len)) {
      state = MAIN_STATE_ERROR;
    }
  } else {
    if (data_len == 0 && IOIOFileDone()) {
      log_print_0("\r\n\r\nSuccessfully wrote application firmware image!\r\n\r\n");
      state = MAIN_STATE_DONE;
    } else {
      state = MAIN_STATE_ERROR;
    }
  }
}

int main() {
  ADB_FILE_HANDLE h;
  InitializeSystem();
  BootloaderInit();

  while (1) {
    BOOL connected = BootloaderTasks();
    mLED_0 = !connected;
    if (!connected) {
      state = MAIN_STATE_WAIT_CONNECT;
    }
    
    switch(state) {
     case MAIN_STATE_WAIT_CONNECT:
      if (connected) {
        log_print_0("ADB connected!");
        IOIOFileInit();
        h = ADBFileRead("/data/data/ioio.manager/files/image.ioio", &FileRecv);
        state = MAIN_STATE_RECV;
      }
      break;

     case MAIN_STATE_RECV:
      break;

     case MAIN_STATE_DONE:
      __asm__("goto __APP_RESET");
      break;

     case MAIN_STATE_ERROR:
      break;
    }
  }
  return 0;
}
