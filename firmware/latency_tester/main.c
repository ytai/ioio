#include <uart2.h>
#include <p24fxxxx.h>
#include <pps.h>

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

unsigned long packets_per_test = 0;
unsigned long packet_size = 0;
unsigned long total_upload_size = 0;
unsigned long total_download_packets = 0;
unsigned long total_latency_packets = 0;
unsigned long total_both_packets = 0;

void recv_sizes(const char* data) {
  packet_size = data[0] << 8 | data[1];
  packets_per_test = data[2] << 8 | data[3];
}

BOOL recv_command(char command_id) {
  switch (command_id) {
    case 'U':
      main_state = MAIN_STATE_TEST_UPLOAD;
      total_upload_size = 0;
      return TRUE;

    case 'D':
      main_state = MAIN_STATE_TEST_DOWNLOAD;
      total_download_packets = 0;
      return TRUE;

    case 'B':
      main_state = MAIN_STATE_TEST_BOTH;
      total_both_packets = 0;
      return TRUE;

    case 'L':
      main_state = MAIN_STATE_TEST_LATENCY;
      total_latency_packets = 0;
      return TRUE;

    default:
      return FALSE;
  }
}

void ChannelRecv(ADB_CHANNEL_HANDLE h, const void* data, UINT32 data_len) {
  UART2PrintString("Reading data");
  switch (main_state) {
    case MAIN_STATE_RECV_SIZES:
      if (data_len != 4) {
        main_state = MAIN_STATE_ERROR;
      } else {
        recv_sizes((const char*) data);
        main_state = MAIN_STATE_RECV_COMMAND;
        send_byte = TRUE;
        to_send = ACK_BYTE;
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
      }
      break;

    case MAIN_STATE_TEST_BOTH:
      total_both_packets++;
      ADBBufferRef();
      data_to_send = data;
      data_to_send_len = data_len;
      if (total_both_packets == packets_per_test) {
        send_byte = TRUE;
        to_send = ACK_BYTE;
        main_state = MAIN_STATE_RECV_COMMAND;
      }
      break;

    case MAIN_STATE_TEST_LATENCY:
      total_latency_packets++;
      send_byte = TRUE;
      to_send = ((const BYTE*) data)[0];
      if (total_latency_packets == packets_per_test) {
        main_state = MAIN_STATE_RECV_COMMAND;
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
          UART2PrintString("Connected");
          h = ADBOpen("tcp:7149", &ChannelRecv);
          main_state = MAIN_STATE_WAIT_READY;
        }
        break;

      case MAIN_STATE_WAIT_READY:
        if (ADBChannelReady(h)) {
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
