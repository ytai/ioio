#include "uart.h"

#include <assert.h>
#include "Compiler.h"
#include "logging.h"
#include "board.h"
#include "byte_queue.h"
#include "protocol.h"
#include "sync.h"

#define RX_BUF_SIZE 128
#define TX_BUF_SIZE 256

static BYTE rx_buffer[NUM_UART_MODULES][RX_BUF_SIZE];
static BYTE tx_buffer[NUM_UART_MODULES][TX_BUF_SIZE];
static BYTE_QUEUE rx_queues[NUM_UART_MODULES];
static BYTE_QUEUE tx_queues[NUM_UART_MODULES];

static int num_tx_since_last_report[NUM_UART_MODULES];

volatile UART* uart_reg[NUM_UART_MODULES] = {
  (volatile UART*) 0x220,
  (volatile UART*) 0x230,
  (volatile UART*) 0x250,
  (volatile UART*) 0x2B0
};

// The macro magic below generates for each type of flag from
// {RXIE, TXIE, RXIF, TXIF, RXIP, TXIP}
// an array of function pointers for setting this flag, where each element in
// the array corresponds to a single UART.
//
// For example, to set RXIE of UART2 to 1, call:
// SetRXIE[1](1);
#define UART_FLAG_FUNC(uart, flag) static void Set##flag##uart(int val) { _U##uart##flag = val; }

typedef void (*UARTFlagFunc)(int val);

#if NUM_UART_MODULES != 4
  #error Currently only devices with 4 UARTs are supported. Please fix below.
#endif

#define ALL_UART_FLAG_FUNC(flag)                                \
  UART_FLAG_FUNC(1, flag)                                       \
  UART_FLAG_FUNC(2, flag)                                       \
  UART_FLAG_FUNC(3, flag)                                       \
  UART_FLAG_FUNC(4, flag)                                       \
  UARTFlagFunc Set##flag[NUM_UART_MODULES] = { &Set##flag##1,   \
                                               &Set##flag##2,   \
                                               &Set##flag##3,   \
                                               &Set##flag##4 };

ALL_UART_FLAG_FUNC(RXIE)
ALL_UART_FLAG_FUNC(RXIF)
ALL_UART_FLAG_FUNC(RXIP)
ALL_UART_FLAG_FUNC(TXIE)
ALL_UART_FLAG_FUNC(TXIF)
ALL_UART_FLAG_FUNC(TXIP)

void UARTInit() {
  int i;
  for (i = 0; i < NUM_UART_MODULES; ++i) {
    UARTConfig(i, 0, 0, 0, 0);
    SetRXIP[i](4);  // RX int. priority 4
    SetTXIP[i](4);  // TX int. priority 4
  }
}

void UARTConfig(int uart, int rate, int speed4x, int two_stop_bits, int parity) {
  volatile UART* regs = uart_reg[uart];
  log_printf("UARTConfig(%d, %d, %d, %d, %d)", uart, rate, speed4x, two_stop_bits, parity);
  SAVE_UART1_FOR_LOG();
  SetRXIE[uart](0);  // disable RX int.
  SetTXIE[uart](0);  // disable TX int.
  regs->uxmode = 0x0000;  // disable UART
  // clear SW buffers
  ByteQueueInit(rx_queues + uart, rx_buffer[uart], RX_BUF_SIZE);
  ByteQueueInit(tx_queues + uart, tx_buffer[uart], TX_BUF_SIZE);
  num_tx_since_last_report[uart] = 0;
  if (rate) {
    regs->uxbrg = rate;
    SetRXIF[uart](0);  // clear RX int.
    SetTXIF[uart](0);  // clear TX int.
    SetRXIE[uart](1);  // enable RX int.
    regs->uxmode = 0x8000 | (speed4x ? 0x0008 : 0x0000) | two_stop_bits | (parity << 1);  // enable
    regs->uxsta = 0x8400;  // IRQ when TX buffer is empty, enable TX, IRQ when character received.
  }
}

void UARTTasks() {
  int i;
  for (i = 0; i < NUM_UART_MODULES; ++i) {
    int size;
    const BYTE* data;
    BYTE_QUEUE* q = rx_queues + i;
    BYTE prev;
    ByteQueuePeek(q, &data, &size);
    if (size > 64) size = 64;  // truncate
    if (size) {
      log_printf("UART %d received %d bytes", i, size);
      OUTGOING_MESSAGE msg;
      msg.type = UART_DATA;
      msg.args.uart_data.uart_num = i;
      msg.args.uart_data.size = size - 1;
      AppProtocolSendMessageWithVarArg(&msg, data, size);
      prev = SyncInterruptLevel(4);
      ByteQueuePull(q, size);
      SyncInterruptLevel(prev);
    }
    if (num_tx_since_last_report[i] > TX_BUF_SIZE / 2) {
      UARTReportTxStatus(i);
    }
  }
}

void UARTReportTxStatus(int uart) {
  int remaining;
  BYTE_QUEUE* q = tx_queues + uart;
  BYTE prev = SyncInterruptLevel(4);
  remaining = ByteQueueRemaining(q);
  num_tx_since_last_report[uart] = 0;
  SyncInterruptLevel(prev);
  OUTGOING_MESSAGE msg;
  msg.type = UART_REPORT_TX_STATUS;
  msg.args.uart_report_tx_status.uart_num = uart;
  msg.args.uart_report_tx_status.bytes_remaining = remaining;
  AppProtocolSendMessage(&msg);
}

static void TXInterrupt(int uart) {
  volatile UART* reg = uart_reg[uart];
  BYTE_QUEUE* q = tx_queues + uart;
  while (ByteQueueSize(q) && !(reg->uxsta & 0x0200)) {
    SetTXIF[uart](0);
    reg->uxtxreg = ByteQueuePullByte(q);
    ++num_tx_since_last_report[uart];
  }
  SetTXIE[uart](ByteQueueSize(q) != 0);
}

static void RXInterrupt(int uart) {
  volatile UART* reg = uart_reg[uart];
  BYTE_QUEUE* q = rx_queues + uart;
  // TODO: handle error
  while (reg->uxsta & 0x0001) {
    ByteQueuePushByte(q, reg->uxrxreg);
  }
}

void UARTTransmit(int uart, const void* data, int size) {
  log_printf("UARTTransmit(%d, %p, %d)", uart, data, size);
  SAVE_UART1_FOR_LOG();
  BYTE_QUEUE* q = tx_queues + uart;
  BYTE prev = SyncInterruptLevel(4);
  ByteQueuePushBuffer(q, data, size);
  SetTXIE[uart](1);  // enable TX int.
  SyncInterruptLevel(prev);
}

#define DEFINE_INTERRUPT_HANDLERS(uart)                                   \
 void __attribute__((__interrupt__, auto_psv)) _U##uart##RXInterrupt() {  \
   RXInterrupt(uart - 1);                                                 \
   _U##uart##RXIF = 0;                                                    \
 }                                                                        \
                                                                          \
 void __attribute__((__interrupt__, auto_psv)) _U##uart##TXInterrupt() {  \
   TXInterrupt(uart - 1);                                                 \
 }

#if NUM_UART_MODULES > 4
  #error Currently only devices with 4 or less UARTs are supported. Please fix below.
#endif

#if NUM_UART_MODULES >= 1
  DEFINE_INTERRUPT_HANDLERS(1)
#endif

#if NUM_UART_MODULES >= 2 && !ENABLE_LOGGING
  DEFINE_INTERRUPT_HANDLERS(2)
#endif

#if NUM_UART_MODULES >= 3
  DEFINE_INTERRUPT_HANDLERS(3)
#endif

#if NUM_UART_MODULES >= 4
  DEFINE_INTERRUPT_HANDLERS(4)
#endif

