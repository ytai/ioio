#ifndef __PROTOCOLDEFS_H__
#define __PROTOCOLDEFS_H__


#define PACKED __attribute__ ((packed))

#define IOIO_MAGIC               0x4F494F49L

// hard reset
typedef struct PACKED {
  DWORD magic;
} HARD_RESET_ARGS;

// establish connection
typedef struct PACKED {
  DWORD magic;
  DWORD hardware : 24;
  DWORD bootloader : 24;
  DWORD firmware : 24;
} ESTABLISH_CONNECTION_ARGS;

// soft reset
typedef struct PACKED {
} SOFT_RESET_ARGS;

// set pin digital out
typedef struct PACKED {
  BYTE open_drain : 1;
  BYTE value : 1;
  BYTE pin : 6;
} SET_PIN_DIGITAL_OUT_ARGS;

// set digital out level
typedef struct PACKED {
  BYTE value : 1;
  BYTE : 1;
  BYTE pin : 6;
} SET_DIGITAL_OUT_LEVEL_ARGS;

// report digital in status
typedef struct PACKED {
  BYTE level : 1;
  BYTE : 1;
  BYTE pin : 6;
} REPORT_DIGITAL_IN_STATUS_ARGS;

// set pin digital in
typedef struct PACKED {
  BYTE pull : 2;
  BYTE pin : 6;
} SET_PIN_DIGITAL_IN_ARGS;

// set change notify
typedef struct PACKED {
  BYTE cn : 1;
  BYTE : 1;
  BYTE pin : 6;
} SET_CHANGE_NOTIFY_ARGS;

// register periodic digital sampling
typedef struct PACKED {
  BYTE pin : 6;
  BYTE : 2;
  BYTE freq_scale;
} REGISTER_PERIODIC_DIGITAL_SAMPLING_ARGS;

// report periodic digital in status
typedef struct PACKED {
  BYTE size;
} REPORT_PERIODIC_DIGITAL_IN_STATUS_ARGS;

// reserved
typedef struct PACKED {
  // for future use
} RESERVED_ARGS;

// set pin pwm
typedef struct PACKED {
  BYTE pin : 6;
  BYTE : 2;
  BYTE pwmNum : 4;
  BYTE : 4;
} SET_PIN_PWM_ARGS;

// set pwm duty cycle
typedef struct PACKED {
  BYTE dc_lsb : 2;
  BYTE pwmNum : 4;
  BYTE : 2;
  WORD dc_msb;
} SET_PWM_DUTY_CYCLE_ARGS;

// set pwm period
typedef struct PACKED {
  BYTE scale256 : 1;
  BYTE pwmNum : 4;
  BYTE : 3;
  WORD period;
} SET_PWM_PERIOD_ARGS;

// BOOKMARK(add_feature): Add a struct for the new incoming / outgoing message
// arguments.

typedef struct PACKED {
  BYTE type;
  union PACKED {
    HARD_RESET_ARGS hard_reset;
    SOFT_RESET_ARGS soft_reset;
    SET_PIN_DIGITAL_OUT_ARGS set_pin_digital_out;
    SET_DIGITAL_OUT_LEVEL_ARGS set_digital_out_level;
    SET_PIN_DIGITAL_IN_ARGS set_pin_digital_in;
    SET_CHANGE_NOTIFY_ARGS set_change_notify;
    REGISTER_PERIODIC_DIGITAL_SAMPLING_ARGS register_periodic_digital_sampling;
    SET_PIN_PWM_ARGS set_pin_pwm;
    SET_PWM_DUTY_CYCLE_ARGS set_pwm_duty_cycle; 
    SET_PWM_PERIOD_ARGS set_pwm_period; 
    // BOOKMARK(add_feature): Add argument struct to the union.
  } args;
} INCOMING_MESSAGE;

typedef struct PACKED {
  BYTE type;
  union PACKED {
    ESTABLISH_CONNECTION_ARGS establish_connection;
    SOFT_RESET_ARGS soft_reset;
    SET_PIN_DIGITAL_OUT_ARGS set_pin_digital_out;
    REPORT_DIGITAL_IN_STATUS_ARGS report_digital_in_status;
    SET_PIN_DIGITAL_IN_ARGS set_pin_digital_in;
    SET_CHANGE_NOTIFY_ARGS set_change_notify;
    REPORT_PERIODIC_DIGITAL_IN_STATUS_ARGS report_periodic_digital_in_status;
    // BOOKMARK(add_feature): Add argument struct to the union.
  } args;
} OUTGOING_MESSAGE;


typedef enum {
  HARD_RESET                        = 0x00,
  ESTABLISH_CONNECTION              = 0x00,
  SOFT_RESET                        = 0x01,
  SET_PIN_DIGITAL_OUT               = 0x02,
  SET_DIGITAL_OUT_LEVEL             = 0x03,
  REPORT_DIGITAL_IN_STATUS          = 0x03,
  SET_PIN_DIGITAL_IN                = 0x04,
  SET_CHANGE_NOTIFY                 = 0x05,
  REGISTER_PERIOD_DIGITAL_SAMPLING  = 0x06,
  REPORT_PERIODIC_DIGITAL_IN_STATUS = 0x07,
  SET_PIN_PWM                       = 0x08,
  SET_PWM_DUTY_CYCLE                = 0x09,
  SET_PWM_PERIOD                    = 0x0A,
  // BOOKMARK(add_feature): Add new message type to enum.
  MESSAGE_TYPE_LIMIT
} MESSAGE_TYPE;


#endif  // __PROTOCOLDEFS_H__
