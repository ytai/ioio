#include "protocol.h"

#include <assert.h>
#include <string.h>
#include "blapi/bootloader.h"
#include "byte_queue.h"
#include "features.h"

#define CMD_ESTABLISH_CONNECTION 0x00

#define IOIO_MAGIC               0x49454945LL
#define FIRMWARE_ID              0x00000001LL

#define PACKED __attribute__ ((packed))

// hard reset
typedef struct PACKED {
  DWORD magic;
} HARD_RESET_ARGS;

// soft reset
typedef struct PACKED {
} SOFT_RESET_ARGS;

// set pin digital out
typedef struct PACKED {
  BYTE open_drain : 1;
  BYTE : 1;
  BYTE pin : 6;
} SET_PIN_DIGITAL_OUT_ARGS;

// set digital out level
typedef struct PACKED {
  BYTE value : 1;
  BYTE : 1;
  BYTE pin : 6;
} SET_DIGITAL_OUT_LEVEL_ARGS;

typedef union PACKED {
  struct PACKED {
    BYTE type;
    union PACKED {
      HARD_RESET_ARGS hard_reset;
      SOFT_RESET_ARGS soft_reset;
      SET_PIN_DIGITAL_OUT_ARGS set_pin_digital_out;
      SET_DIGITAL_OUT_LEVEL_ARGS set_digital_out_level;
    } args;
  } msg;

  BYTE raw[1];
} INCOMING_MESSAGE;

typedef enum {
  HARD_RESET              = 0x00,
  SOFT_RESET              = 0x01,
  SET_PIN_DIGITAL_OUT     = 0x02,
  SET_DIGITAL_OUT_LEVEL   = 0x03,
  MESSAGE_TYPE_LIMIT
} MESSAGE_TYPE;

const BYTE arg_size[MESSAGE_TYPE_LIMIT] = {
  sizeof(HARD_RESET_ARGS),
  sizeof(SOFT_RESET_ARGS),
  sizeof(SET_PIN_DIGITAL_OUT_ARGS),
  sizeof(SET_DIGITAL_OUT_LEVEL_ARGS)
};

DEFINE_STATIC_BYTE_QUEUE(tx_queue, 1024);
static int bytes_transmitted;

static INCOMING_MESSAGE rx_msg;
static int rx_buffer_cursor;
static int rx_message_remaining;

void AppProtocolInit(ADB_CHANNEL_HANDLE h) {
  bytes_transmitted = 0;
  rx_buffer_cursor = 0;
  rx_message_remaining = 0;

  ByteQueueLock(tx_queue);
  ByteQueuePushByte(&tx_queue, CMD_ESTABLISH_CONNECTION);
  ByteQueuePushDword(&tx_queue, IOIO_MAGIC);
  // TODO: read those from ROM somehow
  ByteQueuePushInt24(&tx_queue, 0 /*HardwareVer*/);
  ByteQueuePushInt24(&tx_queue, 0 /*BootloaderVer*/);
  ByteQueuePushInt24(&tx_queue, FIRMWARE_ID);
  ByteQueueUnlock(tx_queue);
}

void AppProtocolTasks(ADB_CHANNEL_HANDLE h) {
  if (ADBChannelReady(h)) {
    ByteQueueLock(tx_queue);
    BYTE* data;
    int size;
    if (bytes_transmitted) {
      ByteQueuePull(&tx_queue, bytes_transmitted);
      bytes_transmitted = 0;
    }
    ByteQueuePeek(&tx_queue, &data, &size);
    if (size > 0) {
      ADBWrite(h, data, size);
      bytes_transmitted = size;
    }
    ByteQueueUnlock(tx_queue);
  }
}

static void Echo() {
  ByteQueueLock(tx_queue);
  ByteQueuePushBuffer(&tx_queue, rx_msg.raw, rx_buffer_cursor);
  ByteQueueUnlock(tx_queue);
}

static void MessageDone() {
  // TODO: fill
  switch (rx_msg.msg.type) {
    case HARD_RESET:
      break;

    case SOFT_RESET:
      break;

    case SET_PIN_DIGITAL_OUT:
      SetPinDigitalOut(rx_msg.msg.args.set_pin_digital_out.pin, rx_msg.msg.args.set_pin_digital_out.open_drain);
      Echo();
      break;

    case SET_DIGITAL_OUT_LEVEL:
      SetDigitalOutLevel(rx_msg.msg.args.set_digital_out_level.pin, rx_msg.msg.args.set_digital_out_level.value);
      break;    
  }
  rx_message_remaining = 0;
  rx_buffer_cursor = 0;
}

void AppProtocolHandleIncoming(const BYTE* data, UINT32 data_len) {
  assert(data);

  while (data_len > 0) {
    if (rx_message_remaining == 0) {
      rx_msg.raw[rx_buffer_cursor++] = data[0];
      ++data;
      --data_len;
      if (rx_msg.msg.type < MESSAGE_TYPE_LIMIT) {
        rx_message_remaining = arg_size[rx_msg.msg.type] & 0x7F;
      } else {
        // TODO: go to error state
      }
    } else {
      if (data_len >= rx_message_remaining) {
        memcpy(rx_msg.raw + rx_buffer_cursor, data, rx_message_remaining);
        data += rx_message_remaining;
        data_len -= rx_message_remaining;
        // TODO: handle the case of varialbe length data
        MessageDone();
      } else {
        memcpy(rx_msg.raw + rx_buffer_cursor, data, data_len);
        rx_buffer_cursor += data_len;
        rx_message_remaining -= data_len;
        data_len = 0;
      }
    }
  }
}
