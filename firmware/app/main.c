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
