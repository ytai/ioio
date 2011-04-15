/*
 * Copyright 2011. All rights reserved.
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

#include "blapi/adb_file.h"
#include "blapi/bootloader.h"

#include "Compiler.h"

typedef enum {
  MAIN_STATE_WAIT_CONNECT,
  MAIN_STATE_WAIT_READY,
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
BYTE to_send = ACK_BYTE;

const void* data_to_send = NULL;
UINT32 data_to_send_len = 0;

char recv_sizes_buf[4];
int sizes_bytes_recived = 0;

unsigned long packets_per_test = 0;
unsigned long packet_size = 0;
unsigned long total_upload_size = 0;
unsigned long total_download_packets = 0;
unsigned long total_latency_packets = 0;
unsigned long total_both_size = 0;
unsigned long total_both_packets = 0;
BOOL one_direction_finished = FALSE;

void recv_sizes(const char* data) {
  packet_size = data[0] << 8 | data[1];
  packets_per_test = data[2] << 8 | data[3];
}

BOOL recv_command(char command_id) {
  switch (command_id) {
    case 'U':
      UART2PrintString("Starting upload test... ");
      total_upload_size = 0;
      main_state = MAIN_STATE_TEST_UPLOAD;
      return TRUE;

    case 'D':
      UART2PrintString("Starting download test... ");
      total_download_packets = 0;
      main_state = MAIN_STATE_TEST_DOWNLOAD;
      return TRUE;

    case 'B':
      UART2PrintString("Starting both test... ");
      total_both_size = 0;
      total_both_packets = 0;
      one_direction_finished = FALSE;
      main_state = MAIN_STATE_TEST_BOTH;
      return TRUE;

    case 'L':
      UART2PrintString("Starting latency test... ");
      total_latency_packets = 0;
      main_state = MAIN_STATE_TEST_LATENCY;
      return TRUE;

    default:
      return FALSE;
  }
}

void ChannelRecv(ADB_CHANNEL_HANDLE h, const void* data, UINT32 data_len) {
  if (data == NULL && data_len == 0) {
    send_byte = FALSE;
    to_send = ACK_BYTE;
    data_to_send = NULL;
    data_to_send_len = 0;
    sizes_bytes_recived = 0;
    main_state = MAIN_STATE_WAIT_CONNECT;
    return;
  }
  switch (main_state) {
    case MAIN_STATE_RECV_SIZES:
      memcpy(recv_sizes_buf + sizes_bytes_recived, data, 4 - sizes_bytes_recived);
      sizes_bytes_recived += data_len;
      if (sizes_bytes_recived > 4) {
        main_state = MAIN_STATE_ERROR;
      } else if (sizes_bytes_recived == 4) {
        recv_sizes(recv_sizes_buf);
        main_state = MAIN_STATE_RECV_COMMAND;
        send_byte = TRUE;
        to_send = ACK_BYTE;
        UART2PrintString("Sizes recieved.\r\n");
      }
      break;
      
    case MAIN_STATE_RECV_COMMAND:
      if ((data_len != 1) || (recv_command(((const char*) data)[0]) == FALSE)) {
        main_state = MAIN_STATE_ERROR;
      } else {
        send_byte = TRUE;
        to_send = ACK_BYTE;
      }
      break;

    case MAIN_STATE_TEST_UPLOAD:
      total_upload_size += data_len;
      if (total_upload_size == (packet_size * packets_per_test)) {
        send_byte = TRUE;
        to_send = ACK_BYTE;
        main_state = MAIN_STATE_RECV_COMMAND;
        UART2PrintString("Done upload test\r\n");
      }
      break;

    case MAIN_STATE_TEST_BOTH:
      total_both_size += data_len;
      if (total_both_size == (packet_size * packets_per_test)) {
        if (one_direction_finished == TRUE) {
          send_byte = TRUE;
          to_send = ACK_BYTE;
          main_state = MAIN_STATE_RECV_COMMAND;
          UART2PrintString("Done both test\r\n");
        }
        one_direction_finished = TRUE;
      }
      break;

    case MAIN_STATE_TEST_LATENCY:
      total_latency_packets++;
      send_byte = TRUE;
      to_send = ((const BYTE*) data)[0];
      if (total_latency_packets == packets_per_test) {
        main_state = MAIN_STATE_RECV_COMMAND;
        UART2PrintString("Done latency test\r\n");
      }
      break;

    default:
      break;
  }
}

int main() {
  ADB_CHANNEL_HANDLE h;
  BYTE download_buffer[4096];
  int i;
  iPPSInput(IN_FN_PPS_U2RX,IN_PIN_PPS_RP2);       //Assign U2RX to pin RP2 (42 on the PIC, 3 on IOIO V1.0 board)
  iPPSOutput(OUT_PIN_PPS_RP4,OUT_FN_PPS_U2TX);    //Assign U2TX to pin RP4 (43 on the PIC, 4 on IOIO V1.0 board)
  UART2Init();

  UART2PrintString("***** Hello from Latency Tester! *******\r\n");

  for (i = 0; i < 4096; ++i) {
    download_buffer[i] = i % 256;
  }

  while (1) {
    BOOL connected = BootloaderTasks();
    if (!connected) {
      main_state = MAIN_STATE_WAIT_CONNECT;
      break;
    }

    if (ADBChannelReady(h)) {
      if ((data_to_send == NULL) && (data_to_send_len != 0)) {
        data_to_send_len = 0;
        ADBBufferUnref();
      } else if (data_to_send != NULL) {
        ADBWrite(h, data_to_send, data_to_send_len);
        data_to_send = NULL;
      } else if (send_byte) {
        ADBWrite(h, &to_send, 1);
        send_byte = FALSE;
      }
    }

    switch (main_state) {
      case MAIN_STATE_WAIT_CONNECT:
        if (connected) {
          h = ADBOpen("tcp:7149", &ChannelRecv);
          main_state = MAIN_STATE_WAIT_READY;
        }
        break;

      case MAIN_STATE_WAIT_READY:
        if (ADBChannelReady(h)) {
          UART2PrintString("Connected\r\n");
          main_state = MAIN_STATE_RECV_SIZES;
        }
        break;

      case MAIN_STATE_TEST_DOWNLOAD:
        if (total_download_packets < packets_per_test) {
          if (ADBChannelReady(h)) {
            ADBWrite(h, download_buffer, packet_size);
            total_download_packets += 1;
          }
        } else {
          send_byte = TRUE;
          to_send = ACK_BYTE;
          main_state = MAIN_STATE_RECV_COMMAND;
          UART2PrintString("Done download test\r\n");
        }
        break;

      case MAIN_STATE_TEST_BOTH:
        if (total_both_packets < packets_per_test) {
          if (ADBChannelReady(h)) {
            ADBWrite(h, download_buffer, packet_size);
            total_both_packets += 1;
          }
        } else {
          if (one_direction_finished == TRUE) {
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
