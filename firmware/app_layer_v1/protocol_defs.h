/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */

#ifndef __PROTOCOLDEFS_H__
#define __PROTOCOLDEFS_H__


#define PACKED __attribute__ ((packed))

#define IOIO_MAGIC 0x4F494F49L

#define PROTOCOL_IID_YTAI0001 "YTAI0001"
#define PROTOCOL_IID_YTAI0002 "YTAI0002"
#define PROTOCOL_IID_IOIO0001 "IOIO0001"
#define PROTOCOL_IID_IOIO0002 "IOIO0002"
#define PROTOCOL_IID_IOIO0003 "IOIO0003"

// hard reset
typedef struct PACKED {
  DWORD magic;
} HARD_RESET_ARGS;

// establish connection
typedef struct PACKED {
  DWORD magic;
  BYTE hw_impl_ver[8];
  BYTE bl_impl_ver[8];
  BYTE fw_impl_ver[8];
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
  BYTE : 3;
  BYTE enable : 1;
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
  BYTE scale_l : 1;
  BYTE pwm_num : 4;
  BYTE : 2;
  BYTE scale_h : 1;
  WORD period;
} SET_PWM_PERIOD_ARGS;

// uart report tx status
typedef struct PACKED {
  BYTE uart_num : 2;
  WORD bytes_to_add : 14;
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

// uart status
typedef struct PACKED {
  BYTE uart_num : 2;
  BYTE : 5;
  BYTE enabled : 1;
} UART_STATUS_ARGS;

// set pin uart
typedef struct PACKED {
  BYTE pin : 6;
  BYTE : 2;
  BYTE uart_num : 2;
  BYTE : 4;
  BYTE dir : 1;
  BYTE enable : 1;
} SET_PIN_UART_ARGS;

// spi report tx status
typedef struct PACKED {
  BYTE spi_num : 2;
  WORD bytes_to_add : 14;
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

// spi status
typedef struct PACKED {
  BYTE spi_num : 2;
  BYTE : 5;
  BYTE enabled : 1;
} SPI_STATUS_ARGS;

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

// i2c status
typedef struct PACKED {
  BYTE i2c_num : 2;
  BYTE : 5;
  BYTE enabled : 1;
} I2C_STATUS_ARGS;

// i2c write read
typedef struct PACKED {
  BYTE i2c_num : 2;
  BYTE : 3;
  BYTE ten_bit_addr : 1;
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
  WORD bytes_to_add : 14;
} I2C_REPORT_TX_STATUS_ARGS;

// set analog pin sampling
typedef struct PACKED {
  BYTE pin : 6;
  BYTE : 1;
  BYTE enable : 1;
} SET_ANALOG_IN_SAMPLING_ARGS;

// check interface
typedef struct PACKED {
  BYTE interface_id[8];
} CHECK_INTERFACE_ARGS;

// check interface response
typedef struct PACKED {
  BYTE supported : 1;
  BYTE : 7;
} CHECK_INTERFACE_RESPONSE_ARGS;

// icsp six
typedef struct PACKED {
    DWORD inst : 24;
} ICSP_SIX_ARGS;

// icsp report rx status
typedef struct PACKED {
    WORD bytes_to_add;
} ICSP_REPORT_RX_STATUS_ARGS;

// icsp regout
typedef struct PACKED {
} ICSP_REGOUT_ARGS;

// icsp result
typedef struct PACKED {
    WORD reg;
} ICSP_RESULT_ARGS;

// icsp programming enter
typedef struct PACKED {
} ICSP_PROG_ENTER_ARGS;

// icsp programming exit
typedef struct PACKED {
} ICSP_PROG_EXIT_ARGS;

// icsp configure
typedef struct PACKED {
    BYTE enable : 1;
} ICSP_CONFIG_ARGS;

// incap configure
typedef struct PACKED {
    BYTE incap_num : 4;
    BYTE : 4;
    BYTE clock : 2;
    BYTE : 1;
    BYTE mode : 3;
    BYTE : 1;
    BYTE double_prec : 1;
} INCAP_CONFIG_ARGS;

// incap status
typedef struct PACKED {
    BYTE incap_num : 4;
    BYTE : 3;
    BYTE enabled : 1;
} INCAP_STATUS_ARGS;

// set pin incap
typedef struct PACKED {
    BYTE pin : 6;
    BYTE : 2;
    BYTE incap_num : 4;
    BYTE : 3;
    BYTE enable : 1;
} SET_PIN_INCAP_ARGS;

// incap report
typedef struct PACKED {
    BYTE incap_num : 4;
    BYTE : 2;
    BYTE size : 2;
} INCAP_REPORT_ARGS;

// soft close
typedef struct PACKED {
} SOFT_CLOSE_ARGS;

// rgb led matrix enable
typedef struct PACKED {
  BYTE len : 3;
} RGB_LED_MATRIX_ENABLE_ARGS;

// rgb led matrix frame
typedef struct PACKED {
  BYTE data[0];
} RGB_LED_MATRIX_FRAME_ARGS;

// rgb led matrix frame
typedef struct PACKED {
  WORD frame_delay;
  BYTE shifter_len : 3;
} RGB_LED_MATRIX_WRITE_FILE_ARGS;

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
    SET_PIN_UART_ARGS                        set_pin_uart;
    SPI_MASTER_REQUEST_ARGS                  spi_master_request;
    SPI_CONFIGURE_MASTER_ARGS                spi_configure_master;
    SET_PIN_SPI_ARGS                         set_pin_spi;
    I2C_CONFIGURE_MASTER_ARGS                i2c_configure_master;
    I2C_WRITE_READ_ARGS                      i2c_write_read;
    SET_ANALOG_IN_SAMPLING_ARGS              set_analog_pin_sampling;
    CHECK_INTERFACE_ARGS                     check_interface;
    ICSP_SIX_ARGS                            icsp_six;
    ICSP_REGOUT_ARGS                         icsp_regout;
    ICSP_PROG_ENTER_ARGS                     icsp_prog_enter;
    ICSP_PROG_EXIT_ARGS                      icsp_prog_exit;
    ICSP_CONFIG_ARGS                         icsp_config;
    INCAP_CONFIG_ARGS                        incap_config;
    SET_PIN_INCAP_ARGS                       set_pin_incap;
    SOFT_CLOSE_ARGS                          soft_close;
    RGB_LED_MATRIX_ENABLE_ARGS               rgb_led_matrix_enable;
    RGB_LED_MATRIX_FRAME_ARGS                rgb_led_matrix_frame;
    RGB_LED_MATRIX_WRITE_FILE_ARGS           rgb_led_matrix_write_file;

    // BOOKMARK(add_feature): Add argument struct to the union.
  } args;
  BYTE __vabuf[1536];  // buffer for var args. never access directly!
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
    CHECK_INTERFACE_RESPONSE_ARGS           check_interface_response;
    UART_STATUS_ARGS                        uart_status;
    SPI_STATUS_ARGS                         spi_status;
    I2C_STATUS_ARGS                         i2c_status;
    ICSP_RESULT_ARGS                        icsp_result;
    ICSP_REPORT_RX_STATUS_ARGS              icsp_report_rx_status;
    INCAP_STATUS_ARGS                       incap_status;
    INCAP_REPORT_ARGS                       incap_report;
    SOFT_CLOSE_ARGS                         soft_close;
    // BOOKMARK(add_feature): Add argument struct to the union.
  } args;
} OUTGOING_MESSAGE;


typedef enum {
  HARD_RESET                          = 0x00,
  ESTABLISH_CONNECTION                = 0x00,
  SOFT_RESET                          = 0x01,
  CHECK_INTERFACE                     = 0x02,
  CHECK_INTERFACE_RESPONSE            = 0x02,

  SET_PIN_DIGITAL_OUT                 = 0x03,
  SET_DIGITAL_OUT_LEVEL               = 0x04,
  REPORT_DIGITAL_IN_STATUS            = 0x04,
  SET_PIN_DIGITAL_IN                  = 0x05,
  REPORT_PERIODIC_DIGITAL_IN_STATUS   = 0x05,
  SET_CHANGE_NOTIFY                   = 0x06,
  REGISTER_PERIODIC_DIGITAL_SAMPLING  = 0x07,

  SET_PIN_PWM                         = 0x08,
  SET_PWM_DUTY_CYCLE                  = 0x09,
  SET_PWM_PERIOD                      = 0x0A,

  SET_PIN_ANALOG_IN                   = 0x0B,
  REPORT_ANALOG_IN_STATUS             = 0x0B,
  SET_ANALOG_IN_SAMPLING              = 0x0C,
  REPORT_ANALOG_IN_FORMAT             = 0x0C,

  UART_CONFIG                         = 0x0D,
  UART_STATUS                         = 0x0D,
  UART_DATA                           = 0x0E,
  SET_PIN_UART                        = 0x0F,
  UART_REPORT_TX_STATUS               = 0x0F,

  SPI_CONFIGURE_MASTER                = 0x10,
  SPI_STATUS                          = 0x10,
  SPI_MASTER_REQUEST                  = 0x11,
  SPI_DATA                            = 0x11,
  SET_PIN_SPI                         = 0x12,
  SPI_REPORT_TX_STATUS                = 0x12,

  I2C_CONFIGURE_MASTER                = 0x13,
  I2C_STATUS                          = 0x13,
  I2C_WRITE_READ                      = 0x14,
  I2C_RESULT                          = 0x14,
  I2C_REPORT_TX_STATUS                = 0x15,

  ICSP_SIX                            = 0x16,
  ICSP_REPORT_RX_STATUS               = 0x16,
  ICSP_REGOUT                         = 0x17,
  ICSP_RESULT                         = 0x17,
  ICSP_PROG_ENTER                     = 0x18,
  ICSP_PROG_EXIT                      = 0x19,
  ICSP_CONFIG                         = 0x1A,

  INCAP_CONFIG                        = 0x1B,
  INCAP_STATUS                        = 0x1B,
  SET_PIN_INCAP                       = 0x1C,
  INCAP_REPORT                        = 0x1C,

  SOFT_CLOSE                          = 0x1D,

  RGB_LED_MATRIX_ENABLE               = 0x1E,
  RGB_LED_MATRIX_FRAME                = 0x1F,

  RGB_LED_MATRIX_WRITE_FILE           = 0x20,

  // BOOKMARK(add_feature): Add new message type to enum.
  MESSAGE_TYPE_LIMIT
} MESSAGE_TYPE;


#endif  // __PROTOCOLDEFS_H__
