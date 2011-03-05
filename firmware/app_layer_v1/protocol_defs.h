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

// uart report tx status
typedef struct PACKED {
  BYTE uart_num : 2;
  WORD bytes_remaining : 14;
} UART_REPORT_TX_STATUS_ARGS;

// set pin analog in
typedef struct PACKED {
  BYTE pin;
} SET_PIN_ANALOG_IN_ARGS;

// report analog in format
typedef struct PACKED {
  BYTE num_pins;
} REPORT_ANALOG_IN_FORMAT_ARGS;

// report analog in status
typedef struct PACKED {
} REPORT_ANALOG_IN_STATUS_ARGS;

// uart data
typedef struct PACKED {
  BYTE size : 6;
  BYTE uart_num : 2;
  BYTE data[0];
} UART_DATA_ARGS;

// uart config
typedef struct PACKED {
  BYTE parity : 2;
  BYTE two_stop_bits : 1;
  BYTE speed4x : 1;
  BYTE : 2;
  BYTE uart_num : 2;
  WORD rate;
} UART_CONFIG_ARGS;

// set pin uart rx
typedef struct PACKED {
  BYTE pin : 6;
  BYTE : 2;
  BYTE uart_num : 2;
  BYTE : 5;
  BYTE enable : 1;
} SET_PIN_UART_RX_ARGS;

// set pin uart tx
typedef struct PACKED {
  BYTE pin : 6;
  BYTE : 2;
  BYTE uart_num : 2;
  BYTE : 5;
  BYTE enable : 1;
} SET_PIN_UART_TX_ARGS;

// spi report tx status
typedef struct PACKED {
  BYTE spi_num : 2;
  WORD bytes_remaining : 14;
} SPI_REPORT_TX_STATUS_ARGS;

// spi data
typedef struct PACKED {
  BYTE size : 6;
  BYTE spi_num : 2;
  BYTE ss_pin : 6;
  BYTE : 2;
  BYTE data[0];
} SPI_DATA_ARGS;

// spi master request
typedef struct PACKED {
  BYTE ss_pin : 6;
  BYTE spi_num : 2;
  BYTE total_size : 6;
  BYTE res_size_neq_total : 1;
  BYTE data_size_neq_total : 1;
  union {
    BYTE data_size;
    BYTE vararg[0];
  };
} SPI_MASTER_REQUEST_ARGS;

// spi configure master
typedef struct PACKED {
  BYTE div : 3;
  BYTE scale : 2;
  BYTE spi_num : 2;
  BYTE : 1;
  BYTE clk_pol : 1;
  BYTE clk_edge : 1;
  BYTE smp_end : 1;
  BYTE : 5;
} SPI_CONFIGURE_MASTER_ARGS;

// set pin spi
typedef struct PACKED {
  BYTE pin : 6;
  BYTE : 2;
  BYTE spi_num : 2;
  BYTE mode : 2;
  BYTE enable : 1;
  BYTE : 3;
} SET_PIN_SPI_ARGS;

// i2c configure master
typedef struct PACKED {
  BYTE i2c_num : 2;
  BYTE : 3;
  BYTE rate : 2;
  BYTE smbus_levels : 1;
} I2C_CONFIGURE_MASTER_ARGS;

// i2c write read
typedef struct PACKED {
  BYTE i2c_num : 2;
  BYTE ten_bit_addr : 1;
  BYTE : 3;
  BYTE addr_msb : 2;
  BYTE addr_lsb;
  BYTE write_size;
  BYTE read_size;
  BYTE data[0];
} I2C_WRITE_READ_ARGS;

// i2c result
typedef struct PACKED {
  BYTE i2c_num : 2;
  BYTE : 6;
  BYTE size;
} I2C_RESULT_ARGS;

// i2c report tx status
typedef struct PACKED {
  BYTE i2c_num : 2;
  WORD bytes_remaining : 14;
} I2C_REPORT_TX_STATUS_ARGS;

// set pin i2c
// set pin spi
typedef struct PACKED {
} SET_PIN_I2C_ARGS;

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
    SPI_MASTER_REQUEST_ARGS                  spi_master_request;
    SPI_CONFIGURE_MASTER_ARGS                spi_configure_master;
    SET_PIN_SPI_ARGS                         set_pin_spi;
    I2C_CONFIGURE_MASTER_ARGS                i2c_configure_master;
    I2C_WRITE_READ_ARGS                      i2c_write_read;
    SET_PIN_I2C_ARGS                         set_pin_i2c;
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
    UART_REPORT_TX_STATUS_ARGS              uart_report_tx_status;
    UART_DATA_ARGS                          uart_data;
    SPI_REPORT_TX_STATUS_ARGS               spi_report_tx_status;
    SPI_DATA_ARGS                           spi_data;
    I2C_RESULT_ARGS                         i2c_result;
    I2C_REPORT_TX_STATUS_ARGS               i2c_report_tx_status;
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
  UART_REPORT_TX_STATUS             = 0x0A,
  SET_PIN_ANALOG_IN                 = 0x0B,
  UART_DATA                         = 0x0C,
  UART_CONFIG                       = 0x0D,
  SET_PIN_UART_RX                   = 0x0E,
  SET_PIN_UART_TX                   = 0x0F,
  SPI_MASTER_REQUEST                = 0x10,
  SPI_DATA                          = 0x10,
  SPI_REPORT_TX_STATUS              = 0x11,
  SPI_CONFIGURE_MASTER              = 0x12,
  SET_PIN_SPI                       = 0x13,
  I2C_CONFIGURE_MASTER              = 0x14,
  I2C_WRITE_READ                    = 0x15,
  I2C_RESULT                        = 0x15,
  I2C_REPORT_TX_STATUS              = 0x16,
  SET_PIN_I2C                       = 0x17,

  // BOOKMARK(add_feature): Add new message type to enum.
  MESSAGE_TYPE_LIMIT
} MESSAGE_TYPE;


#endif  // __PROTOCOLDEFS_H__
