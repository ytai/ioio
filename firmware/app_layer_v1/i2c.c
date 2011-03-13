#include "i2c.h"

#include <assert.h>
#include "Compiler.h"
#include "board.h"
#include "sync.h"
#include "byte_queue.h"
#include "logging.h"
#include "protocol.h"

#define PACKED __attribute__ ((packed))

#define RX_BUF_SIZE 128
#define TX_BUF_SIZE 256

typedef enum {
  STATE_START,
  STATE_ADDR1_WRITE,
  STATE_ADDR2_WRITE,
  STATE_WRITE_DATA,
  STATE_STOP_WRITE_ONLY,
  STATE_RESTART,
  STATE_ADDR_READ,
  STATE_ACK_ADDR_READ,
  STATE_READ_DATA,
  STATE_ACK_READ_DATA
} MESSAGE_STATE;

typedef struct PACKED {
  union {
    struct {
      BYTE addr1;
      BYTE addr2;
    };
    WORD addr;
  };
  BYTE write_size;
  BYTE read_size;
} TX_MESSAGE_HEADER;

typedef struct {
  MESSAGE_STATE message_state;
  TX_MESSAGE_HEADER cur_tx_header;
  int num_tx_since_last_report;
  int bytes_remaining;

  BYTE_QUEUE rx_queue;
  int num_messages_rx_queue;
  BYTE_QUEUE tx_queue;

  BYTE rx_buffer[RX_BUF_SIZE];
  BYTE tx_buffer[TX_BUF_SIZE];
} I2C_STATE;

I2C_STATE i2c_states[NUM_I2C_MODULES];

typedef struct {
  unsigned int rcv;
  unsigned int trn;
  unsigned int brg;
  unsigned int con;
  unsigned int stat;
  unsigned int add;
  unsigned int mask;
} I2CREG;

#if NUM_I2C_MODULES != 3
  #error The code below assumes 3 I2C modules. Please fix.
#endif
volatile I2CREG* i2c_reg[NUM_I2C_MODULES] = {
  (volatile I2CREG*) &I2C1RCV,
  (volatile I2CREG*) &I2C2RCV,
  (volatile I2CREG*) &I2C3RCV
};

// The macro magic below generates for each type of flag from
// {IE, IF, IP}
// an array of function pointers for setting this flag, where each element in
// the array corresponds to a single I2C module, in master mode.
//
// For example, to set IE of I2C2 as master to 1, call:
// SetMIE[1](1);
#define MI2C_FLAG_FUNC(i2c, flag) static void SetM##flag##i2c(int val) { _MI2C##i2c##flag = val; }

typedef void (*I2CFlagFunc)(int val);

#if NUM_I2C_MODULES != 3
  #error Currently only devices with 3 I2C modules are supported. Please fix below.
#endif

#define ALL_I2C_FLAG_FUNC(flag)                                 \
  MI2C_FLAG_FUNC(1, flag)                                       \
  MI2C_FLAG_FUNC(2, flag)                                       \
  MI2C_FLAG_FUNC(3, flag)                                       \
  I2CFlagFunc SetM##flag[NUM_I2C_MODULES] = { &SetM##flag##1,   \
                                              &SetM##flag##2,   \
                                              &SetM##flag##3 };

ALL_I2C_FLAG_FUNC(IE)
ALL_I2C_FLAG_FUNC(IF)
ALL_I2C_FLAG_FUNC(IP)


void I2CInit() {
  log_printf("I2CInit()");
  int i;
  for (i = 0; i < NUM_I2C_MODULES; ++i) {
    SetMIP[i](4);  // interrupt priority 4
    I2CConfigMaster(i, 0, 0);
  }
}

void I2CConfigMaster(int i2c_num, int rate, int smbus_levels) {
  volatile I2CREG* regs = i2c_reg[i2c_num];
  I2C_STATE* i2c = i2c_states + i2c_num;
  static const unsigned int brg_values[] = { 0x9D, 0x25, 0x0D };
  
  log_printf("I2CConfigMaster(%d, %d, %d)", i2c_num, rate, smbus_levels);
  SetMIE[i2c_num](0);  // disable interrupt
  regs->con = 0x0000;  // disable module
  SetMIF[i2c_num](0);  // clear interrupt
  ByteQueueInit(&i2c->tx_queue, i2c->tx_buffer, TX_BUF_SIZE);
  ByteQueueInit(&i2c->rx_queue, i2c->rx_buffer, RX_BUF_SIZE);
  i2c->num_messages_rx_queue = 0;
  i2c->message_state = STATE_START;
  if (rate) {
    regs->brg = brg_values[rate - 1];
    regs->con = (1 << 15)               // enable
                | ((rate != 2) << 9)    // disable slew rate unless 400KHz mode
                | (smbus_levels << 8);  // use SMBus levels
    SetMIF[i2c_num](1);  // signal interrupt
  }
}

void I2CTasks() {
  int i;
  for (i = 0; i < NUM_I2C_MODULES; ++i) {
    int size1, size2, size;
    const BYTE *data1, *data2;
    I2C_STATE* i2c = &i2c_states[i];
    BYTE_QUEUE* q = &i2c->rx_queue;
    BYTE prev;
    while (i2c->num_messages_rx_queue) {
//      log_printf("msg");
      OUTGOING_MESSAGE msg;
      msg.type = I2C_RESULT;
      msg.args.i2c_result.i2c_num = i;
      prev = SyncInterruptLevel(4);
      msg.args.i2c_result.size = ByteQueuePullByte(q);
      --i2c->num_messages_rx_queue;
      SyncInterruptLevel(prev);
      log_printf("I2C %d received %d bytes", i, msg.args.i2c_result.size);
      if (msg.args.i2c_result.size != 0xFF && msg.args.i2c_result.size > 0) {
        ByteQueuePeekMax(q, msg.args.i2c_result.size, &data1, &size1, &data2,
                         &size2);
        size = size1 + size2;
        assert(size == msg.args.i2c_result.size);
        AppProtocolSendMessageWithVarArgSplit(&msg, data1, size1, data2, size2);
        prev = SyncInterruptLevel(4);
        ByteQueuePull(q, size);
        SyncInterruptLevel(prev);
      } else {
        AppProtocolSendMessage(&msg);
      }
    }
    if (i2c->num_tx_since_last_report > TX_BUF_SIZE / 2) {
      I2CReportTxStatus(i);
    }
  }
}

void I2CWriteRead(int i2c_num, unsigned int addr, const void* data,
                  int write_bytes, int read_bytes) {
  I2C_STATE* i2c = i2c_states + i2c_num;
  TX_MESSAGE_HEADER hdr;
  BYTE prev;
  log_printf("I2CWriteRead(%d, 0x%x, %p, %d, %d)", i2c_num, addr,
             data, write_bytes, read_bytes);
  hdr.addr = addr;
  hdr.write_size = write_bytes;
  hdr.read_size = read_bytes;
  prev = SyncInterruptLevel(4);
  ByteQueuePushBuffer(&i2c->tx_queue, &hdr, sizeof hdr);
  ByteQueuePushBuffer(&i2c->tx_queue, data, write_bytes);
  SetMIE[i2c_num](1);
  SyncInterruptLevel(prev);
}

void I2CReportTxStatus(int i2c_num) {
  int remaining;
  I2C_STATE* i2c = &i2c_states[i2c_num];
  BYTE_QUEUE* q = &i2c->tx_queue;
  BYTE prev = SyncInterruptLevel(4);
  remaining = ByteQueueRemaining(q);
  i2c->num_tx_since_last_report = 0;
  SyncInterruptLevel(prev);
  OUTGOING_MESSAGE msg;
  msg.type = I2C_REPORT_TX_STATUS;
  msg.args.i2c_report_tx_status.i2c_num = i2c_num;
  msg.args.i2c_report_tx_status.bytes_remaining = remaining;
  AppProtocolSendMessage(&msg);
}

static void MI2CInterrupt(int i2c_num) {
  I2C_STATE* i2c = i2c_states + i2c_num;
  volatile I2CREG* reg = i2c_reg[i2c_num];

  log_printf("int state = %d", i2c->message_state);
  SetMIF[i2c_num](0);  // clear interrupt
  switch (i2c->message_state) {
    case STATE_START:
      ByteQueuePullToBuffer(&i2c->tx_queue, &i2c->cur_tx_header,
                            sizeof(TX_MESSAGE_HEADER));
      i2c->num_tx_since_last_report += sizeof(TX_MESSAGE_HEADER);
      i2c->bytes_remaining = i2c->cur_tx_header.write_size;
      reg->con |= 0x0001;  // send start bit
      if (i2c->bytes_remaining) {
        i2c->message_state = STATE_ADDR1_WRITE;
      } else {
        i2c->message_state = STATE_ADDR_READ;
      }
      break;
      
    case STATE_ADDR1_WRITE:
//      log_printf("writing: 0x%x", i2c->cur_tx_header.addr1);
//      log_printf("stat=0x%x", reg->stat);
      reg->trn = i2c->cur_tx_header.addr1;
//      log_printf("stat=0x%x", reg->stat);
      if (i2c->cur_tx_header.addr1 >> 3 == 0b00011110) {
        i2c->message_state = STATE_ADDR2_WRITE;
      } else {
        i2c->message_state = STATE_WRITE_DATA;
      }
      break;
      
    case STATE_ADDR2_WRITE:
      if (reg->stat >> 15) goto error;
//      log_printf("writing: 0x%x", i2c->cur_tx_header.addr2);
      reg->trn = i2c->cur_tx_header.addr2;
      i2c->message_state = STATE_WRITE_DATA;
      break;
      
    case STATE_WRITE_DATA:
      if (reg->stat >> 15) goto error;
      {
        BYTE b = ByteQueuePullByte(&i2c->tx_queue);
//        log_printf("writing: 0x%x", b);
        reg->trn = b;
      }
      ++i2c->num_tx_since_last_report;
      if (--i2c->bytes_remaining == 0) {
        i2c->message_state = i2c->cur_tx_header.read_size
                             ? STATE_RESTART
                             : STATE_STOP_WRITE_ONLY;
      }
      break;

    case STATE_STOP_WRITE_ONLY:
      if (reg->stat >> 15) goto error;
      ByteQueuePushByte(&i2c->rx_queue, 0x00);
      goto done;
      
    case STATE_RESTART:
      reg->con |= 0x0002;  // send restart
      i2c->message_state = STATE_ADDR_READ;
      break;
      
    case STATE_ADDR_READ:
//      log_printf("writing: 0x%x", i2c->cur_tx_header.addr1 | 0x01);
      reg->trn = i2c->cur_tx_header.addr1 | 0x01;  // read address
      i2c->message_state = STATE_ACK_ADDR_READ;
      break;

    case STATE_ACK_ADDR_READ:
      if (reg->stat >> 15) goto error;
      // from now on, we can no longer fail.
      i2c->bytes_remaining = i2c->cur_tx_header.read_size;
      ByteQueuePushByte(&i2c->rx_queue, i2c->cur_tx_header.read_size);
      reg->con |= 0x0008;  // RCEN
      i2c->message_state = STATE_READ_DATA;
      break;

    case STATE_READ_DATA:
      ByteQueuePushByte(&i2c->rx_queue, reg->rcv);
      reg->con |= (1 << 4)
                  | (i2c->bytes_remaining == 1) << 5;  // nack last byte
      i2c->message_state = STATE_ACK_READ_DATA;
      break;

    case STATE_ACK_READ_DATA:
      if (--i2c->bytes_remaining == 0) {
        goto done;
      } else {
        reg->con |= 0x0008;  // RCEN
        i2c->message_state = STATE_READ_DATA;
      }
      break;
  }
  return;
  
error:
  log_printf("error");
  // pull remainder of tx message
  ByteQueuePull(&i2c->tx_queue, i2c->bytes_remaining);
  i2c->num_tx_since_last_report += i2c->bytes_remaining;
  ByteQueuePushByte(&i2c->rx_queue, 0xFF);

done:
  log_printf("done");
  ++i2c->num_messages_rx_queue;
  reg->con |= (1 << 2);  // send stop bit
  i2c->message_state = STATE_START;
  SetMIE[i2c_num](ByteQueueSize(&i2c->tx_queue) > 0);
}

#define DEFINE_INTERRUPT_HANDLERS(i2c_num)                                     \
  void __attribute__((__interrupt__, auto_psv)) _MI2C##i2c_num##Interrupt() {  \
    MI2CInterrupt(i2c_num - 1);                                                \
  }

#if NUM_I2C_MODULES > 3
  #error Currently only devices with 3 or less I2Cs are supported. Please fix below.
#endif

#if NUM_I2C_MODULES >= 1
  DEFINE_INTERRUPT_HANDLERS(1)
#endif

#if NUM_I2C_MODULES >= 2
  DEFINE_INTERRUPT_HANDLERS(2)
#endif

#if NUM_I2C_MODULES >= 3
  DEFINE_INTERRUPT_HANDLERS(3)
#endif
