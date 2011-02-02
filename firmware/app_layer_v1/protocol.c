#include "protocol.h"

#include <assert.h>
#include <string.h>
#include "blapi/bootloader.h"
#include "byte_queue.h"
#include "features.h"
#include "protocol_defs.h"
#include "logging.h"
#include "board.h"

#define CHECK(cond) do { if (!(cond)) { log_printf("Check failed: %s", #cond); return FALSE; }} while(0)

#define FIRMWARE_ID              0x00000001LL

const BYTE incoming_arg_size[MESSAGE_TYPE_LIMIT] = {
  sizeof(HARD_RESET_ARGS),
  sizeof(SOFT_RESET_ARGS),
  sizeof(SET_PIN_DIGITAL_OUT_ARGS),
  sizeof(SET_DIGITAL_OUT_LEVEL_ARGS),
  sizeof(SET_PIN_DIGITAL_IN_ARGS),
  sizeof(SET_CHANGE_NOTIFY_ARGS),
  sizeof(REGISTER_PERIODIC_DIGITAL_SAMPLING_ARGS),
  sizeof(RESERVED_ARGS),
  sizeof(SET_PIN_PWM_ARGS),
  sizeof(SET_PWM_DUTY_CYCLE_ARGS),
  sizeof(SET_PWM_PERIOD_ARGS)
  // BOOKMARK(add_feature): Add sizeof (argument for incoming message).
  // Array is indexed by message type enum.
};

const BYTE outgoing_arg_size[MESSAGE_TYPE_LIMIT] = {
  sizeof(ESTABLISH_CONNECTION_ARGS),
  sizeof(SOFT_RESET_ARGS),
  sizeof(SET_PIN_DIGITAL_OUT_ARGS),
  sizeof(REPORT_DIGITAL_IN_STATUS_ARGS),
  sizeof(SET_PIN_DIGITAL_IN_ARGS),
  sizeof(SET_CHANGE_NOTIFY_ARGS),
  sizeof(REGISTER_PERIODIC_DIGITAL_SAMPLING_ARGS),
  sizeof(RESERVED_ARGS),
  sizeof(RESERVED_ARGS),
  sizeof(RESERVED_ARGS)
  // BOOKMARK(add_feature): Add sizeof (argument for outgoing message).
  // Array is indexed by message type enum.
};

DEFINE_STATIC_BYTE_QUEUE(tx_queue, 1024);
static int bytes_transmitted;

static INCOMING_MESSAGE rx_msg;
static int rx_buffer_cursor;
static int rx_message_remaining;

static BYTE OutgoingMessageLength(const OUTGOING_MESSAGE* msg) {
  return sizeof(BYTE) + outgoing_arg_size[msg->type];
  // TODO: handle variable size
}

void AppProtocolInit(ADB_CHANNEL_HANDLE h) {
  bytes_transmitted = 0;
  rx_buffer_cursor = 0;
  rx_message_remaining = 0;

  OUTGOING_MESSAGE msg;
  msg.type = ESTABLISH_CONNECTION;
  msg.args.establish_connection.magic = IOIO_MAGIC;
  // TODO: read those from ROM somehow
  msg.args.establish_connection.hardware = 0;  // HardwareVer
  msg.args.establish_connection.bootloader = 1;  // BootloaderVer
  msg.args.establish_connection.firmware = FIRMWARE_ID;
  AppProtocolSendMessage(&msg);
}

void AppProtocolSendMessage(const OUTGOING_MESSAGE* msg) {
  ByteQueueLock(tx_queue);
  ByteQueuePushBuffer(&tx_queue, (const BYTE*) msg, OutgoingMessageLength(msg));
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
  AppProtocolSendMessage((const OUTGOING_MESSAGE*) &rx_msg);
}

static BOOL MessageDone() {
  switch (rx_msg.type) {
    case HARD_RESET:
      CHECK(rx_msg.args.hard_reset.magic == IOIO_MAGIC);
      HardReset();
      break;

    case SOFT_RESET:
      SoftReset();
      Echo();
      break;

    case SET_PIN_DIGITAL_OUT:
      CHECK(rx_msg.args.set_pin_digital_out.pin < NUM_PINS);
      SetPinDigitalOut(rx_msg.args.set_pin_digital_out.pin,
                       rx_msg.args.set_pin_digital_out.value,
                       rx_msg.args.set_pin_digital_out.open_drain);
      Echo();
      break;

    case SET_DIGITAL_OUT_LEVEL:
      CHECK(rx_msg.args.set_digital_out_level.pin < NUM_PINS);
      SetDigitalOutLevel(rx_msg.args.set_digital_out_level.pin,
                         rx_msg.args.set_digital_out_level.value);
      break;

    case SET_PIN_DIGITAL_IN:
      CHECK(rx_msg.args.set_pin_digital_in.pin < NUM_PINS);
      CHECK(rx_msg.args.set_pin_digital_in.pull < 3);
      SetPinDigitalIn(rx_msg.args.set_pin_digital_in.pin, rx_msg.args.set_pin_digital_in.pull);
      Echo();
      break;

    case SET_CHANGE_NOTIFY:
      CHECK(rx_msg.args.set_change_notify.pin < NUM_PINS);
      SetChangeNotify(rx_msg.args.set_change_notify.pin, rx_msg.args.set_change_notify.cn);
      Echo();
      if (rx_msg.args.set_change_notify.cn) {
        ReportDigitalInStatus(rx_msg.args.set_change_notify.pin);
      }
      break;

    case SET_PIN_PWM:
      CHECK(rx_msg.args.set_pin_pwm.pin < NUM_PINS);
      CHECK(rx_msg.args.set_pin_pwm.pwmNum < NUM_PWMS);
      SetPinPwm(rx_msg.args.set_pin_pwm.pin, rx_msg.args.set_pin_pwm.pwmNum);
      break;

    case SET_PWM_DUTY_CYCLE:
      CHECK(rx_msg.args.set_pwm_duty_cycle.pwmNum < NUM_PWMS);
      SetPwmDutyCycle(rx_msg.args.set_pwm_duty_cycle.pwmNum,
                      rx_msg.args.set_pwm_duty_cycle.dc_lsb,
                      rx_msg.args.set_pwm_duty_cycle.dc_msb);
      break;

    case SET_PWM_PERIOD:
      CHECK(rx_msg.args.set_pwm_period.pwmNum < NUM_PWMS);
      SetPwmPeriod(rx_msg.args.set_pwm_period.pwmNum,
                   rx_msg.args.set_pwm_period.period,
                   rx_msg.args.set_pwm_period.scale256);
      break;

    // BOOKMARK(add_feature): Add incoming message handling to switch clause.
    // Call Echo() if the message is to be echoed back.

    default:
      return FALSE;
  }
  rx_message_remaining = 0;
  rx_buffer_cursor = 0;
  return TRUE;
}

BOOL AppProtocolHandleIncoming(const BYTE* data, UINT32 data_len) {
  assert(data);

  while (data_len > 0) {
    if (rx_message_remaining == 0) {
      ((BYTE *) &rx_msg)[rx_buffer_cursor++] = data[0];
      ++data;
      --data_len;
      if (rx_msg.type < MESSAGE_TYPE_LIMIT) {
        rx_message_remaining = incoming_arg_size[rx_msg.type] & 0x7F;
        if (rx_message_remaining == 0) {
          // no args
          if (!MessageDone()) return FALSE;
        }
      } else {
        return FALSE;
      }
    } else {
      if (data_len >= rx_message_remaining) {
        memcpy(((BYTE *) &rx_msg) + rx_buffer_cursor, data, rx_message_remaining);
        data += rx_message_remaining;
        data_len -= rx_message_remaining;
        // TODO: handle the case of varialbe length data
        if (!MessageDone()) return FALSE;
      } else {
        memcpy(((BYTE *) &rx_msg) + rx_buffer_cursor, data, data_len);
        rx_buffer_cursor += data_len;
        rx_message_remaining -= data_len;
        data_len = 0;
      }
    }
  }
  return TRUE;
}
