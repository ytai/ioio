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
#include <uart2.h>
#include <p24fxxxx.h>
#include <pps.h>


#include "blapi/adb_file.h"
#include "blapi/bootloader.h"

#include "Compiler.h"

typedef enum {
  STATE_READ_FILE,
  STATE_DONE
} STATE;

STATE state;

void FileCallback(ADB_FILE_HANDLE f, const void* data, UINT32 data_len) {
  if (data) {
    UINT32 i;
    UART2PrintString("***** Got file data *******\r\n");
    for (i = 0; i < data_len; ++i) {
      UART2PutHex(((const BYTE*) data)[i]);
      UART2PutChar(' ');
    }
  } else {
    if (data_len == 0) {
       UART2PrintString("***** EOF *******\r\n");
    } else {
       UART2PrintString("***** Error *******\r\n");
    }
    state = STATE_DONE;
  }
}

int main() {
  ADB_FILE_HANDLE f;
  iPPSInput(IN_FN_PPS_U2RX,IN_PIN_PPS_RP2);       //Assign U2RX to pin RP2 (42)
  iPPSOutput(OUT_PIN_PPS_RP4,OUT_FN_PPS_U2TX);    //Assign U2TX to pin RP4 (43)
  UART2Init();

  UART2PrintString("***** Hello from app! *******\r\n");

  f = ADBFileRead("/data/data/ioio.manager/files/image.ioio", &FileCallback);
  UART2PrintString("***** file handle: ");
  UART2PutHexWord(f);
  UART2PrintString("*****\r\n");

  state = STATE_READ_FILE;
  
  while (state == STATE_READ_FILE) {
    BootloaderTasks();
  }
  

  // blink LED
  TRISFbits.TRISF3 = 0;
  while (1) {
    long i = 1000L;
    LATFbits.LATF3 = 0;
    while (i--);
    i = 1000000L;
    LATFbits.LATF3 = 1;
    while (i--);
    BootloaderTasks();
  }
}
