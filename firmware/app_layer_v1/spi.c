#include "spi.h"

#include "packet_queue.h"
#include "board.h"
#include "logging.h"
#include "pins.h"
#include "protocol.h"
#include "sync.h"

#define RX_BUF_SIZE 128
#define TX_BUF_SIZE 256

typedef enum {
  PACKET_STATE_IDLE,
  PACKET_STATE_IN_PROGRESS,
  PACKET_STATE_DONE
} PACKET_STATE;

static BYTE rx_buffer[NUM_SPI_MODULES][RX_BUF_SIZE];
static BYTE tx_buffer[NUM_SPI_MODULES][TX_BUF_SIZE];
static PACKET_QUEUE rx_queues[NUM_SPI_MODULES];
static PACKET_QUEUE tx_queues[NUM_SPI_MODULES];
static PACKET_STATE packet_state[NUM_SPI_MODULES];

static int num_tx_since_last_report[NUM_SPI_MODULES];

volatile SPI* spi_reg[NUM_SPI_MODULES] = {
  (volatile SPI*) 0x240,
  (volatile SPI*) 0x260,
  (volatile SPI*) 0x280
};

// The macro magic below generates for each type of flag from
// {IE, IF, IP}
// an array of function pointers for setting this flag, where each element in
// the array corresponds to a single SPI module.
//
// For example, to set IE of SPI2 to 1, call:
// SetIE[1](1);
#define SPI_FLAG_FUNC(spi, flag) static void Set##flag##spi(int val) { _SPI##spi##flag = val; }

typedef void (*SPIFlagFunc)(int val);

#if NUM_SPI_MODULES != 3
  #error Currently only devices with 3 SPI modules are supported. Please fix below.
#endif

#define ALL_SPI_FLAG_FUNC(flag)                              \
  SPI_FLAG_FUNC(1, flag)                                     \
  SPI_FLAG_FUNC(2, flag)                                     \
  SPI_FLAG_FUNC(3, flag)                                     \
  SPIFlagFunc Set##flag[NUM_SPI_MODULES] = { &Set##flag##1,  \
                                             &Set##flag##2,  \
                                             &Set##flag##3 };

ALL_SPI_FLAG_FUNC(IE)
ALL_SPI_FLAG_FUNC(IF)
ALL_SPI_FLAG_FUNC(IP)

void SPIInit() {
  int i;
  for (i = 0; i < NUM_SPI_MODULES; ++i) {
    SPIConfigMaster(i, 0, 0, 0, 0, 0);
    SetIP[i](4);  // int. priority 4
  }
}

void SPIConfigMaster(int spi, int scale, int div, int smp_end, int clk_edge,
               int clk_pol) {
  volatile SPI* regs = spi_reg[spi];
  log_printf("SPIConfigMaster(%d, %d, %d, %d, %d, %d)", spi, scale, div,
             smp_end, clk_edge, clk_pol);
  SetIE[spi](0);  // disable int.
  regs->spixstat = 0x0000;  // disable SPI
  // clear SW buffers
  PacketQueueInit(rx_queues + spi, rx_buffer[spi], RX_BUF_SIZE);
  PacketQueueInit(tx_queues + spi, tx_buffer[spi], TX_BUF_SIZE);
  // TODO: configure
  num_tx_since_last_report[spi] = 0;
  if (scale && div) {
    regs->spixcon1 = (smp_end << 9)
                     | (clk_edge << 8)
                     | (clk_pol << 6)
                     | (1 << 5)  // master
                     | ((7 - div) << 2)
                     | ((3 - scale));
    regs->spixcon2 = 0x0001;  // enhanced buffer mode
    regs->spixstat = (1 << 15)  // enable
                     | (6 << 2);  // int. when TX FIFO is empty
  }
}

void SPITasks() {
  int i;
  for (i = 0; i < NUM_SPI_MODULES; ++i) {
    int size1, size2, size;
    const BYTE *data1, *data2;
    PACKET_QUEUE* q = rx_queues + i;
    BYTE prev;
    while (PacketQueueHasData(q)) {
      OUTGOING_MESSAGE msg;
      PacketQueueStartRead(q);
      PacketQueuePeek(q, &data1, &size1, &data2, &size2);
      size = size1 + size2;
      assert(size == PacketQueueReadRemaining(q));
      log_printf("SPI %d received %d bytes", i, size);
      msg.type = SPI_DATA;
      msg.args.spi_data.spi_num = i;
      msg.args.spi_data.ss_pin = PacketQueueCurrentDest(q);
      msg.args.spi_data.size = size1 + size2 - 1;
      AppProtocolSendMessageWithVarArgSplit(&msg, data1, size1, data2, size2);
      prev = SyncInterruptLevel(4);
      PacketQueuePull(q, size);
      SyncInterruptLevel(prev);
    }
    if (num_tx_since_last_report[i] > TX_BUF_SIZE / 2) {
      SPIReportTxStatus(i);
    }
  }
}

void SPIReportTxStatus(int spi) {
  int remaining;
  PACKET_QUEUE* q = tx_queues + spi;
  BYTE prev = SyncInterruptLevel(4);
  remaining = PacketQueueRemaining(q);
  num_tx_since_last_report[spi] = 0;
  SyncInterruptLevel(prev);
  OUTGOING_MESSAGE msg;
  msg.type = SPI_REPORT_TX_STATUS;
  msg.args.spi_report_tx_status.spi_num = spi;
  msg.args.spi_report_tx_status.bytes_remaining = remaining;
  AppProtocolSendMessage(&msg);
}

static void SPIInterrupt(int spi) {
  volatile SPI* reg = spi_reg[spi];
  PACKET_QUEUE* tx_queue = tx_queues + spi;
  PACKET_QUEUE* rx_queue = rx_queues + spi;
  int bytes_to_write;
  int max_bytes_to_write = 7;

  // can't have incoming data on idle state. if we do - it's a bug
  assert(packet_state[spi] != PACKET_STATE_IDLE
         || reg->spixstat & (1 << 5));
  
  // read incoming data into rx_queue
  while (!(reg->spixstat & (1 << 5))) {
    PacketQueuePushByte(rx_queue, reg->spixbuf);
  }
  
  switch (packet_state[spi]) {
    case PACKET_STATE_IDLE:
      assert(PacketQueueHasData(tx_queue));
      PacketQueueStartRead(tx_queue);
      // write packet header to rx_queue
      PacketQueuePushByte(rx_queue, PacketQueueCurrentDest(tx_queue));
      PacketQueuePushByte(rx_queue, PacketQueueReadRemaining(tx_queue));
      
      PinSetLat(PacketQueueCurrentDest(tx_queue), 1);  // assert SS
      ++max_bytes_to_write;  // we can write 8 bytes the first time, since the
                             // shift register is empty.
      packet_state[spi] = PACKET_STATE_IN_PROGRESS;
      // fall-through on purpose
      
    case PACKET_STATE_IN_PROGRESS:
      bytes_to_write = PacketQueueReadRemaining(tx_queue);
      if (bytes_to_write > max_bytes_to_write)  {
        bytes_to_write = max_bytes_to_write;
      } else {
        // this is the end of the packet, we want next interrupt to occur after
        // last byte finished sending.
        reg->spixstat = (1 << 15)  // enable
                        | (5 << 2);  // int. when last byte shifted
        packet_state[spi] = PACKET_STATE_DONE;
      }
      SetIF[spi](0);
      while (bytes_to_write-- > 0) {
        reg->spixbuf = PacketQueueRead(tx_queue);
        ++num_tx_since_last_report[spi];
      }
      break;
      
    case PACKET_STATE_DONE:
      PinSetLat(PacketQueueCurrentDest(tx_queue), 0);  // clear SS
      reg->spixstat = (1 << 15)  // enable
                      | (5 << 2);  // int. when TX FIFO empty
      SetIE[spi](PacketQueueHasData(tx_queue));
      packet_state[spi] = PACKET_STATE_IDLE;
      break;
  }
}

void SPITransmit(int spi, int dest, const void* data, int size) {
  log_printf("SPITransmit(%d, %d, %p, %d)", spi, dest, data, size);
  PACKET_QUEUE* q = tx_queues + spi;
  BYTE prev = SyncInterruptLevel(4);
  PacketQueuePush(q, dest, size, data);
  SetIE[spi](1);  // enable int.
  SyncInterruptLevel(prev);
}

#define DEFINE_INTERRUPT_HANDLERS(spi)                                    \
 void __attribute__((__interrupt__, auto_psv)) _SPI##spi##Interrupt() {   \
   SPIInterrupt(spi - 1);                                                 \
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
