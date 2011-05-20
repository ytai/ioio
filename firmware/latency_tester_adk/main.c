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
#include <p24Fxxxx.h>
#include <PPS.h>
#include <string.h>

#include "blapi/adk.h"
#include "blapi/bootloader.h"

#include "Compiler.h"

typedef enum {
  MAIN_STATE_WAIT_CONNECT,
  MAIN_STATE_RECV_SIZES,
  MAIN_STATE_RECV_COMMAND,
  MAIN_STATE_TEST_UPLOAD,
  MAIN_STATE_TEST_DOWNLOAD,
  MAIN_STATE_TEST_BOTH,
  MAIN_STATE_TEST_LATENCY,
  MAIN_STATE_ERROR
} MAIN_STATE;

MAIN_STATE main_state = MAIN_STATE_WAIT_CONNECT;

#define ACK_BYTE 'A'
BOOL send_byte = FALSE;
int to_send = ACK_BYTE;

const void* data_to_send = NULL;
UINT32 data_to_send_len = 0;

char recv_sizes_buf[4];
int sizes_bytes_remaining = 0;

unsigned long packets_per_test = 0;
unsigned long packet_size = 0;
unsigned long remaining_upload_size = 0;
unsigned long remaining_download_packets = 0;
BOOL one_direction_finished = FALSE;

void recv_sizes(const char* data) {
  packet_size = ((int) data[0]) << 8 | data[1];
  packets_per_test = ((int) data[2]) << 8 | data[3];
}

BOOL recv_command(char command_id) {
  switch (command_id) {
    case 'U':
      UART2PrintString("Starting upload test... ");
      remaining_upload_size = packet_size * packets_per_test;
      main_state = MAIN_STATE_TEST_UPLOAD;
      return TRUE;

    case 'D':
      UART2PrintString("Starting download test... ");
      remaining_download_packets = packets_per_test;
      main_state = MAIN_STATE_TEST_DOWNLOAD;
      return TRUE;

    case 'B':
      UART2PrintString("Starting both test... ");
      remaining_upload_size = packet_size * packets_per_test;
      remaining_download_packets = packets_per_test;
      one_direction_finished = FALSE;
      main_state = MAIN_STATE_TEST_BOTH;
      return TRUE;

    case 'L':
      UART2PrintString("Starting latency test... ");
      remaining_download_packets = packets_per_test;
      main_state = MAIN_STATE_TEST_LATENCY;
      return TRUE;

    default:
      return FALSE;
  }
}

void HandleIncoming(const BYTE* data, DWORD data_len) {
  while (data_len > 0) {
    switch (main_state) {
      case MAIN_STATE_RECV_SIZES:
        if (data_len < sizes_bytes_remaining) {
          memcpy(recv_sizes_buf + 4 - sizes_bytes_remaining, data, data_len);
          data_len = 0;
        } else {
          memcpy(recv_sizes_buf + 4 - sizes_bytes_remaining, data, sizes_bytes_remaining);
          data_len -= sizes_bytes_remaining;
          data += sizes_bytes_remaining;
          recv_sizes(recv_sizes_buf);
          main_state = MAIN_STATE_RECV_COMMAND;
          send_byte = TRUE;
          to_send = ACK_BYTE;
          UART2PrintString("Sizes recieved.\r\n");
        }
        break;

      case MAIN_STATE_RECV_COMMAND:
        if (!recv_command(data[0])) {
          main_state = MAIN_STATE_ERROR;
        } else {
          --data_len;
          ++data;
          send_byte = TRUE;
          to_send = ACK_BYTE;
        }
        break;

      case MAIN_STATE_TEST_UPLOAD:
        if (data_len < remaining_upload_size) {
          remaining_upload_size -= data_len;
          data_len = 0;
        } else {
          data_len -= remaining_upload_size;
          data += remaining_upload_size;
          remaining_upload_size = 0;
          send_byte = TRUE;
          to_send = ACK_BYTE;
          main_state = MAIN_STATE_RECV_COMMAND;
          UART2PrintString("Done upload test\r\n");
        }
        break;

      case MAIN_STATE_TEST_BOTH:
        if (data_len < remaining_upload_size) {
          remaining_upload_size -= data_len;
          data_len = 0;
        } else {
          data_len -= remaining_upload_size;
          data += remaining_upload_size;
          remaining_upload_size = 0;
          UART2PrintString("upload done... ");
         if (one_direction_finished) {
            send_byte = TRUE;
            to_send = ACK_BYTE;
            main_state = MAIN_STATE_RECV_COMMAND;
            UART2PrintString("Done both test\r\n");
          }
          one_direction_finished = TRUE;
        }
        break;

      case MAIN_STATE_TEST_LATENCY:
        if (data_len != 1) {
          main_state = MAIN_STATE_ERROR;
        } else {
          send_byte = TRUE;
          to_send = ((const BYTE*) data)[0];
          data_len = 0;
          if (--remaining_download_packets == 0) {
            main_state = MAIN_STATE_RECV_COMMAND;
            UART2PrintString("Done latency test\r\n");
          }
        }
        break;

      default:
        break;
    }
  }
}

static BYTE __attribute__((far)) download_buffer[4096];
static BYTE __attribute__((far)) incoming_buffer[1024];

int main() {
  int write_ready = 1, read_ready = 1;
  int i;
  BYTE error;
  iPPSOutput(OUT_PIN_PPS_RP28,OUT_FN_PPS_U2TX);    //Assign U2TX to pin RP4 (43 on the PIC, 4 on IOIO V1.0 board)
  UART2Init();

  UART2PrintString("***** Hello from Latency Tester! *******\r\n");

  for (i = 0; i < sizeof download_buffer; ++i) {
    download_buffer[i] = i % 256;
  }

  while (1) {
    BOOL connected;

    BootloaderTasks();
    connected = ADKAttached();
    if (!connected) {
      main_state = MAIN_STATE_WAIT_CONNECT;
      break;
    } else {
      if (read_ready) {
        ADKRead(incoming_buffer, sizeof incoming_buffer);
        read_ready = 0;
      } else {
        DWORD num_read;
        if (ADKReadDone(&error, &num_read)) {
          HandleIncoming(incoming_buffer, num_read);
          read_ready = 1;
        }
      }
      
      if (write_ready) {
        if (send_byte) {
          ADKWrite(&to_send, 1);
          send_byte = FALSE;
          write_ready = 0;
        }
      } else {
        // check if a previous write completed
        if (ADKWriteDone(&error)) {
          write_ready = 1;
          if (error == 0) {
//            UART2PrintString("OK\r\n");
          } else {
            UART2PrintString("Write failed eventually with code: 0x");
            UART2PutHex(error);
            UART2PrintString("\r\n");
          }
        }
      }
    }

    switch (main_state) {
      case MAIN_STATE_WAIT_CONNECT:
        if (connected) {
          sizes_bytes_remaining = 4;
          main_state = MAIN_STATE_RECV_SIZES;
        }
        break;

      case MAIN_STATE_TEST_DOWNLOAD:
        if (remaining_download_packets > 0) {
          if (write_ready) {
            int res = ADKWrite(download_buffer, packet_size);
            if (0 == res) {
//              UART2PrintString("Wrote 0x");
//              UART2PutHexWord(size);
//              UART2PrintString("\r\n");
              write_ready = 0;
              --remaining_download_packets;
            } else {
              UART2PrintString("Write failed immediately with code: 0x");
              UART2PutHex(res);
              UART2PrintString("\r\n");
            }
          }
        } else {
          send_byte = TRUE;
          to_send = ACK_BYTE;
          main_state = MAIN_STATE_RECV_COMMAND;
          UART2PrintString("Done download test\r\n");
        }
        break;

      case MAIN_STATE_TEST_BOTH:
        if (remaining_download_packets > 0) {
          if (write_ready) {
            int res = ADKWrite(download_buffer, packet_size);
            if (0 == res) {
              write_ready = 0;
              --remaining_download_packets;
            } else {
              UART2PrintString("Write failed immediately with code: 0x");
              UART2PutHex(res);
              UART2PrintString("\r\n");
            }
          }
        } else {
          UART2PrintString("download done... ");
          if (one_direction_finished) {
            send_byte = TRUE;
            to_send = ACK_BYTE;
            main_state = MAIN_STATE_RECV_COMMAND;
            UART2PrintString("Done both test\r\n");
          }
          one_direction_finished = TRUE;
        }
        break;

      case MAIN_STATE_ERROR:
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
        break;

      default:
        break;
    }
  }

  return 0;
}
