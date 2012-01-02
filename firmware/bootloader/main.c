/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */

// Bootloader main

#include <string.h>
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "board.h"
#include "bootloader_defs.h"
#include "libadb/adb.h"
#include "libadb/adb_file.h"
#include "bootloader_conn.h"
#include "flash.h"
#include "logging.h"
#include "ioio_file.h"
#include "dumpsys.h"
#include "auth.h"

//
// Desired behavior:
// 1. Wait for ADB connection.
// 2. Find directory of manager app. If not found, skip to step 8.
// 3. Attempt to read fingerprint file.
//    If read failed OR fingerprint is identical to the one we have, skip to
//    step 8.
// 4. Authenticate manager app. If failed, skip to step 8.
// 5. Erase fingerprint. If failed, signal error.
// 6. Read application image file and program Flash. If failed, signal error.
// 7. Program new fingerprint. If failed, signal error.
// 8. Run the application.
//
// When signaling error, only a reset gets us out of there.
// A lost connection at any point gets us back to step 1.


// *****************************************************************************
// *****************************************************************************
// Configuration Bits
// *****************************************************************************
// *****************************************************************************

#if defined(__PIC24FJ256DA206__) || defined(__PIC24FJ128DA106__) || defined(__PIC24FJ128DA206__)
  _CONFIG1(FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
  _CONFIG2(POSCMOD_NONE & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL96MHZ_ON & PLLDIV_NODIV & IESO_OFF)
  _CONFIG3(WPDIS_WPEN & WPFP_WPFP19 & WPCFG_WPCFGEN & WPEND_WPSTARTMEM & SOSCSEL_EC)
#else
  #error Unsupported target
#endif

// platform ID:
// the platform ID is what maintains compatibility between the application
// firmware image and the board / bootloader.
// any application firmware image is given a platform ID for which it was built,
// similarly, the bootloader holds a platform ID uniquely designating its own
// binary interface and the underlying hardware.
// the bootloader will only attempt to install a firmware image with a matching
// platform ID.
// boards that are completely electrically equivalent and have the same pin
// numbering scheme and the same bootloader interface, will have identical
// platform IDs.
//
// note that when BLAPI changes, this list will need to be completely rebuilt
// with new numbers per hardware version.
#if BOARD_VER == BOARD_SPRK0010
  #define PLATFORM_ID "IOIO0020"
#elif BOARD_VER >= BOARD_SPRK0011 && BOARD_VER <= BOARD_SPRK0012
  #define PLATFORM_ID "IOIO0021"
#elif BOARD_VER >= BOARD_SPRK0013 && BOARD_VER <= BOARD_SPRK0015
  #define PLATFORM_ID "IOIO0022"
#elif BOARD_VER == BOARD_SPRK0016
  #define PLATFORM_ID "IOIO0023"
#else
  #error Unknown board version - cannot determine platform ID
#endif

#define FINGERPRINT_SIZE 16
#define MAX_PATH 64

int pass_usb_to_app __attribute__ ((near, section("bootflag.sec"))) = 0;

void InitializeSystem() {
  mInitAllLEDs();
}

typedef enum {
  MAIN_STATE_WAIT_CONNECT,
  MAIN_STATE_WAIT_ADB_READY,
  MAIN_STATE_FIND_PATH,
  MAIN_STATE_FIND_PATH_DONE,
  MAIN_STATE_AUTH_MANAGER,
  MAIN_STATE_AUTH_PASSED,
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
static const char* manager_path;
char filepath[MAX_PATH];
static AUTH_RESULT auth_result;
static int led_counter = 0;

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
        log_printf("Fingerprint match - skipping download");
        state = MAIN_STATE_RUN_APP;
      } else {
        // if anything went wrong or fingerprint is different, force download
        log_printf("Fingerprint mismatch - downloading image");
        state = MAIN_STATE_FP_FAILED;
      }
    } else {
      // failed to read fingerprint file, probably doesn't exist, run whatever we have
      // if all went OK and the fingerprint matches, skip download and run app
      log_printf("Couldn't find firmware to install");
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
      log_printf("Successfully wrote application firmware image!");
      state = MAIN_STATE_RECV_IMAGE_DONE;
    } else {
      state = MAIN_STATE_ERROR;
    }
  }
}

void FileRecvPackages(ADB_FILE_HANDLE h, const void* data, UINT32 data_len) {
  if (data) {
    if (auth_result == AUTH_BUSY) {
      auth_result = AuthProcess(data, data_len);
      if (auth_result == AUTH_BUSY) {
        return;
      } else {
        ADBFileClose(h);
      }
    }
  }
  if (auth_result == AUTH_DONE_PASS) {
    log_printf("IOIO manager is authentic.");
    state = MAIN_STATE_AUTH_PASSED;
  } else {
    log_printf("IOIO manager authentication failed. Skipping download.");
    state = MAIN_STATE_RUN_APP;
  }
}

void RecvDumpsys(ADB_CHANNEL_HANDLE h, const void* data, UINT32 data_len) {
  if (data) {
    if (manager_path == DUMPSYS_BUSY) {
      manager_path = DumpsysProcess(data, data_len);
      if (manager_path == DUMPSYS_BUSY) {
        return;
      } else {
        ADBClose(h);
      }
    }
  }
  if (manager_path != DUMPSYS_BUSY && manager_path != DUMPSYS_ERROR) {
    log_printf("IOIO manager found with path %s", manager_path);
    state = MAIN_STATE_FIND_PATH_DONE;
  } else {
    log_printf("IOIO manager not found, skipping download");
    state = MAIN_STATE_RUN_APP;
  }
}

// When IOIO gets reset for an unexpected reason, its default behavior is to
// restart normally. This is desired in most cases.
// For debug purposes, however, we want to know what went wrong. With defining
// SIGANAL_AFTER_BAD_RESET, on any reset other than power-on, IOIO will blink
// a 16-bit code (long-1 short-0) to designtae the error.
// One very useful case is catching failed asserts. Their blink code will start
// with short-long-...
#ifdef SIGNAL_AFTER_BAD_RESET
static void Delay(unsigned long time) {
  while (time-- > 0);
}

static void SignalBit(int bit) {
  _LATF3 = 0;
  Delay(bit ? 900000UL : 100000UL);
  _LATF3 = 1;
  Delay(bit ? 100000UL : 900000UL);
}

static void SignalWord(unsigned int word) {
  int i;
  for (i = 0; i < 16; ++i) {
    SignalBit(word & 0x8000);
    word <<= 1;
  }
}

static void SignalRcon() {
  _LATF3 = 1;
  _TRISF3 = 0;
  while (1) {
   SignalWord(RCON);
   Delay(8000000UL);
  }
}
#endif

int main() {
  ADB_FILE_HANDLE f;
  ADB_CHANNEL_HANDLE h;
#ifdef SIGNAL_AFTER_BAD_RESET
  if (RCON & 0b1100001001000000) {
    SignalRcon();
  }
#endif
  log_init();
  log_printf("Hello from Bootloader!!!");
  InitializeSystem();
  BootloaderConnInit();

  while (1) {
    BOOL connected = BootloaderConnTasks();
    mLED_0 = (state == MAIN_STATE_ERROR) ? (led_counter++ >> 13) : !connected;
    if (!connected) {
      state = MAIN_STATE_WAIT_CONNECT;
    }

    switch(state) {
      case MAIN_STATE_WAIT_CONNECT:
        if (connected) {
          log_printf("Device connected!");
          if (ADBAttached()) {
            log_printf("ADB attached - attempting firmware upgrade");
            state = MAIN_STATE_WAIT_ADB_READY;
          } else {
            log_printf("ADB not attached - skipping boot sequence");
            state = MAIN_STATE_RUN_APP;
          }
        }
        break;

      case MAIN_STATE_WAIT_ADB_READY:
        if (ADBConnected()) {
            log_printf("ADB connected - starting boot sequence");
            manager_path = DUMPSYS_BUSY;
            DumpsysInit();
            h = ADBOpen("shell:dumpsys package ioio.manager", &RecvDumpsys);
            state = MAIN_STATE_FIND_PATH;
        }
        break;

      case MAIN_STATE_FIND_PATH_DONE:
        fingerprint_size = 0;
        strcpy(filepath, manager_path);
        strcat(filepath, "/files/" PLATFORM_ID ".fp");
        f = ADBFileRead(filepath, &FileRecvFingerprint);
        state = MAIN_STATE_WAIT_RECV_FP;
        break;

      case MAIN_STATE_FP_FAILED:
#ifdef BYPASS_SECURITY
        state = MAIN_STATE_AUTH_PASSED;
        break;
#endif
        auth_result = AUTH_BUSY;
        AuthInit();
        f = ADBFileRead("/data/system/packages.xml", &FileRecvPackages);
        state = MAIN_STATE_AUTH_MANAGER;
        break;

      case MAIN_STATE_AUTH_PASSED:
        if (!EraseFingerprint()) {
          state = MAIN_STATE_ERROR;
        } else {
          IOIOFileInit();
          strcpy(filepath, manager_path);
          strcat(filepath, "/files/" PLATFORM_ID ".ioio");
          f = ADBFileRead(filepath, &FileRecvImage);
          state = MAIN_STATE_WAIT_RECV_IMAGE;
        }
        break;

      case MAIN_STATE_RECV_IMAGE_DONE:
        if (WriteFingerprint()) {
          state = MAIN_STATE_RUN_APP;
        } else {
          state = MAIN_STATE_ERROR;
        }
        break;

      case MAIN_STATE_RUN_APP:
        BootloaderConnResetUSB();
        pass_usb_to_app = 1;
        log_printf("Running app...");
        __asm__("goto __APP_RESET");

      default:
        break;  // do nothing
    }
  }
  return 0;
}
