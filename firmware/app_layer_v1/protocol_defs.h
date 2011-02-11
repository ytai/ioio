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
  BYTE pwm_num : 4;
  BYTE : 4;
} SET_PIN_PWM_ARGS;

// set pwm duty cycle
typedef struct PACKED {
  BYTE fraction : 2;
  BYTE pwm_num : 4;
  BYTE : 2;
  WORD dc;
} SET_PWM_DUTY_CYCLE_ARGS;

// set pwm period
typedef struct PACKED {
  BYTE scale256 : 1;
  BYTE pwm_num : 4;
  BYTE : 3;
  WORD period;
} SET_PWM_PERIOD_ARGS;

typedef struct PACKED {
  BYTE pin;
} SET_PIN_ANALOG_IN_ARGS;

typedef struct PACKED {
  BYTE num_pins;
} REPORT_ANALOG_IN_FORMAT_ARGS;

typedef struct PACKED {
} REPORT_ANALOG_IN_STATUS_ARGS;

typedef struct PACKED {
  BYTE size : 6;
  BYTE uart_num : 2;
  BYTE data[0];
} UART_DATA_ARGS;

typedef struct PACKED {
  BYTE parity : 2;
  BYTE two_stop_bits : 1;
  BYTE speed4x : 1;
  BYTE : 2;
  BYTE uart_num : 2;
  WORD rate;
} UART_CONFIG_ARGS;

typedef struct PACKED {
  BYTE pin : 6;
  BYTE : 2;
  BYTE uart_num : 2;
  BYTE : 5;
  BYTE enable : 1;
} SET_PIN_UART_RX_ARGS;

typedef struct PACKED {
  BYTE pin : 6;
  BYTE : 2;
  BYTE uart_num : 2;
  BYTE : 5;
  BYTE enable : 1;
} SET_PIN_UART_TX_ARGS;

// BOOKMARK(add_feature): Add a struct for the new incoming / outgoing message
// arguments.

typedef struct PACKED {
  BYTE type;
  union PACKED {
    HARD_RESET_ARGS                          hard_reset;
    SOFT_RESET_ARGS                          soft_reset;
    SET_PIN_DIGITAL_OUT_ARGS                 set_pin_digital_out;
    SET_DIGITAL_OUT_LEVEL_ARGS               set_digital_out_level;
    SET_PIN_DIGITAL_IN_ARGS                  set_pin_digital_in;
    SET_CHANGE_NOTIFY_ARGS                   set_change_notify;
    REGISTER_PERIODIC_DIGITAL_SAMPLING_ARGS  register_periodic_digital_sampling;
    SET_PIN_PWM_ARGS                         set_pin_pwm;
    SET_PWM_DUTY_CYCLE_ARGS                  set_pwm_duty_cycle; 
    SET_PWM_PERIOD_ARGS                      set_pwm_period;
    SET_PIN_ANALOG_IN_ARGS                   set_pin_analog_in;
    UART_DATA_ARGS                           uart_data;
    UART_CONFIG_ARGS                         uart_config;
    SET_PIN_UART_RX_ARGS                     set_pin_uart_rx;
    SET_PIN_UART_TX_ARGS                     set_pin_uart_tx;
    // BOOKMARK(add_feature): Add argument struct to the union.
  } args;
  BYTE __vabuf[64];  // buffer for var args. never access directly!
} INCOMING_MESSAGE;

typedef struct PACKED {
  BYTE type;
  union PACKED {
    ESTABLISH_CONNECTION_ARGS               establish_connection;
    REPORT_DIGITAL_IN_STATUS_ARGS           report_digital_in_status;
    REPORT_PERIODIC_DIGITAL_IN_STATUS_ARGS  report_periodic_digital_in_status;
    REPORT_ANALOG_IN_FORMAT_ARGS            report_analog_in_format;
    REPORT_ANALOG_IN_STATUS_ARGS            report_analog_in_status;
    UART_DATA_ARGS                          uart_data;
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
  REPORT_ANALOG_IN_FORMAT           = 0x08,
  SET_PWM_DUTY_CYCLE                = 0x09,
  REPORT_ANALOG_IN_STATUS           = 0x09,
  SET_PWM_PERIOD                    = 0x0A,
  SET_PIN_ANALOG_IN                 = 0x0B,
  UART_DATA                         = 0x0C,
  UART_CONFIG                       = 0x0D,
  SET_PIN_UART_RX                   = 0x0E,
  SET_PIN_UART_TX                   = 0x0F,

  // BOOKMARK(add_feature): Add new message type to enum.
  MESSAGE_TYPE_LIMIT
} MESSAGE_TYPE;


#endif  // __PROTOCOLDEFS_H__
