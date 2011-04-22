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

#include "spi.h"

#include <assert.h>
#include "byte_queue.h"
#include "board.h"
#include "logging.h"
#include "pins.h"
#include "pp_util.h"
#include "protocol.h"
#include "sync.h"

#define RX_BUF_SIZE 128
#define TX_BUF_SIZE 256

typedef enum {
  PACKET_STATE_IDLE,
  PACKET_STATE_IN_PROGRESS,
  PACKET_STATE_DONE
} PACKET_STATE;

typedef struct {
  PACKET_STATE packet_state;
  int num_tx_since_last_report;
  BYTE cur_msg_dest;
  BYTE cur_msg_total_size;
  BYTE cur_msg_data_size;
  BYTE cur_msg_trim_rx;

  // message format:
  // BYTE dest
  // BYTE tx_size
  // BYTE tx_data[tx_size]
  BYTE_QUEUE rx_queue;

  int num_messages_rx_queue;

  // message format:
  // BYTE dest
  // BYTE total_size
  // BYTE data_size
  // BYTE rx_trim
  // BYTE tx_data[tx_size]
  BYTE_QUEUE tx_queue;

  BYTE rx_buffer[RX_BUF_SIZE];
  BYTE tx_buffer[TX_BUF_SIZE];
} SPI_STATE;

static SPI_STATE spis[NUM_SPI_MODULES];

typedef struct {
  unsigned int spixstat;
  unsigned int spixcon1;
  unsigned int spixcon2;
  unsigned int reserved;
  unsigned int spixbuf;
} SPIREG;

#define _SPIREG_REF_COMMA(num, dummy) (volatile SPIREG*) &SPI##num##STAT,

volatile SPIREG* spi_reg[NUM_SPI_MODULES] = {
  REPEAT_1B(_SPIREG_REF_COMMA, NUM_SPI_MODULES, 0)
};

DEFINE_REG_SETTERS_1B(NUM_SPI_MODULES, _SPI, IF)
DEFINE_REG_SETTERS_1B(NUM_SPI_MODULES, _SPI, IE)
DEFINE_REG_SETTERS_1B(NUM_SPI_MODULES, _SPI, IP)

void SPIInit() {
  int i;
  for (i = 0; i < NUM_SPI_MODULES; ++i) {
    SPIConfigMaster(i, 0, 0, 0, 0, 0);
    Set_SPIIP[i](4);  // int. priority 4
  }
}

void SPIConfigMaster(int spi_num, int scale, int div, int smp_end, int clk_edge,
               int clk_pol) {
  volatile SPIREG* regs = spi_reg[spi_num];
  SPI_STATE* spi = &spis[spi_num];
  log_printf("SPIConfigMaster(%d, %d, %d, %d, %d, %d)", spi_num, scale, div,
             smp_end, clk_edge, clk_pol);
  Set_SPIIE[spi_num](0);  // disable int.
  regs->spixstat = 0x0000;  // disable SPI
  // clear SW buffers
  ByteQueueInit(&spi->rx_queue, spi->rx_buffer, RX_BUF_SIZE);
  ByteQueueInit(&spi->tx_queue, spi->tx_buffer, TX_BUF_SIZE);
  spi->num_tx_since_last_report = 0;
  spi->num_messages_rx_queue = 0;
  spi->packet_state = PACKET_STATE_IDLE;
  if (scale && div) {
    spi->num_tx_since_last_report = TX_BUF_SIZE;
    regs->spixcon1 = (smp_end << 9)
                     | (clk_edge << 8)
                     | (clk_pol << 6)
                     | (1 << 5)  // master
                     | ((7 - div) << 2)
                     | ((3 - scale));
    regs->spixcon2 = 0x0001;  // enhanced buffer mode
    regs->spixstat = (1 << 15)  // enable
                     | (5 << 2);  // int. when TX FIFO is empty
    Set_SPIIF[spi_num](1);  // set int. flag, so int. will occur as soon as data is
                        // written
  }
}

static void SPIReportTxStatus(int spi_num) {
  int report;
  SPI_STATE* spi = &spis[spi_num];
  BYTE prev = SyncInterruptLevel(4);
  report = spi->num_tx_since_last_report;
  spi->num_tx_since_last_report = 0;
  SyncInterruptLevel(prev);
  OUTGOING_MESSAGE msg;
  msg.type = SPI_REPORT_TX_STATUS;
  msg.args.spi_report_tx_status.spi_num = spi_num;
  msg.args.spi_report_tx_status.bytes_to_add = report;
  AppProtocolSendMessage(&msg);
}

void SPITasks() {
  int i;
  for (i = 0; i < NUM_SPI_MODULES; ++i) {
    int size1, size2, size;
    const BYTE *data1, *data2;
    SPI_STATE* spi = &spis[i];
    BYTE_QUEUE* q = &spi->rx_queue;
    BYTE prev;
    while (spi->num_messages_rx_queue) {
      OUTGOING_MESSAGE msg;
      msg.type = SPI_DATA;
      msg.args.spi_data.spi_num = i;
      prev = SyncInterruptLevel(4);
      msg.args.spi_data.ss_pin = ByteQueuePullByte(q);
      msg.args.spi_data.size = ByteQueuePullByte(q) - 1;
      SyncInterruptLevel(prev);
      ByteQueuePeekMax(q, msg.args.spi_data.size + 1, &data1, &size1, &data2,
                       &size2);
      size = size1 + size2;
      assert(size == msg.args.spi_data.size + 1);
      log_printf("SPI %d received %d bytes", i, size);
      AppProtocolSendMessageWithVarArgSplit(&msg, data1, size1, data2, size2);
      prev = SyncInterruptLevel(4);
      ByteQueuePull(q, size);
      --spi->num_messages_rx_queue;
      SyncInterruptLevel(prev);
    }
    if (spi->num_tx_since_last_report > TX_BUF_SIZE / 2) {
      SPIReportTxStatus(i);
    }
  }
}

static void SPIInterrupt(int spi_num) {
  volatile SPIREG* reg = spi_reg[spi_num];
  SPI_STATE* spi = &spis[spi_num];
  BYTE_QUEUE* tx_queue = &spi->tx_queue;
  BYTE_QUEUE* rx_queue = &spi->rx_queue;
  int bytes_to_write;
  int max_bytes_to_write = 7;
  int total_rx_bytes;

  // can't have incoming data on idle state. if we do - it's a bug
  assert(spi->packet_state != PACKET_STATE_IDLE
         || reg->spixstat & (1 << 5));
  
  // read incoming data into rx_queue
  while (!(reg->spixstat & (1 << 5))) {
    BYTE rx_byte = reg->spixbuf;
    if (spi->cur_msg_trim_rx) {
      --spi->cur_msg_trim_rx;
    } else {
      ByteQueuePushByte(rx_queue, rx_byte);
    }
  }
  
  switch (spi->packet_state) {
    case PACKET_STATE_IDLE:
      assert(ByteQueueSize(tx_queue));
      spi->cur_msg_dest = ByteQueuePullByte(tx_queue);
      spi->cur_msg_total_size = ByteQueuePullByte(tx_queue);
      spi->cur_msg_data_size = ByteQueuePullByte(tx_queue);
      spi->cur_msg_trim_rx = ByteQueuePullByte(tx_queue);
      spi->num_tx_since_last_report += 4;

      // write packet header to rx_queue, if non-empty
      total_rx_bytes = spi->cur_msg_total_size - spi->cur_msg_trim_rx;
      if (total_rx_bytes > 0) {
        ByteQueuePushByte(rx_queue, spi->cur_msg_dest);
        ByteQueuePushByte(rx_queue, total_rx_bytes);
      }
      
      PinSetLat(spi->cur_msg_dest, 0);  // activate SS
      ++max_bytes_to_write;  // we can write 8 bytes the first time, since the
                             // shift register is empty.
      spi->packet_state = PACKET_STATE_IN_PROGRESS;
      // fall-through on purpose
      
    case PACKET_STATE_IN_PROGRESS:
      bytes_to_write = spi->cur_msg_total_size;
      if (bytes_to_write > max_bytes_to_write)  {
        bytes_to_write = max_bytes_to_write;
      } else {
        // this is the end of the packet, we want next interrupt to occur after
        // last byte finished sending.
        reg->spixstat = (1 << 15)  // enable
                        | (5 << 2);  // int. when last byte shifted
          spi->packet_state = PACKET_STATE_DONE;
      }
      Set_SPIIF[spi_num](0);
      while (bytes_to_write-- > 0) {
        BYTE tx_byte = 0xFF;
        if (spi->cur_msg_data_size) {
          tx_byte = ByteQueuePullByte(tx_queue);
          --spi->cur_msg_data_size;
          ++spi->num_tx_since_last_report;
        }
        reg->spixbuf = tx_byte;
        --spi->cur_msg_total_size;
      }
      break;
      
    case PACKET_STATE_DONE:
      PinSetLat(spi->cur_msg_dest, 1);  // deactivate SS
      ++spi->num_messages_rx_queue;
      reg->spixstat = (1 << 15)  // enable
                      | (6 << 2);  // int. when TX FIFO empty
      Set_SPIIE[spi_num](ByteQueueSize(tx_queue));
      spi->packet_state = PACKET_STATE_IDLE;
      break;
  }
}

void SPITransmit(int spi_num, int dest, const void* data, int data_size,
                 int total_size, int trim_rx) {
  log_printf("SPITransmit(%d, %d, %p, %d, %d, %d)", spi_num, dest, data,
             data_size, total_size, trim_rx);
  BYTE_QUEUE* q = &spis[spi_num].tx_queue;
  BYTE prev = SyncInterruptLevel(4);
  ByteQueuePushByte(q, dest);
  ByteQueuePushByte(q, total_size);
  ByteQueuePushByte(q, data_size);
  ByteQueuePushByte(q, trim_rx);
  ByteQueuePushBuffer(q, data, data_size);
  Set_SPIIE[spi_num](1);  // enable int.
  SyncInterruptLevel(prev);
}

#define DEFINE_INTERRUPT_HANDLERS(spi_num)                                   \
 void __attribute__((__interrupt__, auto_psv)) _SPI##spi_num##Interrupt() {  \
   SPIInterrupt(spi_num - 1);                                                \
 }

#if NUM_SPI_MODULES > 3
  #error Currently only devices with 3 or less SPIs are supported. Please fix below.
#endif

#if NUM_SPI_MODULES >= 1
  DEFINE_INTERRUPT_HANDLERS(1)
#endif

#if NUM_SPI_MODULES >= 2
  DEFINE_INTERRUPT_HANDLERS(2)
#endif

#if NUM_SPI_MODULES >= 3
  DEFINE_INTERRUPT_HANDLERS(3)
#endif
