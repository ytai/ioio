// Bootloader main

#include <string.h>
#include "GenericTypeDefs.h"
#include "bootloader_private.h"
#include "bootloader_defs.h"
#include "blapi/adb.h"
#include "blapi/adb_file.h"
#include "blapi/flash.h"
#include "HardwareProfile.h"
#include "logging.h"
#include "ioio_file.h"

#ifdef ENABLE_LOGGING
#include "uart2.h"
#include "PPS.h"
#endif

//
// Desired behavior:
// 1. Wait for ADB connection.
// 2. Attempt to read fingerprint file.
//    If read failed OR fingerprint is identical to the one we have, skip to
//    step 6.
// 3. Erase fingerprint.
// 4. Read application image file and program Flash.
// 5. Program new fingerprint.
// 6. Run the application.
//
// If anything goes wrong on the way, go back to step 1.


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

#define FINGERPRINT_SIZE 16

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
  MAIN_STATE_WAIT_RECV_FP,
  MAIN_STATE_FP_FAILED,
  MAIN_STATE_WAIT_RECV_IMAGE,
  MAIN_STATE_RECV_IMAGE_DONE,
  MAIN_STATE_RUN_APP,
  MAIN_STATE_ERROR
} MAIN_STATE;

static MAIN_STATE state = MAIN_STATE_WAIT_CONNECT;
static int fingerprint_size;
static BYTE fingerprint[FINGERPRINT_SIZE];

BOOL ValidateFingerprint() {
  int i;
  DWORD addr = BOOTLOADER_FINGERPRINT_ADDRESS;
  BYTE* fp = fingerprint;
  for (i = 0; i < FINGERPRINT_SIZE / 2; ++i) {
    DWORD_VAL dw = {FlashReadDWORD(addr)};
    if (*fp++ != dw.byte.LB) return FALSE;
    if (*fp++ != dw.byte.HB) return FALSE;
    if (dw.word.HW != 0) return FALSE;
    addr += 2;
  }
  return TRUE;
}

BOOL EraseFingerprint() {
  return FlashErasePage(BOOTLOADER_FINGERPRINT_PAGE);
}

BOOL WriteFingerprint() {
  int i;
  DWORD addr = BOOTLOADER_FINGERPRINT_ADDRESS;
  BYTE* fp = fingerprint;
  for (i = 0; i < FINGERPRINT_SIZE / 2; ++i) {
    DWORD_VAL dw = {0};
    dw.byte.LB = *fp++;
    dw.byte.HB = *fp++;
    if (!FlashWriteDWORD(addr, dw.Val)) return FALSE;
    addr += 2;
  }
  return TRUE;
}

void FileRecvFingerprint(ADB_FILE_HANDLE h, const void* data, UINT32 data_len) {
  if (data) {
    if (fingerprint_size != -1 && data_len <= FINGERPRINT_SIZE - fingerprint_size) {
      memcpy(fingerprint + fingerprint_size, data, data_len);
      fingerprint_size += data_len;
    } else {
      fingerprint_size = -1;
    }
  } else {
    // EOF or error
    if (data_len == 0 && fingerprint_size == FINGERPRINT_SIZE) {
      if (ValidateFingerprint()) {
        // if all went OK and the fingerprint matches, skip download and run app
        log_print_0("Fingerprint match - skipping download");
        state = MAIN_STATE_RUN_APP;
      } else {
        // if anything went wrong or fingerprint is different, force download
        log_print_0("Fingerprint mismatch - downloading image");
        state = MAIN_STATE_FP_FAILED;
      }
    } else {
      // failed to read fingerprint file, probably doesn't exist, run whatever we have
      // if all went OK and the fingerprint matches, skip download and run app
      log_print_0("Couldn't find firmware to install");
      state = MAIN_STATE_RUN_APP;
    }
  }
}

void FileRecvImage(ADB_FILE_HANDLE h, const void* data, UINT32 data_len) {
  if (data) {
    if (!IOIOFileHandleBuffer(data, data_len)) {
      state = MAIN_STATE_ERROR;
    }
  } else {
    if (data_len == 0 && IOIOFileDone()) {
      log_print_0("Successfully wrote application firmware image!");
      state = MAIN_STATE_RECV_IMAGE_DONE;
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
        fingerprint_size = 0;
        h = ADBFileRead("/data/data/ioio.manager/files/image.fp", &FileRecvFingerprint);
        state = MAIN_STATE_WAIT_RECV_FP;
      }
      break;

     case MAIN_STATE_FP_FAILED:
      if (!EraseFingerprint()) {
        state = MAIN_STATE_ERROR;
      } else {
        IOIOFileInit();
        h = ADBFileRead("/data/data/ioio.manager/files/image.ioio", &FileRecvImage);
        state = MAIN_STATE_WAIT_RECV_IMAGE;
      }
      break;

     case MAIN_STATE_WAIT_RECV_FP:
     case MAIN_STATE_WAIT_RECV_IMAGE:
      break;

     case MAIN_STATE_RECV_IMAGE_DONE:
      if (WriteFingerprint()) {
        state = MAIN_STATE_RUN_APP;
      } else {
        state = MAIN_STATE_ERROR;
      }
      break;

     case MAIN_STATE_RUN_APP:
      __asm__("goto __APP_RESET");
      break;

     case MAIN_STATE_ERROR:
      break;
    }
  }
  return 0;
}
