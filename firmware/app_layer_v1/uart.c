#include "uart.h"

#include <assert.h>
#include "Compiler.h"
#include "logging.h"
#include "board.h"
#include "byte_queue.h"
#include "protocol.h"

#define RX_BUF_SIZE 128
#define TX_BUF_SIZE 256

static BYTE rx_buffer[NUM_UARTS][RX_BUF_SIZE];
static BYTE tx_buffer[NUM_UARTS][TX_BUF_SIZE];
static ByteQueue rx_queues[NUM_UARTS];
static ByteQueue tx_queues[NUM_UARTS];

#define UART_REG(num) (((volatile UART *) &UART1) + num)

// The macro magic below generates for each type of flag from
// {RXIE, TXIE, RXIF, TXIF, RXIP, TXIP}
// an array of function pointers for setting this flag, where each element in
// the array corresponds to a single UART.
//
// For example, to set RXIE of UART2 to 1, call:
// SetRXIE[1](1);
#define UART_FLAG_FUNC(uart, flag) static void Set##flag##uart(int val) { _U##uart##flag = val; }

typedef void (*UARTFlagFunc)(int val);

#if NUM_UARTS != 4
  #error Currently only devices with 4 UARTs are supported. Please fix below.
#endif

#define ALL_UART_FLAG_FUNC(flag) \
  UART_FLAG_FUNC(1, flag)        \
  UART_FLAG_FUNC(2, flag)        \
  UART_FLAG_FUNC(3, flag)        \
  UART_FLAG_FUNC(4, flag)        \
  UARTFlagFunc Set##flag[NUM_UARTS] = { &Set##flag##1, &Set##flag##2, &Set##flag##3, &Set##flag##4 };

ALL_UART_FLAG_FUNC(RXIE)
ALL_UART_FLAG_FUNC(RXIF)
ALL_UART_FLAG_FUNC(RXIP)
ALL_UART_FLAG_FUNC(TXIE)
ALL_UART_FLAG_FUNC(TXIF)
ALL_UART_FLAG_FUNC(TXIP)

void UARTInit() {
  int i;
  for (i = 0; i < NUM_UARTS; ++i) {
    UARTConfig(i, 0, 0, 0);
  }
  // TODO: temp
  UARTConfig(0, 207, 1, 0);
}

void UARTConfig(int uart, int rate, int high_speed, int two_stop_bits) {
  volatile UART* regs = UART_REG(uart);
  log_printf("UARTConfig(%d, %d, %d, %d)", uart, rate, high_speed, two_stop_bits);
  SAVE_UART1_FOR_LOG();
  SetRXIE[uart](0);  // disable RX int.
  SetTXIE[uart](0);  // disable TX int.
  regs->uxmode = 0x0000;  // disable UART
  SetRXIF[uart](0);  // clear RX int.
  SetTXIF[uart](0);  // clear TX int.
  // clear SW buffers
  ByteQueueInit(rx_queues + uart, rx_buffer[uart], RX_BUF_SIZE);
  ByteQueueInit(tx_queues + uart, tx_buffer[uart], TX_BUF_SIZE);
  if (rate) {
    regs->uxbrg = rate;
    SetRXIP[uart](4);  // RX int. priority 4
    SetTXIP[uart](4);  // TX int. priority 4
    SetRXIE[uart](1);  // enable RX int.
    regs->uxmode = 0x8000 | (high_speed ? 0x0008 : 0x0000) | two_stop_bits | 0x0040;  // enable TODO: disable loopback
    regs->uxsta = 0x8400;  // IRQ when TX buffer is empty, enable TX, IRQ when character received.
  }
}

void UARTTasks() {
  int i;
  for (i = 0; i < NUM_UARTS; ++i) {
    int size;
    const BYTE* data;
    ByteQueue* q = rx_queues + i;
    BYTE lock;
    ByteQueuePeek(q, &data, &size);
    if (size > 64) size = 64;  // truncate
    if (size) {
      log_printf("UART %d received %d bytes", i, size);
      OUTGOING_MESSAGE msg;
      msg.type = UART_DATA;
      msg.args.uart_data.uart_num = i;
      msg.args.uart_data.size = size - 1;
      AppProtocolSendMessageWithVarArg(&msg, data, size);
      ByteQueueLock(q, lock, 4);
      ByteQueuePull(q, size);
      ByteQueueUnlock(q, lock);
    }
  }
}

static void TXInterrupt(int uart) {
  volatile UART* reg = UART_REG(uart);
  ByteQueue* q = tx_queues + uart;
  while (ByteQueueSize(q) && !(reg->uxsta & 0x0200)) {
    SetTXIF[uart](0);
    reg->uxtxreg = ByteQueuePullByte(q);
  }
  SetTXIE[uart](ByteQueueSize(q) != 0);
}

static void RXInterrupt(int uart) {
  volatile UART* reg = UART_REG(uart);
  ByteQueue* q = rx_queues + uart;
  // TODO: handle error
  while (reg->uxsta & 0x0001) {
    ByteQueuePushByte(q, reg->uxrxreg);
  }
}

void UARTTransmit(int uart, const void* data, int size) {
  log_printf("UARTTransmit(%d, %p, %d)", uart, data, size);
  SAVE_UART1_FOR_LOG();
  BYTE lock;
  ByteQueue* q = tx_queues + uart;
  ByteQueueLock(q, lock, 4);
  ByteQueuePushBuffer(q, data, size);
  SetTXIE[uart](1);  // enable TX int.
  ByteQueueUnlock(q, lock);
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

#if NUM_UARTS > 4
  #error Currently only devices with 4 or less UARTs are supported. Please fix below.
#endif

#if NUM_UARTS >= 1
  DEFINE_INTERRUPT_HANDLERS(1)
#endif

#if NUM_UARTS >= 2 && !ENABLE_LOGGING
  DEFINE_INTERRUPT_HANDLERS(2)
#endif

#if NUM_UARTS >= 3
  DEFINE_INTERRUPT_HANDLERS(3)
#endif

#if NUM_UARTS >= 4
  DEFINE_INTERRUPT_HANDLERS(4)
#endif

