#include "uart.h"

#include <assert.h>
#include "Compiler.h"
#include "logging.h"
#include "board.h"
#include "byte_queue.h"
#include "protocol.h"

// TODO: temp
#include "uart2.h"

#define RX_BUF_SIZE 128
#define TX_BUF_SIZE 256

static BYTE rx_buffer[NUM_UARTS][RX_BUF_SIZE];
static BYTE tx_buffer[NUM_UARTS][TX_BUF_SIZE];
static ByteQueue rx_queues[NUM_UARTS];
static ByteQueue tx_queues[NUM_UARTS];

#define UART_REG(num) (((volatile UART *) &UART1) + num)

// This is a "function template" for setting interrupt related flags for UART
// by number. It currently only works if there are 4 UARTs. Can be fixed with
// some pre-processor magic when the time comes.
#if NUM_UARTS != 4
  #error Currently only devices with 4 UARTs are supported. Please fix below.
#endif
#define SetUART(flag)                                    \
  static void SetUART##flag(int uart, int val) {  \
    switch (uart) {                                      \
      case 0:                                            \
        _U1##flag = val;                                 \
        break;                                           \
                                                         \
      case 1:                                            \
        _U2##flag = val;                                 \
        break;                                           \
                                                         \
      case 2:                                            \
        _U3##flag = val;                                 \
        break;                                           \
                                                         \
      case 3:                                            \
        _U4##flag = val;                                 \
        break;                                           \
                                                         \
      default:                                           \
        log_printf("Invalid UART number: %d", uart);     \
        assert(0);                                       \
    }                                                    \
  }

SetUART(RXIE)
SetUART(TXIE)
SetUART(RXIF)
SetUART(TXIF)
SetUART(RXIP)
SetUART(TXIP)

#undef SetUART

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
  SetUARTRXIE(uart, 0);  // disable RX int.
  SetUARTTXIE(uart, 0);  // disable TX int.
  regs->uxmode = 0x0000;  // disable UART
  SetUARTRXIF(uart, 0);  // clear RX int.
  SetUARTTXIF(uart, 0);  // clear TX int.
  // clear SW buffers
  ByteQueueInit(rx_queues + uart, rx_buffer[uart], RX_BUF_SIZE);
  ByteQueueInit(tx_queues + uart, tx_buffer[uart], TX_BUF_SIZE);
  if (rate) {
    regs->uxbrg = rate;
    SetUARTRXIP(uart, 4);  // RX int. priority 4
    SetUARTTXIP(uart, 4);  // TX int. priority 4
    SetUARTRXIE(uart, 1);  // enable RX int.
    regs->uxmode = 0x8000 | (high_speed ? 0x0008 : 0x0000) | two_stop_bits/* | 0x0040*/;  // enable TODO: disable loopback
    regs->uxsta = 0x8400;  // IRQ when TX buffer is empty, enable TX, IRQ when character received.

    // HACK
/*    _U1RXIE = 0;
    _U1TXIE = 0;
    U1BRG = 206;
    U1MODE = 0;
    U1MODEbits.BRGH = 1;
    U1STA = 0x8000;
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
    _U1RXIF = 0;*/

    log_printf("U1MODE=0x%x, U1STA=0x%x, U1TXIE=%d, U1TXIF=%d", U1MODE, U1STA, _U1TXIE, _U1TXIE);
  }
}

void UARTTasks() {
  int i;
  for (i = 0; i < NUM_UARTS; ++i) {
    int size;
    const BYTE* data;
    ByteQueuePeek(rx_queues + i, &data, &size);
    if (size > 64) size = 64;  // truncate
    if (size) {
      log_printf("UART %d received %d bytes", i, size);
      OUTGOING_MESSAGE msg;
      msg.type = UART_DATA;
      msg.args.uart_data.uart_num = i;
      msg.args.uart_data.size = size - 1;
      AppProtocolSendMessageWithVarArg(&msg, data, size);
    }
  }
}

static void TXInterrupt(int uart) {
  log_printf("TXInterrupt(%d)", uart);
  log_printf("U1MODE=0x%x, U1STA=0x%x, U1TXIE=%d, U1TXIF=%d", U1MODE, U1STA, _U1TXIE, _U1TXIE);
  BYTE lock;
  volatile UART* reg = UART_REG(uart);
  ByteQueue* q = tx_queues + uart;
  SetUARTTXIE(uart, 0);  // disable TX int.
  ByteQueueLock(q, lock);
  while (ByteQueueSize(q) && (UART2PutChar('@'), !(reg->uxsta & 0x0200))) {
    UART2PutChar('^');
    reg->uxtxreg = ByteQueuePullByte(q);
  }
  ByteQueueUnlock(q, lock);
  SetUARTTXIE(uart, ByteQueueSize(q) > 0);  // enable TX int.
}

static void RXInterrupt(int uart) {
  return;
  log_printf("RXInterrupt(%d)", uart);
  BYTE lock;
  volatile UART* reg = UART_REG(uart);
  ByteQueue* q = rx_queues + uart;
  ByteQueueLock(q, lock);
  // TODO: handle error
  while (reg->uxsta & 0x0001) {
    UART2PutChar('V');
    ByteQueuePushByte(q, reg->uxrxreg);
  }
  ByteQueueUnlock(q, lock);
}

void UARTTransmit(int uart, const void* data, int size) {
  log_printf("UARTTransmit(%d, %p, %d)", uart, data, size);
  SAVE_UART1_FOR_LOG();
  BYTE lock;
  ByteQueue* q = tx_queues + uart;
  ByteQueueLock(q, lock);
  ByteQueuePushBuffer(q, data, size);
  SetUARTTXIE(uart, 1);  // enable TX int.
  ByteQueueUnlock(q, lock);
}

#define DEFINE_INTERRUPT_HANDLERS(uart)                                   \
 void __attribute__((__interrupt__, auto_psv)) _U##uart##RXInterrupt() {  \
   RXInterrupt(uart - 1);                                                 \
   _U##uart##RXIF = 0;                                                    \
 }                                                                        \
                                                                          \
 void __attribute__((__interrupt__, auto_psv)) _U##uart##TXInterrupt() {  \
   log_printf("%d", _U1TXIF);                                             \
   TXInterrupt(uart - 1);                                                 \
   _U##uart##TXIF = 0;                                                    \
   log_printf("%d", _U1TXIF);                                             \
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

