# 1 "protocol.c"
# 1 "/usr/local/google/home/dchristian/IOIO/ioio.git/firmware/app_layer_v1//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "protocol.c"
# 30 "protocol.c"
# 1 "protocol.h" 1
# 47 "protocol.h"
# 1 "../libconn/connection.h" 1
# 39 "../libconn/connection.h"
# 1 "../microchip/include/GenericTypeDefs.h" 1
# 63 "../microchip/include/GenericTypeDefs.h"
# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stddef.h" 1 3 4



typedef int ptrdiff_t;
typedef unsigned int size_t;
typedef short unsigned int wchar_t;






extern int errno;
# 64 "../microchip/include/GenericTypeDefs.h" 2

typedef enum _BOOL { FALSE = 0, TRUE } BOOL;
typedef enum _BIT { CLEAR = 0, SET } BIT;






typedef signed int INT;
typedef signed char INT8;
typedef signed short int INT16;
typedef signed long int INT32;



__extension__ typedef signed long long INT64;



typedef unsigned int UINT;
typedef unsigned char UINT8;
typedef unsigned short int UINT16;




typedef unsigned long int UINT32;


__extension__ typedef unsigned long long UINT64;


typedef union
{
    UINT8 Val;
    struct
    {
        __extension__ UINT8 b0:1;
        __extension__ UINT8 b1:1;
        __extension__ UINT8 b2:1;
        __extension__ UINT8 b3:1;
        __extension__ UINT8 b4:1;
        __extension__ UINT8 b5:1;
        __extension__ UINT8 b6:1;
        __extension__ UINT8 b7:1;
    } bits;
} UINT8_VAL, UINT8_BITS;

typedef union
{
    UINT16 Val;
    UINT8 v[2] ;
    struct
    {
        UINT8 LB;
        UINT8 HB;
    } byte;
    struct
    {
        __extension__ UINT8 b0:1;
        __extension__ UINT8 b1:1;
        __extension__ UINT8 b2:1;
        __extension__ UINT8 b3:1;
        __extension__ UINT8 b4:1;
        __extension__ UINT8 b5:1;
        __extension__ UINT8 b6:1;
        __extension__ UINT8 b7:1;
        __extension__ UINT8 b8:1;
        __extension__ UINT8 b9:1;
        __extension__ UINT8 b10:1;
        __extension__ UINT8 b11:1;
        __extension__ UINT8 b12:1;
        __extension__ UINT8 b13:1;
        __extension__ UINT8 b14:1;
        __extension__ UINT8 b15:1;
    } bits;
} UINT16_VAL, UINT16_BITS;
# 185 "../microchip/include/GenericTypeDefs.h"
typedef union
{
    UINT32 Val;
    UINT16 w[2] ;
    UINT8 v[4] ;
    struct
    {
        UINT16 LW;
        UINT16 HW;
    } word;
    struct
    {
        UINT8 LB;
        UINT8 HB;
        UINT8 UB;
        UINT8 MB;
    } byte;
    struct
    {
        UINT16_VAL low;
        UINT16_VAL high;
    }wordUnion;
    struct
    {
        __extension__ UINT8 b0:1;
        __extension__ UINT8 b1:1;
        __extension__ UINT8 b2:1;
        __extension__ UINT8 b3:1;
        __extension__ UINT8 b4:1;
        __extension__ UINT8 b5:1;
        __extension__ UINT8 b6:1;
        __extension__ UINT8 b7:1;
        __extension__ UINT8 b8:1;
        __extension__ UINT8 b9:1;
        __extension__ UINT8 b10:1;
        __extension__ UINT8 b11:1;
        __extension__ UINT8 b12:1;
        __extension__ UINT8 b13:1;
        __extension__ UINT8 b14:1;
        __extension__ UINT8 b15:1;
        __extension__ UINT8 b16:1;
        __extension__ UINT8 b17:1;
        __extension__ UINT8 b18:1;
        __extension__ UINT8 b19:1;
        __extension__ UINT8 b20:1;
        __extension__ UINT8 b21:1;
        __extension__ UINT8 b22:1;
        __extension__ UINT8 b23:1;
        __extension__ UINT8 b24:1;
        __extension__ UINT8 b25:1;
        __extension__ UINT8 b26:1;
        __extension__ UINT8 b27:1;
        __extension__ UINT8 b28:1;
        __extension__ UINT8 b29:1;
        __extension__ UINT8 b30:1;
        __extension__ UINT8 b31:1;
    } bits;
} UINT32_VAL;



typedef union
{
    UINT64 Val;
    UINT32 d[2] ;
    UINT16 w[4] ;
    UINT8 v[8] ;
    struct
    {
        UINT32 LD;
        UINT32 HD;
    } dword;
    struct
    {
        UINT16 LW;
        UINT16 HW;
        UINT16 UW;
        UINT16 MW;
    } word;
    struct
    {
        __extension__ UINT8 b0:1;
        __extension__ UINT8 b1:1;
        __extension__ UINT8 b2:1;
        __extension__ UINT8 b3:1;
        __extension__ UINT8 b4:1;
        __extension__ UINT8 b5:1;
        __extension__ UINT8 b6:1;
        __extension__ UINT8 b7:1;
        __extension__ UINT8 b8:1;
        __extension__ UINT8 b9:1;
        __extension__ UINT8 b10:1;
        __extension__ UINT8 b11:1;
        __extension__ UINT8 b12:1;
        __extension__ UINT8 b13:1;
        __extension__ UINT8 b14:1;
        __extension__ UINT8 b15:1;
        __extension__ UINT8 b16:1;
        __extension__ UINT8 b17:1;
        __extension__ UINT8 b18:1;
        __extension__ UINT8 b19:1;
        __extension__ UINT8 b20:1;
        __extension__ UINT8 b21:1;
        __extension__ UINT8 b22:1;
        __extension__ UINT8 b23:1;
        __extension__ UINT8 b24:1;
        __extension__ UINT8 b25:1;
        __extension__ UINT8 b26:1;
        __extension__ UINT8 b27:1;
        __extension__ UINT8 b28:1;
        __extension__ UINT8 b29:1;
        __extension__ UINT8 b30:1;
        __extension__ UINT8 b31:1;
        __extension__ UINT8 b32:1;
        __extension__ UINT8 b33:1;
        __extension__ UINT8 b34:1;
        __extension__ UINT8 b35:1;
        __extension__ UINT8 b36:1;
        __extension__ UINT8 b37:1;
        __extension__ UINT8 b38:1;
        __extension__ UINT8 b39:1;
        __extension__ UINT8 b40:1;
        __extension__ UINT8 b41:1;
        __extension__ UINT8 b42:1;
        __extension__ UINT8 b43:1;
        __extension__ UINT8 b44:1;
        __extension__ UINT8 b45:1;
        __extension__ UINT8 b46:1;
        __extension__ UINT8 b47:1;
        __extension__ UINT8 b48:1;
        __extension__ UINT8 b49:1;
        __extension__ UINT8 b50:1;
        __extension__ UINT8 b51:1;
        __extension__ UINT8 b52:1;
        __extension__ UINT8 b53:1;
        __extension__ UINT8 b54:1;
        __extension__ UINT8 b55:1;
        __extension__ UINT8 b56:1;
        __extension__ UINT8 b57:1;
        __extension__ UINT8 b58:1;
        __extension__ UINT8 b59:1;
        __extension__ UINT8 b60:1;
        __extension__ UINT8 b61:1;
        __extension__ UINT8 b62:1;
        __extension__ UINT8 b63:1;
    } bits;
} UINT64_VAL;





typedef void VOID;

typedef char CHAR8;
typedef unsigned char UCHAR8;

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned long DWORD;

__extension__
typedef unsigned long long QWORD;
typedef signed char CHAR;
typedef signed short int SHORT;
typedef signed long LONG;

__extension__
typedef signed long long LONGLONG;
typedef union
{
    BYTE Val;
    struct
    {
        __extension__ BYTE b0:1;
        __extension__ BYTE b1:1;
        __extension__ BYTE b2:1;
        __extension__ BYTE b3:1;
        __extension__ BYTE b4:1;
        __extension__ BYTE b5:1;
        __extension__ BYTE b6:1;
        __extension__ BYTE b7:1;
    } bits;
} BYTE_VAL, BYTE_BITS;

typedef union
{
    WORD Val;
    BYTE v[2] ;
    struct
    {
        BYTE LB;
        BYTE HB;
    } byte;
    struct
    {
        __extension__ BYTE b0:1;
        __extension__ BYTE b1:1;
        __extension__ BYTE b2:1;
        __extension__ BYTE b3:1;
        __extension__ BYTE b4:1;
        __extension__ BYTE b5:1;
        __extension__ BYTE b6:1;
        __extension__ BYTE b7:1;
        __extension__ BYTE b8:1;
        __extension__ BYTE b9:1;
        __extension__ BYTE b10:1;
        __extension__ BYTE b11:1;
        __extension__ BYTE b12:1;
        __extension__ BYTE b13:1;
        __extension__ BYTE b14:1;
        __extension__ BYTE b15:1;
    } bits;
} WORD_VAL, WORD_BITS;

typedef union
{
    DWORD Val;
    WORD w[2] ;
    BYTE v[4] ;
    struct
    {
        WORD LW;
        WORD HW;
    } word;
    struct
    {
        BYTE LB;
        BYTE HB;
        BYTE UB;
        BYTE MB;
    } byte;
    struct
    {
        WORD_VAL low;
        WORD_VAL high;
    }wordUnion;
    struct
    {
        __extension__ BYTE b0:1;
        __extension__ BYTE b1:1;
        __extension__ BYTE b2:1;
        __extension__ BYTE b3:1;
        __extension__ BYTE b4:1;
        __extension__ BYTE b5:1;
        __extension__ BYTE b6:1;
        __extension__ BYTE b7:1;
        __extension__ BYTE b8:1;
        __extension__ BYTE b9:1;
        __extension__ BYTE b10:1;
        __extension__ BYTE b11:1;
        __extension__ BYTE b12:1;
        __extension__ BYTE b13:1;
        __extension__ BYTE b14:1;
        __extension__ BYTE b15:1;
        __extension__ BYTE b16:1;
        __extension__ BYTE b17:1;
        __extension__ BYTE b18:1;
        __extension__ BYTE b19:1;
        __extension__ BYTE b20:1;
        __extension__ BYTE b21:1;
        __extension__ BYTE b22:1;
        __extension__ BYTE b23:1;
        __extension__ BYTE b24:1;
        __extension__ BYTE b25:1;
        __extension__ BYTE b26:1;
        __extension__ BYTE b27:1;
        __extension__ BYTE b28:1;
        __extension__ BYTE b29:1;
        __extension__ BYTE b30:1;
        __extension__ BYTE b31:1;
    } bits;
} DWORD_VAL;


typedef union
{
    QWORD Val;
    DWORD d[2] ;
    WORD w[4] ;
    BYTE v[8] ;
    struct
    {
        DWORD LD;
        DWORD HD;
    } dword;
    struct
    {
        WORD LW;
        WORD HW;
        WORD UW;
        WORD MW;
    } word;
    struct
    {
        __extension__ BYTE b0:1;
        __extension__ BYTE b1:1;
        __extension__ BYTE b2:1;
        __extension__ BYTE b3:1;
        __extension__ BYTE b4:1;
        __extension__ BYTE b5:1;
        __extension__ BYTE b6:1;
        __extension__ BYTE b7:1;
        __extension__ BYTE b8:1;
        __extension__ BYTE b9:1;
        __extension__ BYTE b10:1;
        __extension__ BYTE b11:1;
        __extension__ BYTE b12:1;
        __extension__ BYTE b13:1;
        __extension__ BYTE b14:1;
        __extension__ BYTE b15:1;
        __extension__ BYTE b16:1;
        __extension__ BYTE b17:1;
        __extension__ BYTE b18:1;
        __extension__ BYTE b19:1;
        __extension__ BYTE b20:1;
        __extension__ BYTE b21:1;
        __extension__ BYTE b22:1;
        __extension__ BYTE b23:1;
        __extension__ BYTE b24:1;
        __extension__ BYTE b25:1;
        __extension__ BYTE b26:1;
        __extension__ BYTE b27:1;
        __extension__ BYTE b28:1;
        __extension__ BYTE b29:1;
        __extension__ BYTE b30:1;
        __extension__ BYTE b31:1;
        __extension__ BYTE b32:1;
        __extension__ BYTE b33:1;
        __extension__ BYTE b34:1;
        __extension__ BYTE b35:1;
        __extension__ BYTE b36:1;
        __extension__ BYTE b37:1;
        __extension__ BYTE b38:1;
        __extension__ BYTE b39:1;
        __extension__ BYTE b40:1;
        __extension__ BYTE b41:1;
        __extension__ BYTE b42:1;
        __extension__ BYTE b43:1;
        __extension__ BYTE b44:1;
        __extension__ BYTE b45:1;
        __extension__ BYTE b46:1;
        __extension__ BYTE b47:1;
        __extension__ BYTE b48:1;
        __extension__ BYTE b49:1;
        __extension__ BYTE b50:1;
        __extension__ BYTE b51:1;
        __extension__ BYTE b52:1;
        __extension__ BYTE b53:1;
        __extension__ BYTE b54:1;
        __extension__ BYTE b55:1;
        __extension__ BYTE b56:1;
        __extension__ BYTE b57:1;
        __extension__ BYTE b58:1;
        __extension__ BYTE b59:1;
        __extension__ BYTE b60:1;
        __extension__ BYTE b61:1;
        __extension__ BYTE b62:1;
        __extension__ BYTE b63:1;
    } bits;
} QWORD_VAL;
# 40 "../libconn/connection.h" 2

typedef int CHANNEL_HANDLE;



typedef enum {
  CHANNEL_TYPE_ADB,
  CHANNEL_TYPE_ACC,
  CHANNEL_TYPE_BT
} CHANNEL_TYPE;







typedef void (*ChannelCallback) (CHANNEL_HANDLE ch, const void* data,
                                 UINT32 size);


void ConnectionInit();




BOOL ConnectionTasks();


void ConnectionResetUSB();

BOOL ConnectionTypeSupported(CHANNEL_TYPE con);
BOOL ConnectionCanOpenChannel(CHANNEL_TYPE con);
CHANNEL_HANDLE ConnectionOpenChannelAdb(const char *name, ChannelCallback cb);
CHANNEL_HANDLE ConnectionOpenChannelBtServer(ChannelCallback cb);
CHANNEL_HANDLE ConnectionOpenChannelAccessory(ChannelCallback cb);
void ConnectionSend(CHANNEL_HANDLE ch, const void *data, int size);
BOOL ConnectionCanSend(CHANNEL_HANDLE ch);
void ConnectionCloseChannel(CHANNEL_HANDLE ch);
int ConnectionGetMaxPacket(CHANNEL_HANDLE ch);
# 48 "protocol.h" 2
# 1 "protocol_defs.h" 1
# 43 "protocol_defs.h"
typedef struct __attribute__ ((packed)) {
  DWORD magic;
} HARD_RESET_ARGS;


typedef struct __attribute__ ((packed)) {
  DWORD magic;
  BYTE hw_impl_ver[8];
  BYTE bl_impl_ver[8];
  BYTE fw_impl_ver[8];
} ESTABLISH_CONNECTION_ARGS;


typedef struct __attribute__ ((packed)) {
} SOFT_RESET_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE open_drain : 1;
  BYTE value : 1;
  BYTE pin : 6;
} SET_PIN_DIGITAL_OUT_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE value : 1;
  BYTE : 1;
  BYTE pin : 6;
} SET_DIGITAL_OUT_LEVEL_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE level : 1;
  BYTE : 1;
  BYTE pin : 6;
} REPORT_DIGITAL_IN_STATUS_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE pull : 2;
  BYTE pin : 6;
} SET_PIN_DIGITAL_IN_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE cn : 1;
  BYTE : 1;
  BYTE pin : 6;
} SET_CHANGE_NOTIFY_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE pin : 6;
  BYTE : 2;
  BYTE freq_scale;
} REGISTER_PERIODIC_DIGITAL_SAMPLING_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE size;
} REPORT_PERIODIC_DIGITAL_IN_STATUS_ARGS;


typedef struct __attribute__ ((packed)) {

} RESERVED_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE pin : 6;
  BYTE : 2;
  BYTE pwm_num : 4;
  BYTE : 3;
  BYTE enable : 1;
} SET_PIN_PWM_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE fraction : 2;
  BYTE pwm_num : 4;
  BYTE : 2;
  WORD dc;
} SET_PWM_DUTY_CYCLE_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE scale_l : 1;
  BYTE pwm_num : 4;
  BYTE : 2;
  BYTE scale_h : 1;
  WORD period;
} SET_PWM_PERIOD_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE uart_num : 2;
  WORD bytes_to_add : 14;
} UART_REPORT_TX_STATUS_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE pin;
} SET_PIN_ANALOG_IN_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE num_pins;
} REPORT_ANALOG_IN_FORMAT_ARGS;


typedef struct __attribute__ ((packed)) {
} REPORT_ANALOG_IN_STATUS_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE size : 6;
  BYTE uart_num : 2;
  BYTE data[0];
} UART_DATA_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE parity : 2;
  BYTE two_stop_bits : 1;
  BYTE speed4x : 1;
  BYTE : 2;
  BYTE uart_num : 2;
  WORD rate;
} UART_CONFIG_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE uart_num : 2;
  BYTE : 5;
  BYTE enabled : 1;
} UART_STATUS_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE pin : 6;
  BYTE : 2;
  BYTE uart_num : 2;
  BYTE : 4;
  BYTE dir : 1;
  BYTE enable : 1;
} SET_PIN_UART_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE spi_num : 2;
  WORD bytes_to_add : 14;
} SPI_REPORT_TX_STATUS_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE size : 6;
  BYTE spi_num : 2;
  BYTE ss_pin : 6;
  BYTE : 2;
  BYTE data[0];
} SPI_DATA_ARGS;


typedef struct __attribute__ ((packed)) {
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


typedef struct __attribute__ ((packed)) {
  BYTE div : 3;
  BYTE scale : 2;
  BYTE spi_num : 2;
  BYTE : 1;
  BYTE clk_pol : 1;
  BYTE clk_edge : 1;
  BYTE smp_end : 1;
  BYTE : 5;
} SPI_CONFIGURE_MASTER_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE spi_num : 2;
  BYTE : 5;
  BYTE enabled : 1;
} SPI_STATUS_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE pin : 6;
  BYTE : 2;
  BYTE spi_num : 2;
  BYTE mode : 2;
  BYTE enable : 1;
  BYTE : 3;
} SET_PIN_SPI_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE i2c_num : 2;
  BYTE : 3;
  BYTE rate : 2;
  BYTE smbus_levels : 1;
} I2C_CONFIGURE_MASTER_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE i2c_num : 2;
  BYTE : 5;
  BYTE enabled : 1;
} I2C_STATUS_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE i2c_num : 2;
  BYTE : 3;
  BYTE ten_bit_addr : 1;
  BYTE addr_msb : 2;
  BYTE addr_lsb;
  BYTE write_size;
  BYTE read_size;
  BYTE data[0];
} I2C_WRITE_READ_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE i2c_num : 2;
  BYTE : 6;
  BYTE size;
} I2C_RESULT_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE i2c_num : 2;
  WORD bytes_to_add : 14;
} I2C_REPORT_TX_STATUS_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE pin : 6;
  BYTE : 1;
  BYTE enable : 1;
} SET_ANALOG_IN_SAMPLING_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE interface_id[8];
} CHECK_INTERFACE_ARGS;


typedef struct __attribute__ ((packed)) {
  BYTE supported : 1;
  BYTE : 7;
} CHECK_INTERFACE_RESPONSE_ARGS;


typedef struct __attribute__ ((packed)) {
    DWORD inst : 24;
} ICSP_SIX_ARGS;


typedef struct __attribute__ ((packed)) {
    WORD bytes_to_add;
} ICSP_REPORT_RX_STATUS_ARGS;


typedef struct __attribute__ ((packed)) {
} ICSP_REGOUT_ARGS;


typedef struct __attribute__ ((packed)) {
    WORD reg;
} ICSP_RESULT_ARGS;


typedef struct __attribute__ ((packed)) {
} ICSP_PROG_ENTER_ARGS;


typedef struct __attribute__ ((packed)) {
} ICSP_PROG_EXIT_ARGS;


typedef struct __attribute__ ((packed)) {
    BYTE enable : 1;
} ICSP_CONFIG_ARGS;


typedef struct __attribute__ ((packed)) {
    BYTE incap_num : 4;
    BYTE : 4;
    BYTE clock : 2;
    BYTE : 1;
    BYTE mode : 3;
    BYTE : 1;
    BYTE double_prec : 1;
} INCAP_CONFIG_ARGS;


typedef struct __attribute__ ((packed)) {
    BYTE incap_num : 4;
    BYTE : 3;
    BYTE enabled : 1;
} INCAP_STATUS_ARGS;


typedef struct __attribute__ ((packed)) {
    BYTE pin : 6;
    BYTE : 2;
    BYTE incap_num : 4;
    BYTE : 3;
    BYTE enable : 1;
} SET_PIN_INCAP_ARGS;


typedef struct __attribute__ ((packed)) {
    BYTE incap_num : 4;
    BYTE : 2;
    BYTE size : 2;
} INCAP_REPORT_ARGS;


typedef struct __attribute__ ((packed)) {
} SOFT_CLOSE_ARGS;





typedef struct __attribute__ ((packed)) {
  BYTE type;
  union __attribute__ ((packed)) {
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
    SET_PIN_ANALOG_IN_ARGS set_pin_analog_in;
    UART_DATA_ARGS uart_data;
    UART_CONFIG_ARGS uart_config;
    SET_PIN_UART_ARGS set_pin_uart;
    SPI_MASTER_REQUEST_ARGS spi_master_request;
    SPI_CONFIGURE_MASTER_ARGS spi_configure_master;
    SET_PIN_SPI_ARGS set_pin_spi;
    I2C_CONFIGURE_MASTER_ARGS i2c_configure_master;
    I2C_WRITE_READ_ARGS i2c_write_read;
    SET_ANALOG_IN_SAMPLING_ARGS set_analog_pin_sampling;
    CHECK_INTERFACE_ARGS check_interface;
    ICSP_SIX_ARGS icsp_six;
    ICSP_REGOUT_ARGS icsp_regout;
    ICSP_PROG_ENTER_ARGS icsp_prog_enter;
    ICSP_PROG_EXIT_ARGS icsp_prog_exit;
    ICSP_CONFIG_ARGS icsp_config;
    INCAP_CONFIG_ARGS incap_config;
    SET_PIN_INCAP_ARGS set_pin_incap;
    SOFT_CLOSE_ARGS soft_close;

  } args;
  BYTE __vabuf[64];
} INCOMING_MESSAGE;

typedef struct __attribute__ ((packed)) {
  BYTE type;
  union __attribute__ ((packed)) {
    ESTABLISH_CONNECTION_ARGS establish_connection;
    REPORT_DIGITAL_IN_STATUS_ARGS report_digital_in_status;
    REPORT_PERIODIC_DIGITAL_IN_STATUS_ARGS report_periodic_digital_in_status;
    REPORT_ANALOG_IN_FORMAT_ARGS report_analog_in_format;
    REPORT_ANALOG_IN_STATUS_ARGS report_analog_in_status;
    UART_REPORT_TX_STATUS_ARGS uart_report_tx_status;
    UART_DATA_ARGS uart_data;
    SPI_REPORT_TX_STATUS_ARGS spi_report_tx_status;
    SPI_DATA_ARGS spi_data;
    I2C_RESULT_ARGS i2c_result;
    I2C_REPORT_TX_STATUS_ARGS i2c_report_tx_status;
    CHECK_INTERFACE_RESPONSE_ARGS check_interface_response;
    UART_STATUS_ARGS uart_status;
    SPI_STATUS_ARGS spi_status;
    I2C_STATUS_ARGS i2c_status;
    ICSP_RESULT_ARGS icsp_result;
    ICSP_REPORT_RX_STATUS_ARGS icsp_report_rx_status;
    INCAP_STATUS_ARGS incap_status;
    INCAP_REPORT_ARGS incap_report;
    SOFT_CLOSE_ARGS soft_close;

  } args;
} OUTGOING_MESSAGE;


typedef enum {
  HARD_RESET = 0x00,
  ESTABLISH_CONNECTION = 0x00,
  SOFT_RESET = 0x01,
  CHECK_INTERFACE = 0x02,
  CHECK_INTERFACE_RESPONSE = 0x02,

  SET_PIN_DIGITAL_OUT = 0x03,
  SET_DIGITAL_OUT_LEVEL = 0x04,
  REPORT_DIGITAL_IN_STATUS = 0x04,
  SET_PIN_DIGITAL_IN = 0x05,
  REPORT_PERIODIC_DIGITAL_IN_STATUS = 0x05,
  SET_CHANGE_NOTIFY = 0x06,
  REGISTER_PERIODIC_DIGITAL_SAMPLING = 0x07,

  SET_PIN_PWM = 0x08,
  SET_PWM_DUTY_CYCLE = 0x09,
  SET_PWM_PERIOD = 0x0A,

  SET_PIN_ANALOG_IN = 0x0B,
  REPORT_ANALOG_IN_STATUS = 0x0B,
  SET_ANALOG_IN_SAMPLING = 0x0C,
  REPORT_ANALOG_IN_FORMAT = 0x0C,

  UART_CONFIG = 0x0D,
  UART_STATUS = 0x0D,
  UART_DATA = 0x0E,
  SET_PIN_UART = 0x0F,
  UART_REPORT_TX_STATUS = 0x0F,

  SPI_CONFIGURE_MASTER = 0x10,
  SPI_STATUS = 0x10,
  SPI_MASTER_REQUEST = 0x11,
  SPI_DATA = 0x11,
  SET_PIN_SPI = 0x12,
  SPI_REPORT_TX_STATUS = 0x12,

  I2C_CONFIGURE_MASTER = 0x13,
  I2C_STATUS = 0x13,
  I2C_WRITE_READ = 0x14,
  I2C_RESULT = 0x14,
  I2C_REPORT_TX_STATUS = 0x15,

  ICSP_SIX = 0x16,
  ICSP_REPORT_RX_STATUS = 0x16,
  ICSP_REGOUT = 0x17,
  ICSP_RESULT = 0x17,
  ICSP_PROG_ENTER = 0x18,
  ICSP_PROG_EXIT = 0x19,
  ICSP_CONFIG = 0x1A,

  INCAP_CONFIG = 0x1B,
  INCAP_STATUS = 0x1B,
  SET_PIN_INCAP = 0x1C,
  INCAP_REPORT = 0x1C,

  SOFT_CLOSE = 0x1D,


  MESSAGE_TYPE_LIMIT
} MESSAGE_TYPE;
# 49 "protocol.h" 2
# 58 "protocol.h"
void AppProtocolInit(CHANNEL_HANDLE h);




void AppProtocolTasks(CHANNEL_HANDLE h);



BOOL AppProtocolHandleIncoming(const BYTE* data, UINT32 data_len);






void AppProtocolSendMessage(const OUTGOING_MESSAGE* msg);





void AppProtocolSendMessageWithVarArg(const OUTGOING_MESSAGE* msg,
                                      const void* data, int size);



void AppProtocolSendMessageWithVarArgSplit(const OUTGOING_MESSAGE* msg,
                                          const void* data1, int size1,
                                          const void* data2, int size2);
# 31 "protocol.c" 2

# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/assert.h" 1 3 4
# 33 "protocol.c" 2
# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/string.h" 1 3 4
# 10 "/opt/microchip/xc16/v1.10/bin/bin/../../include/string.h" 3 4
extern void * memcpy(void *, const void *, size_t);
extern void * memmove(void *, const void *, size_t);
extern void * memset(void *, int, size_t);

extern char * strcat(char *, const char *);
extern char * strcpy(char *, const char *);
extern char * strncat(char *, const char *, size_t);
extern char * strncpy(char *, const char *, size_t);
extern char * strdup(const char *);
extern char * strtok(char *, const char *);

extern int memcmp(const void *, const void *, size_t);
extern int strcmp(const char *, const char *);
extern int strncmp(const char *, const char *, size_t);
extern void * memchr(const void *, int, size_t);
extern size_t strcspn(const char *, const char *);
extern char * strpbrk(const char *, const char *);
extern size_t strspn(const char *, const char *);
extern char * strstr(const char *, const char *);
extern char * strerror(int);
extern size_t strlen(const char *);
extern char * strchr(const char *, int);
extern char * strrchr(const char *, int);
# 34 "protocol.c" 2
# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 1 3 4
# 23 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
extern int __attach_input_file(const char *f);
void __close_input_file(void);
# 34 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
extern int __C30_UART;
# 44 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
extern void __delay32(unsigned long cycles);
# 58 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
extern void __delay_ms(unsigned long);
extern void __delay_us(unsigned long);
# 82 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
extern __attribute__((space(prog))) int _PROGRAM_END;
# 95 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
extern void _dump_heap_info(void);
# 114 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
typedef unsigned long _prog_addressT;

extern _prog_addressT _memcpy_helper(_prog_addressT src, void *dst,
                                     unsigned int len, int flags);
# 127 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
extern void _memcpy_df(__pack_upper_byte void *src,void *dst, unsigned int len);
# 146 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
_prog_addressT _memcpy_p2d16(void *dest, _prog_addressT src,
                             unsigned int len);
# 158 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
_prog_addressT _memcpy_p2d24(void *dest, _prog_addressT src,
                             unsigned int len);
# 171 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
_prog_addressT _strncpy_p2d16(char *dest, _prog_addressT src,
                              unsigned int len);
# 184 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
_prog_addressT _strncpy_p2d24(char *dest, _prog_addressT src,
                              unsigned int len);







void _memcpy_packed(char *dest, __pack_upper_byte void *src, unsigned int len);
# 301 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
extern void _eedata_helper1(_prog_addressT dst, int len);
extern void _eedata_helper2(void);
extern void _eedata_helper3(_prog_addressT dst, int dat);
extern void _eedata_helper4(_prog_addressT dst, int *src);
extern void _eedata_helper5(_prog_addressT dst, int len);
extern void _eedata_helper6(void);
extern void _eedata_helper7(_prog_addressT dst, int dat);
extern void _flash_helper1 (_prog_addressT dst, int code);
extern void _flash_helper2 (_prog_addressT dst, int *src, int len);
extern void _flash_helper3 (_prog_addressT dst, long *src, int len);
extern void _flash_helper4 (_prog_addressT dst, int dat);
extern void _flash_helper5 (_prog_addressT dst, long dat);
extern void _flash_helper6 (int code);
extern void _flash_helper7(_prog_addressT dst, int len);
extern void _flash_helper8 (_prog_addressT dst, int *src, int len);
extern void _flash_helper9 (_prog_addressT dst, long *src, int len);
extern void _flash_helper10 (_prog_addressT dst, int dat1, int empty, int dat2);
extern void _flash_helper11 (_prog_addressT dst, long dat1, long dat2);
# 441 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
void _erase_flash(_prog_addressT dst);
# 457 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
void _write_flash16(_prog_addressT dst, int *src);
# 473 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
void _write_flash24(_prog_addressT dst, long *src);
# 493 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
void _write_flash_word16(_prog_addressT dst, int dat);
# 511 "/opt/microchip/xc16/v1.10/bin/bin/../../support/generic/h/libpic30.h" 3 4
void _write_flash_word24(_prog_addressT dst, long dat);
# 35 "protocol.c" 2
# 1 "../blapi/version.h" 1
# 41 "../blapi/version.h"
extern const char __attribute__((space(psv))) bootloader_version[8];
extern const char __attribute__((space(psv))) hardware_version[8];
# 36 "protocol.c" 2
# 1 "byte_queue.h" 1
# 36 "byte_queue.h"
# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../support/PIC24F/h/p24Fxxxx.h" 1 3 4
# 258 "/opt/microchip/xc16/v1.10/bin/bin/../../support/PIC24F/h/p24Fxxxx.h" 3 4
# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../support/PIC24F/h/p24FJ256DA206.h" 1 3 4
# 14 "/opt/microchip/xc16/v1.10/bin/bin/../../support/PIC24F/h/p24FJ256DA206.h" 3 4
extern volatile unsigned int WREG0 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG1 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG2 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG3 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG4 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG5 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG6 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG7 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG8 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG9 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG10 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG11 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG12 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG13 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG14 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG15 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int SPLIM __attribute__((__sfr__));
extern volatile unsigned int PCL __attribute__((__sfr__));
extern volatile unsigned char PCH __attribute__((__sfr__));
extern volatile unsigned int DSRPAG __attribute__((__sfr__));
extern volatile unsigned int DSWPAG __attribute__((__sfr__));
extern volatile unsigned int RCOUNT __attribute__((__sfr__));
extern volatile unsigned int SR __attribute__((__sfr__));
__extension__ typedef struct tagSRBITS {
  union {
    struct {
      unsigned C:1;
      unsigned Z:1;
      unsigned OV:1;
      unsigned N:1;
      unsigned RA:1;
      unsigned IPL:3;
      unsigned DC:1;
    };
    struct {
      unsigned :5;
      unsigned IPL0:1;
      unsigned IPL1:1;
      unsigned IPL2:1;
    };
  };
} SRBITS;
extern volatile SRBITS SRbits __attribute__((__sfr__));

extern volatile unsigned int CORCON __attribute__((__sfr__));
typedef struct tagCORCONBITS {
  unsigned :3;
  unsigned IPL3:1;
} CORCONBITS;
extern volatile CORCONBITS CORCONbits __attribute__((__sfr__));

extern volatile unsigned int DISICNT __attribute__((__sfr__));
extern volatile unsigned int TBLPAG __attribute__((__sfr__));
extern volatile unsigned int CNPD1 __attribute__((__sfr__));
typedef struct tagCNPD1BITS {
  unsigned CN0PDE:1;
  unsigned CN1PDE:1;
  unsigned CN2PDE:1;
  unsigned CN3PDE:1;
  unsigned CN4PDE:1;
  unsigned CN5PDE:1;
  unsigned CN6PDE:1;
  unsigned CN7PDE:1;
  unsigned CN8PDE:1;
  unsigned CN9PDE:1;
  unsigned CN10PDE:1;
  unsigned CN11PDE:1;
  unsigned CN12PDE:1;
  unsigned CN13PDE:1;
  unsigned CN14PDE:1;
  unsigned CN15PDE:1;
} CNPD1BITS;
extern volatile CNPD1BITS CNPD1bits __attribute__((__sfr__));

extern volatile unsigned int CNPD2 __attribute__((__sfr__));
typedef struct tagCNPD2BITS {
  unsigned CN16PDE:1;
  unsigned CN17PDE:1;
  unsigned CN18PDE:1;
  unsigned :3;
  unsigned CN22PDE:1;
  unsigned CN23PDE:1;
  unsigned CN24PDE:1;
  unsigned CN25PDE:1;
  unsigned CN26PDE:1;
  unsigned CN27PDE:1;
  unsigned CN28PDE:1;
  unsigned CN29PDE:1;
  unsigned CN30PDE:1;
  unsigned CN31PDE:1;
} CNPD2BITS;
extern volatile CNPD2BITS CNPD2bits __attribute__((__sfr__));

extern volatile unsigned int CNPD3 __attribute__((__sfr__));
typedef struct tagCNPD3BITS {
  unsigned CN32PDE:1;
} CNPD3BITS;
extern volatile CNPD3BITS CNPD3bits __attribute__((__sfr__));

extern volatile unsigned int CNPD4 __attribute__((__sfr__));
typedef struct tagCNPD4BITS {
  unsigned :1;
  unsigned CN49PDE:1;
  unsigned CN50PDE:1;
  unsigned CN51PDE:1;
  unsigned CN52PDE:1;
  unsigned CN53PDE:1;
  unsigned CN54PDE:1;
  unsigned CN55PDE:1;
  unsigned CN56PDE:1;
  unsigned :1;
  unsigned CN58PDE:1;
  unsigned CN59PDE:1;
  unsigned CN60PDE:1;
  unsigned CN61PDE:1;
  unsigned CN62PDE:1;
  unsigned CN63PDE:1;
} CNPD4BITS;
extern volatile CNPD4BITS CNPD4bits __attribute__((__sfr__));

extern volatile unsigned int CNPD5 __attribute__((__sfr__));
typedef struct tagCNPD5BITS {
  unsigned CN64PDE:1;
  unsigned CN65PDE:1;
  unsigned :2;
  unsigned CN68PDE:1;
  unsigned CN69PDE:1;
  unsigned :1;
  unsigned CN71PDE:1;
} CNPD5BITS;
extern volatile CNPD5BITS CNPD5bits __attribute__((__sfr__));

extern volatile unsigned int CNPD6 __attribute__((__sfr__));
typedef struct tagCNPD6BITS {
  unsigned :3;
  unsigned CN83PDE:1;
  unsigned CN84PDE:1;
} CNPD6BITS;
extern volatile CNPD6BITS CNPD6bits __attribute__((__sfr__));

extern volatile unsigned int CNEN1 __attribute__((__sfr__));
typedef struct tagCNEN1BITS {
  unsigned CN0IE:1;
  unsigned CN1IE:1;
  unsigned CN2IE:1;
  unsigned CN3IE:1;
  unsigned CN4IE:1;
  unsigned CN5IE:1;
  unsigned CN6IE:1;
  unsigned CN7IE:1;
  unsigned CN8IE:1;
  unsigned CN9IE:1;
  unsigned CN10IE:1;
  unsigned CN11IE:1;
  unsigned CN12IE:1;
  unsigned CN13IE:1;
  unsigned CN14IE:1;
  unsigned CN15IE:1;
} CNEN1BITS;
extern volatile CNEN1BITS CNEN1bits __attribute__((__sfr__));

extern volatile unsigned int CNEN2 __attribute__((__sfr__));
typedef struct tagCNEN2BITS {
  unsigned CN16IE:1;
  unsigned CN17IE:1;
  unsigned CN18IE:1;
  unsigned :3;
  unsigned CN22IE:1;
  unsigned CN23IE:1;
  unsigned CN24IE:1;
  unsigned CN25IE:1;
  unsigned CN26IE:1;
  unsigned CN27IE:1;
  unsigned CN28IE:1;
  unsigned CN29IE:1;
  unsigned CN30IE:1;
  unsigned CN31IE:1;
} CNEN2BITS;
extern volatile CNEN2BITS CNEN2bits __attribute__((__sfr__));

extern volatile unsigned int CNEN3 __attribute__((__sfr__));
typedef struct tagCNEN3BITS {
  unsigned CN32IE:1;
} CNEN3BITS;
extern volatile CNEN3BITS CNEN3bits __attribute__((__sfr__));

extern volatile unsigned int CNEN4 __attribute__((__sfr__));
typedef struct tagCNEN4BITS {
  unsigned :1;
  unsigned CN49IE:1;
  unsigned CN50IE:1;
  unsigned CN51IE:1;
  unsigned CN52IE:1;
  unsigned CN53IE:1;
  unsigned CN54IE:1;
  unsigned CN55IE:1;
  unsigned CN56IE:1;
  unsigned :1;
  unsigned CN58IE:1;
  unsigned CN59IE:1;
  unsigned CN60IE:1;
  unsigned CN61IE:1;
  unsigned CN62IE:1;
  unsigned CN63IE:1;
} CNEN4BITS;
extern volatile CNEN4BITS CNEN4bits __attribute__((__sfr__));

extern volatile unsigned int CNEN5 __attribute__((__sfr__));
typedef struct tagCNEN5BITS {
  unsigned CN64IE:1;
  unsigned CN65IE:1;
  unsigned :2;
  unsigned CN68IE:1;
  unsigned CN69IE:1;
  unsigned :1;
  unsigned CN71IE:1;
} CNEN5BITS;
extern volatile CNEN5BITS CNEN5bits __attribute__((__sfr__));

extern volatile unsigned int CNEN6 __attribute__((__sfr__));
typedef struct tagCNEN6BITS {
  unsigned :3;
  unsigned CN83IE:1;
  unsigned CN84IE:1;
} CNEN6BITS;
extern volatile CNEN6BITS CNEN6bits __attribute__((__sfr__));

extern volatile unsigned int CNPU1 __attribute__((__sfr__));
typedef struct tagCNPU1BITS {
  unsigned CN0PUE:1;
  unsigned CN1PUE:1;
  unsigned CN2PUE:1;
  unsigned CN3PUE:1;
  unsigned CN4PUE:1;
  unsigned CN5PUE:1;
  unsigned CN6PUE:1;
  unsigned CN7PUE:1;
  unsigned CN8PUE:1;
  unsigned CN9PUE:1;
  unsigned CN10PUE:1;
  unsigned CN11PUE:1;
  unsigned CN12PUE:1;
  unsigned CN13PUE:1;
  unsigned CN14PUE:1;
  unsigned CN15PUE:1;
} CNPU1BITS;
extern volatile CNPU1BITS CNPU1bits __attribute__((__sfr__));

extern volatile unsigned int CNPU2 __attribute__((__sfr__));
typedef struct tagCNPU2BITS {
  unsigned CN16PUE:1;
  unsigned CN17PUE:1;
  unsigned CN18PUE:1;
  unsigned :3;
  unsigned CN22PUE:1;
  unsigned CN23PUE:1;
  unsigned CN24PUE:1;
  unsigned CN25PUE:1;
  unsigned CN26PUE:1;
  unsigned CN27PUE:1;
  unsigned CN28PUE:1;
  unsigned CN29PUE:1;
  unsigned CN30PUE:1;
  unsigned CN31PUE:1;
} CNPU2BITS;
extern volatile CNPU2BITS CNPU2bits __attribute__((__sfr__));

extern volatile unsigned int CNPU3 __attribute__((__sfr__));
typedef struct tagCNPU3BITS {
  unsigned CN32PUE:1;
} CNPU3BITS;
extern volatile CNPU3BITS CNPU3bits __attribute__((__sfr__));

extern volatile unsigned int CNPU4 __attribute__((__sfr__));
typedef struct tagCNPU4BITS {
  unsigned :1;
  unsigned CN49PUE:1;
  unsigned CN50PUE:1;
  unsigned CN51PUE:1;
  unsigned CN52PUE:1;
  unsigned CN53PUE:1;
  unsigned CN54PUE:1;
  unsigned CN55PUE:1;
  unsigned CN56PUE:1;
  unsigned :1;
  unsigned CN58PUE:1;
  unsigned CN59PUE:1;
  unsigned CN60PUE:1;
  unsigned CN61PUE:1;
  unsigned CN62PUE:1;
  unsigned CN63PUE:1;
} CNPU4BITS;
extern volatile CNPU4BITS CNPU4bits __attribute__((__sfr__));

extern volatile unsigned int CNPU5 __attribute__((__sfr__));
typedef struct tagCNPU5BITS {
  unsigned CN64PUE:1;
  unsigned CN65PUE:1;
  unsigned :2;
  unsigned CN68PUE:1;
  unsigned CN69PUE:1;
  unsigned :1;
  unsigned CN71PUE:1;
} CNPU5BITS;
extern volatile CNPU5BITS CNPU5bits __attribute__((__sfr__));

extern volatile unsigned int CNPU6 __attribute__((__sfr__));
typedef struct tagCNPU6BITS {
  unsigned :3;
  unsigned CN83PUE:1;
  unsigned CN84PUE:1;
} CNPU6BITS;
extern volatile CNPU6BITS CNPU6bits __attribute__((__sfr__));

extern volatile unsigned int INTCON1 __attribute__((__sfr__));
typedef struct tagINTCON1BITS {
  unsigned :1;
  unsigned OSCFAIL:1;
  unsigned STKERR:1;
  unsigned ADDRERR:1;
  unsigned MATHERR:1;
  unsigned :10;
  unsigned NSTDIS:1;
} INTCON1BITS;
extern volatile INTCON1BITS INTCON1bits __attribute__((__sfr__));

extern volatile unsigned int INTCON2 __attribute__((__sfr__));
typedef struct tagINTCON2BITS {
  unsigned INT0EP:1;
  unsigned INT1EP:1;
  unsigned INT2EP:1;
  unsigned INT3EP:1;
  unsigned INT4EP:1;
  unsigned :9;
  unsigned DISI:1;
  unsigned ALTIVT:1;
} INTCON2BITS;
extern volatile INTCON2BITS INTCON2bits __attribute__((__sfr__));

extern volatile unsigned int IFS0 __attribute__((__sfr__));
typedef struct tagIFS0BITS {
  unsigned INT0IF:1;
  unsigned IC1IF:1;
  unsigned OC1IF:1;
  unsigned T1IF:1;
  unsigned :1;
  unsigned IC2IF:1;
  unsigned OC2IF:1;
  unsigned T2IF:1;
  unsigned T3IF:1;
  unsigned SPF1IF:1;
  unsigned SPI1IF:1;
  unsigned U1RXIF:1;
  unsigned U1TXIF:1;
  unsigned AD1IF:1;
} IFS0BITS;
extern volatile IFS0BITS IFS0bits __attribute__((__sfr__));

extern volatile unsigned int IFS1 __attribute__((__sfr__));
typedef struct tagIFS1BITS {
  unsigned SI2C1IF:1;
  unsigned MI2C1IF:1;
  unsigned CMIF:1;
  unsigned CNIF:1;
  unsigned INT1IF:1;
  unsigned :1;
  unsigned IC7IF:1;
  unsigned IC8IF:1;
  unsigned :1;
  unsigned OC3IF:1;
  unsigned OC4IF:1;
  unsigned T4IF:1;
  unsigned T5IF:1;
  unsigned INT2IF:1;
  unsigned U2RXIF:1;
  unsigned U2TXIF:1;
} IFS1BITS;
extern volatile IFS1BITS IFS1bits __attribute__((__sfr__));

extern volatile unsigned int IFS2 __attribute__((__sfr__));
typedef struct tagIFS2BITS {
  unsigned SPF2IF:1;
  unsigned SPI2IF:1;
  unsigned :3;
  unsigned IC3IF:1;
  unsigned IC4IF:1;
  unsigned IC5IF:1;
  unsigned IC6IF:1;
  unsigned OC5IF:1;
  unsigned OC6IF:1;
  unsigned OC7IF:1;
  unsigned OC8IF:1;
} IFS2BITS;
extern volatile IFS2BITS IFS2bits __attribute__((__sfr__));

extern volatile unsigned int IFS3 __attribute__((__sfr__));
typedef struct tagIFS3BITS {
  unsigned :1;
  unsigned SI2C2IF:1;
  unsigned MI2C2IF:1;
  unsigned :2;
  unsigned INT3IF:1;
  unsigned INT4IF:1;
  unsigned :7;
  unsigned RTCIF:1;
} IFS3BITS;
extern volatile IFS3BITS IFS3bits __attribute__((__sfr__));

extern volatile unsigned int IFS4 __attribute__((__sfr__));
typedef struct tagIFS4BITS {
  unsigned :1;
  unsigned U1ERIF:1;
  unsigned U2ERIF:1;
  unsigned CRCIF:1;
  unsigned :4;
  unsigned LVDIF:1;
  unsigned :4;
  unsigned CTMUIF:1;
} IFS4BITS;
extern volatile IFS4BITS IFS4bits __attribute__((__sfr__));

extern volatile unsigned int IFS5 __attribute__((__sfr__));
typedef struct tagIFS5BITS {
  unsigned :1;
  unsigned U3ERIF:1;
  unsigned U3RXIF:1;
  unsigned U3TXIF:1;
  unsigned SI2C3IF:1;
  unsigned MI2C3IF:1;
  unsigned USB1IF:1;
  unsigned U4ERIF:1;
  unsigned U4RXIF:1;
  unsigned U4TXIF:1;
  unsigned SPF3IF:1;
  unsigned SPI3IF:1;
  unsigned OC9IF:1;
  unsigned IC9IF:1;
} IFS5BITS;
extern volatile IFS5BITS IFS5bits __attribute__((__sfr__));

extern volatile unsigned int IFS6 __attribute__((__sfr__));
typedef struct tagIFS6BITS {
  unsigned :4;
  unsigned GFX1IF:1;
} IFS6BITS;
extern volatile IFS6BITS IFS6bits __attribute__((__sfr__));

extern volatile unsigned int IEC0 __attribute__((__sfr__));
typedef struct tagIEC0BITS {
  unsigned INT0IE:1;
  unsigned IC1IE:1;
  unsigned OC1IE:1;
  unsigned T1IE:1;
  unsigned :1;
  unsigned IC2IE:1;
  unsigned OC2IE:1;
  unsigned T2IE:1;
  unsigned T3IE:1;
  unsigned SPF1IE:1;
  unsigned SPI1IE:1;
  unsigned U1RXIE:1;
  unsigned U1TXIE:1;
  unsigned AD1IE:1;
} IEC0BITS;
extern volatile IEC0BITS IEC0bits __attribute__((__sfr__));

extern volatile unsigned int IEC1 __attribute__((__sfr__));
typedef struct tagIEC1BITS {
  unsigned SI2C1IE:1;
  unsigned MI2C1IE:1;
  unsigned CMIE:1;
  unsigned CNIE:1;
  unsigned INT1IE:1;
  unsigned :1;
  unsigned IC7IE:1;
  unsigned IC8IE:1;
  unsigned :1;
  unsigned OC3IE:1;
  unsigned OC4IE:1;
  unsigned T4IE:1;
  unsigned T5IE:1;
  unsigned INT2IE:1;
  unsigned U2RXIE:1;
  unsigned U2TXIE:1;
} IEC1BITS;
extern volatile IEC1BITS IEC1bits __attribute__((__sfr__));

extern volatile unsigned int IEC2 __attribute__((__sfr__));
typedef struct tagIEC2BITS {
  unsigned SPF2IE:1;
  unsigned SPI2IE:1;
  unsigned :3;
  unsigned IC3IE:1;
  unsigned IC4IE:1;
  unsigned IC5IE:1;
  unsigned IC6IE:1;
  unsigned OC5IE:1;
  unsigned OC6IE:1;
  unsigned OC7IE:1;
  unsigned OC8IE:1;
} IEC2BITS;
extern volatile IEC2BITS IEC2bits __attribute__((__sfr__));

extern volatile unsigned int IEC3 __attribute__((__sfr__));
typedef struct tagIEC3BITS {
  unsigned :1;
  unsigned SI2C2IE:1;
  unsigned MI2C2IE:1;
  unsigned :2;
  unsigned INT3IE:1;
  unsigned INT4IE:1;
  unsigned :7;
  unsigned RTCIE:1;
} IEC3BITS;
extern volatile IEC3BITS IEC3bits __attribute__((__sfr__));

extern volatile unsigned int IEC4 __attribute__((__sfr__));
typedef struct tagIEC4BITS {
  unsigned :1;
  unsigned U1ERIE:1;
  unsigned U2ERIE:1;
  unsigned CRCIE:1;
  unsigned :4;
  unsigned LVDIE:1;
  unsigned :4;
  unsigned CTMUIE:1;
} IEC4BITS;
extern volatile IEC4BITS IEC4bits __attribute__((__sfr__));

extern volatile unsigned int IEC5 __attribute__((__sfr__));
typedef struct tagIEC5BITS {
  unsigned :1;
  unsigned U3ERIE:1;
  unsigned U3RXIE:1;
  unsigned U3TXIE:1;
  unsigned SI2C3IE:1;
  unsigned MI2C3IE:1;
  unsigned USB1IE:1;
  unsigned U4ERIE:1;
  unsigned U4RXIE:1;
  unsigned U4TXIE:1;
  unsigned SPF3IE:1;
  unsigned SPI3IE:1;
  unsigned OC9IE:1;
  unsigned IC9IE:1;
} IEC5BITS;
extern volatile IEC5BITS IEC5bits __attribute__((__sfr__));

extern volatile unsigned int IEC6 __attribute__((__sfr__));
typedef struct tagIEC6BITS {
  unsigned :4;
  unsigned GFX1IE:1;
} IEC6BITS;
extern volatile IEC6BITS IEC6bits __attribute__((__sfr__));

extern volatile unsigned int IPC0 __attribute__((__sfr__));
__extension__ typedef struct tagIPC0BITS {
  union {
    struct {
      unsigned INT0IP:3;
      unsigned :1;
      unsigned IC1IP:3;
      unsigned :1;
      unsigned OC1IP:3;
      unsigned :1;
      unsigned T1IP:3;
    };
    struct {
      unsigned INT0IP0:1;
      unsigned INT0IP1:1;
      unsigned INT0IP2:1;
      unsigned :1;
      unsigned IC1IP0:1;
      unsigned IC1IP1:1;
      unsigned IC1IP2:1;
      unsigned :1;
      unsigned OC1IP0:1;
      unsigned OC1IP1:1;
      unsigned OC1IP2:1;
      unsigned :1;
      unsigned T1IP0:1;
      unsigned T1IP1:1;
      unsigned T1IP2:1;
    };
  };
} IPC0BITS;
extern volatile IPC0BITS IPC0bits __attribute__((__sfr__));

extern volatile unsigned int IPC1 __attribute__((__sfr__));
__extension__ typedef struct tagIPC1BITS {
  union {
    struct {
      unsigned :4;
      unsigned IC2IP:3;
      unsigned :1;
      unsigned OC2IP:3;
      unsigned :1;
      unsigned T2IP:3;
    };
    struct {
      unsigned :4;
      unsigned IC2IP0:1;
      unsigned IC2IP1:1;
      unsigned IC2IP2:1;
      unsigned :1;
      unsigned OC2IP0:1;
      unsigned OC2IP1:1;
      unsigned OC2IP2:1;
      unsigned :1;
      unsigned T2IP0:1;
      unsigned T2IP1:1;
      unsigned T2IP2:1;
    };
  };
} IPC1BITS;
extern volatile IPC1BITS IPC1bits __attribute__((__sfr__));

extern volatile unsigned int IPC2 __attribute__((__sfr__));
__extension__ typedef struct tagIPC2BITS {
  union {
    struct {
      unsigned T3IP:3;
      unsigned :1;
      unsigned SPF1IP:3;
      unsigned :1;
      unsigned SPI1IP:3;
      unsigned :1;
      unsigned U1RXIP:3;
    };
    struct {
      unsigned T3IP0:1;
      unsigned T3IP1:1;
      unsigned T3IP2:1;
      unsigned :1;
      unsigned SPF1IP0:1;
      unsigned SPF1IP1:1;
      unsigned SPF1IP2:1;
      unsigned :1;
      unsigned SPI1IP0:1;
      unsigned SPI1IP1:1;
      unsigned SPI1IP2:1;
      unsigned :1;
      unsigned U1RXIP0:1;
      unsigned U1RXIP1:1;
      unsigned U1RXIP2:1;
    };
  };
} IPC2BITS;
extern volatile IPC2BITS IPC2bits __attribute__((__sfr__));

extern volatile unsigned int IPC3 __attribute__((__sfr__));
__extension__ typedef struct tagIPC3BITS {
  union {
    struct {
      unsigned U1TXIP:3;
      unsigned :1;
      unsigned AD1IP:3;
    };
    struct {
      unsigned U1TXIP0:1;
      unsigned U1TXIP1:1;
      unsigned U1TXIP2:1;
      unsigned :1;
      unsigned AD1IP0:1;
      unsigned AD1IP1:1;
      unsigned AD1IP2:1;
    };
  };
} IPC3BITS;
extern volatile IPC3BITS IPC3bits __attribute__((__sfr__));

extern volatile unsigned int IPC4 __attribute__((__sfr__));
__extension__ typedef struct tagIPC4BITS {
  union {
    struct {
      unsigned SI2C1IP:3;
      unsigned :1;
      unsigned MI2C1IP:3;
      unsigned :1;
      unsigned CMIP:3;
      unsigned :1;
      unsigned CNIP:3;
    };
    struct {
      unsigned SI2C1IP0:1;
      unsigned SI2C1IP1:1;
      unsigned SI2C1IP2:1;
      unsigned :1;
      unsigned MI2C1IP0:1;
      unsigned MI2C1IP1:1;
      unsigned MI2C1IP2:1;
      unsigned :1;
      unsigned CMIP0:1;
      unsigned CMIP1:1;
      unsigned CMIP2:1;
      unsigned :1;
      unsigned CNIP0:1;
      unsigned CNIP1:1;
      unsigned CNIP2:1;
    };
    struct {
      unsigned SI2C1P:3;
      unsigned :1;
      unsigned MI2C1P:3;
    };
    struct {
      unsigned SI2C1P0:1;
      unsigned SI2C1P1:1;
      unsigned SI2C1P2:1;
      unsigned :1;
      unsigned MI2C1P0:1;
      unsigned MI2C1P1:1;
      unsigned MI2C1P2:1;
    };
  };
} IPC4BITS;
extern volatile IPC4BITS IPC4bits __attribute__((__sfr__));

extern volatile unsigned int IPC5 __attribute__((__sfr__));
__extension__ typedef struct tagIPC5BITS {
  union {
    struct {
      unsigned INT1IP:3;
      unsigned :5;
      unsigned IC7IP:3;
      unsigned :1;
      unsigned IC8IP:3;
    };
    struct {
      unsigned INT1IP0:1;
      unsigned INT1IP1:1;
      unsigned INT1IP2:1;
      unsigned :5;
      unsigned IC7IP0:1;
      unsigned IC7IP1:1;
      unsigned IC7IP2:1;
      unsigned :1;
      unsigned IC8IP0:1;
      unsigned IC8IP1:1;
      unsigned IC8IP2:1;
    };
  };
} IPC5BITS;
extern volatile IPC5BITS IPC5bits __attribute__((__sfr__));

extern volatile unsigned int IPC6 __attribute__((__sfr__));
__extension__ typedef struct tagIPC6BITS {
  union {
    struct {
      unsigned :4;
      unsigned OC3IP:3;
      unsigned :1;
      unsigned OC4IP:3;
      unsigned :1;
      unsigned T4IP:3;
    };
    struct {
      unsigned :4;
      unsigned OC3IP0:1;
      unsigned OC3IP1:1;
      unsigned OC3IP2:1;
      unsigned :1;
      unsigned OC4IP0:1;
      unsigned OC4IP1:1;
      unsigned OC4IP2:1;
      unsigned :1;
      unsigned T4IP0:1;
      unsigned T4IP1:1;
      unsigned T4IP2:1;
    };
  };
} IPC6BITS;
extern volatile IPC6BITS IPC6bits __attribute__((__sfr__));

extern volatile unsigned int IPC7 __attribute__((__sfr__));
__extension__ typedef struct tagIPC7BITS {
  union {
    struct {
      unsigned T5IP:3;
      unsigned :1;
      unsigned INT2IP:3;
      unsigned :1;
      unsigned U2RXIP:3;
      unsigned :1;
      unsigned U2TXIP:3;
    };
    struct {
      unsigned T5IP0:1;
      unsigned T5IP1:1;
      unsigned T5IP2:1;
      unsigned :1;
      unsigned INT2IP0:1;
      unsigned INT2IP1:1;
      unsigned INT2IP2:1;
      unsigned :1;
      unsigned U2RXIP0:1;
      unsigned U2RXIP1:1;
      unsigned U2RXIP2:1;
      unsigned :1;
      unsigned U2TXIP0:1;
      unsigned U2TXIP1:1;
      unsigned U2TXIP2:1;
    };
  };
} IPC7BITS;
extern volatile IPC7BITS IPC7bits __attribute__((__sfr__));

extern volatile unsigned int IPC8 __attribute__((__sfr__));
__extension__ typedef struct tagIPC8BITS {
  union {
    struct {
      unsigned SPF2IP:3;
      unsigned :1;
      unsigned SPI2IP:3;
    };
    struct {
      unsigned SPF2IP0:1;
      unsigned SPF2IP1:1;
      unsigned SPF2IP2:1;
      unsigned :1;
      unsigned SPI2IP0:1;
      unsigned SPI2IP1:1;
      unsigned SPI2IP2:1;
    };
  };
} IPC8BITS;
extern volatile IPC8BITS IPC8bits __attribute__((__sfr__));

extern volatile unsigned int IPC9 __attribute__((__sfr__));
__extension__ typedef struct tagIPC9BITS {
  union {
    struct {
      unsigned :4;
      unsigned IC3IP:3;
      unsigned :1;
      unsigned IC4IP:3;
      unsigned :1;
      unsigned IC5IP:3;
    };
    struct {
      unsigned :4;
      unsigned IC3IP0:1;
      unsigned IC3IP1:1;
      unsigned IC3IP2:1;
      unsigned :1;
      unsigned IC4IP0:1;
      unsigned IC4IP1:1;
      unsigned IC4IP2:1;
      unsigned :1;
      unsigned IC5IP0:1;
      unsigned IC5IP1:1;
      unsigned IC5IP2:1;
    };
  };
} IPC9BITS;
extern volatile IPC9BITS IPC9bits __attribute__((__sfr__));

extern volatile unsigned int IPC10 __attribute__((__sfr__));
__extension__ typedef struct tagIPC10BITS {
  union {
    struct {
      unsigned IC6IP:3;
      unsigned :1;
      unsigned OC5IP:3;
      unsigned :1;
      unsigned OC6IP:3;
      unsigned :1;
      unsigned OC7IP:3;
    };
    struct {
      unsigned IC6IP0:1;
      unsigned IC6IP1:1;
      unsigned IC6IP2:1;
      unsigned :1;
      unsigned OC5IP0:1;
      unsigned OC5IP1:1;
      unsigned OC5IP2:1;
      unsigned :1;
      unsigned OC6IP0:1;
      unsigned OC6IP1:1;
      unsigned OC6IP2:1;
      unsigned :1;
      unsigned OC7IP0:1;
      unsigned OC7IP1:1;
      unsigned OC7IP2:1;
    };
  };
} IPC10BITS;
extern volatile IPC10BITS IPC10bits __attribute__((__sfr__));

extern volatile unsigned int IPC11 __attribute__((__sfr__));
__extension__ typedef struct tagIPC11BITS {
  union {
    struct {
      unsigned OC8IP:3;
    };
    struct {
      unsigned OC8IP0:1;
      unsigned OC8IP1:1;
      unsigned OC8IP2:1;
    };
  };
} IPC11BITS;
extern volatile IPC11BITS IPC11bits __attribute__((__sfr__));

extern volatile unsigned int IPC12 __attribute__((__sfr__));
__extension__ typedef struct tagIPC12BITS {
  union {
    struct {
      unsigned :4;
      unsigned SI2C2IP:3;
      unsigned :1;
      unsigned MI2C2IP:3;
    };
    struct {
      unsigned :4;
      unsigned SI2C2IP0:1;
      unsigned SI2C2IP1:1;
      unsigned SI2C2IP2:1;
      unsigned :1;
      unsigned MI2C2IP0:1;
      unsigned MI2C2IP1:1;
      unsigned MI2C2IP2:1;
    };
    struct {
      unsigned :4;
      unsigned SI2C2P0:1;
      unsigned SI2C2P1:1;
      unsigned SI2C2P2:1;
      unsigned :1;
      unsigned MI2C2P0:1;
      unsigned MI2C2P1:1;
      unsigned MI2C2P2:1;
    };
    struct {
      unsigned :4;
      unsigned SI2C2P:3;
      unsigned :1;
      unsigned MI2C2P:3;
    };
  };
} IPC12BITS;
extern volatile IPC12BITS IPC12bits __attribute__((__sfr__));

extern volatile unsigned int IPC13 __attribute__((__sfr__));
__extension__ typedef struct tagIPC13BITS {
  union {
    struct {
      unsigned :4;
      unsigned INT3IP:3;
      unsigned :1;
      unsigned INT4IP:3;
    };
    struct {
      unsigned :4;
      unsigned INT3IP0:1;
      unsigned INT3IP1:1;
      unsigned INT3IP2:1;
      unsigned :1;
      unsigned INT4IP0:1;
      unsigned INT4IP1:1;
      unsigned INT4IP2:1;
    };
  };
} IPC13BITS;
extern volatile IPC13BITS IPC13bits __attribute__((__sfr__));

extern volatile unsigned int IPC15 __attribute__((__sfr__));
__extension__ typedef struct tagIPC15BITS {
  union {
    struct {
      unsigned :8;
      unsigned RTCIP:3;
    };
    struct {
      unsigned :8;
      unsigned RTCIP0:1;
      unsigned RTCIP1:1;
      unsigned RTCIP2:1;
    };
  };
} IPC15BITS;
extern volatile IPC15BITS IPC15bits __attribute__((__sfr__));

extern volatile unsigned int IPC16 __attribute__((__sfr__));
__extension__ typedef struct tagIPC16BITS {
  union {
    struct {
      unsigned :4;
      unsigned U1ERIP:3;
      unsigned :1;
      unsigned U2ERIP:3;
      unsigned :1;
      unsigned CRCIP:3;
    };
    struct {
      unsigned :4;
      unsigned U1ERIP0:1;
      unsigned U1ERIP1:1;
      unsigned U1ERIP2:1;
      unsigned :1;
      unsigned U2ERIP0:1;
      unsigned U2ERIP1:1;
      unsigned U2ERIP2:1;
      unsigned :1;
      unsigned CRCIP0:1;
      unsigned CRCIP1:1;
      unsigned CRCIP2:1;
    };
  };
} IPC16BITS;
extern volatile IPC16BITS IPC16bits __attribute__((__sfr__));

extern volatile unsigned int IPC18 __attribute__((__sfr__));
__extension__ typedef struct tagIPC18BITS {
  union {
    struct {
      unsigned LVDIP:3;
    };
    struct {
      unsigned LVDIP0:1;
      unsigned LVDIP1:1;
      unsigned LVDIP2:1;
    };
  };
} IPC18BITS;
extern volatile IPC18BITS IPC18bits __attribute__((__sfr__));

extern volatile unsigned int IPC19 __attribute__((__sfr__));
__extension__ typedef struct tagIPC19BITS {
  union {
    struct {
      unsigned :4;
      unsigned CTMUIP:3;
    };
    struct {
      unsigned :4;
      unsigned CTMUIP0:1;
      unsigned CTMUIP1:1;
      unsigned CTMUIP2:1;
    };
  };
} IPC19BITS;
extern volatile IPC19BITS IPC19bits __attribute__((__sfr__));

extern volatile unsigned int IPC20 __attribute__((__sfr__));
__extension__ typedef struct tagIPC20BITS {
  union {
    struct {
      unsigned :4;
      unsigned U3ERIP:3;
      unsigned :1;
      unsigned U3RXIP:3;
      unsigned :1;
      unsigned U3TXIP:3;
    };
    struct {
      unsigned :4;
      unsigned U3ERIP0:1;
      unsigned U3ERIP1:1;
      unsigned U3ERIP2:1;
      unsigned :1;
      unsigned U3RXIP0:1;
      unsigned U3RXIP1:1;
      unsigned U3RXIP2:1;
      unsigned :1;
      unsigned U3TXIP0:1;
      unsigned U3TXIP1:1;
      unsigned U3TXIP2:1;
    };
  };
} IPC20BITS;
extern volatile IPC20BITS IPC20bits __attribute__((__sfr__));

extern volatile unsigned int IPC21 __attribute__((__sfr__));
__extension__ typedef struct tagIPC21BITS {
  union {
    struct {
      unsigned SI2C3IP:3;
      unsigned :1;
      unsigned MI2C3IP:3;
      unsigned :1;
      unsigned USB1IP:3;
      unsigned :1;
      unsigned U4ERIP:3;
    };
    struct {
      unsigned SI2C3IP0:1;
      unsigned SI2C3IP1:1;
      unsigned SI2C3IP2:1;
      unsigned :1;
      unsigned MI2C3IP0:1;
      unsigned MI2C3IP1:1;
      unsigned MI2C3IP2:1;
      unsigned :1;
      unsigned USB1IP0:1;
      unsigned USB1IP1:1;
      unsigned USB1IP2:1;
      unsigned :1;
      unsigned U4ERIP0:1;
      unsigned U4ERIP1:1;
      unsigned U4ERIP2:1;
    };
    struct {
      unsigned SI2C3P0:1;
      unsigned SI2C3P1:1;
      unsigned SI2C3P2:1;
      unsigned :1;
      unsigned MI2C3P0:1;
      unsigned MI2C3P1:1;
      unsigned MI2C3P2:1;
    };
    struct {
      unsigned SI2C3P:3;
      unsigned :1;
      unsigned MI2C3P:3;
    };
  };
} IPC21BITS;
extern volatile IPC21BITS IPC21bits __attribute__((__sfr__));

extern volatile unsigned int IPC22 __attribute__((__sfr__));
__extension__ typedef struct tagIPC22BITS {
  union {
    struct {
      unsigned U4RXIP:3;
      unsigned :1;
      unsigned U4TXIP:3;
      unsigned :1;
      unsigned SPF3IP:3;
      unsigned :1;
      unsigned SPI3IP:3;
    };
    struct {
      unsigned U4RXIP0:1;
      unsigned U4RXIP1:1;
      unsigned U4RXIP2:1;
      unsigned :1;
      unsigned U4TXIP0:1;
      unsigned U4TXIP1:1;
      unsigned U4TXIP2:1;
      unsigned :1;
      unsigned SPF3IP0:1;
      unsigned SPF3IP1:1;
      unsigned SPF3IP2:1;
      unsigned :1;
      unsigned SPI3IP0:1;
      unsigned SPI3IP1:1;
      unsigned SPI3IP2:1;
    };
  };
} IPC22BITS;
extern volatile IPC22BITS IPC22bits __attribute__((__sfr__));

extern volatile unsigned int IPC23 __attribute__((__sfr__));
__extension__ typedef struct tagIPC23BITS {
  union {
    struct {
      unsigned OC9IP:3;
      unsigned :1;
      unsigned IC9IP:3;
    };
    struct {
      unsigned OC9IP0:1;
      unsigned OC9IP1:1;
      unsigned OC9IP2:1;
      unsigned :1;
      unsigned IC9IP0:1;
      unsigned IC9IP1:1;
      unsigned IC9IP2:1;
    };
  };
} IPC23BITS;
extern volatile IPC23BITS IPC23bits __attribute__((__sfr__));

extern volatile unsigned int IPC25 __attribute__((__sfr__));
__extension__ typedef struct tagIPC25BITS {
  union {
    struct {
      unsigned GFX1IP:3;
    };
    struct {
      unsigned GFX1IP0:1;
      unsigned GFX1IP1:1;
      unsigned GFX1IP2:1;
    };
  };
} IPC25BITS;
extern volatile IPC25BITS IPC25bits __attribute__((__sfr__));

extern volatile unsigned int INTTREG __attribute__((__sfr__));
__extension__ typedef struct tagINTTREGBITS {
  union {
    struct {
      unsigned VECNUM:7;
      unsigned :1;
      unsigned ILR:4;
      unsigned :1;
      unsigned VHOLD:1;
      unsigned :1;
      unsigned CPUIRQ:1;
    };
    struct {
      unsigned VECNUM0:1;
      unsigned VECNUM1:1;
      unsigned VECNUM2:1;
      unsigned VECNUM3:1;
      unsigned VECNUM4:1;
      unsigned VECNUM5:1;
      unsigned VECNUM6:1;
      unsigned :1;
      unsigned ILR0:1;
      unsigned ILR1:1;
      unsigned ILR2:1;
      unsigned ILR3:1;
    };
  };
} INTTREGBITS;
extern volatile INTTREGBITS INTTREGbits __attribute__((__sfr__));

extern volatile unsigned int TMR1 __attribute__((__sfr__));
extern volatile unsigned int PR1 __attribute__((__sfr__));
extern volatile unsigned int T1CON __attribute__((__sfr__));
__extension__ typedef struct tagT1CONBITS {
  union {
    struct {
      unsigned :1;
      unsigned TCS:1;
      unsigned TSYNC:1;
      unsigned :1;
      unsigned TCKPS:2;
      unsigned TGATE:1;
      unsigned :6;
      unsigned TSIDL:1;
      unsigned :1;
      unsigned TON:1;
    };
    struct {
      unsigned :4;
      unsigned TCKPS0:1;
      unsigned TCKPS1:1;
    };
  };
} T1CONBITS;
extern volatile T1CONBITS T1CONbits __attribute__((__sfr__));

extern volatile unsigned int TMR2 __attribute__((__sfr__));
extern volatile unsigned int TMR3HLD __attribute__((__sfr__));
extern volatile unsigned int TMR3 __attribute__((__sfr__));
extern volatile unsigned int PR2 __attribute__((__sfr__));
extern volatile unsigned int PR3 __attribute__((__sfr__));
extern volatile unsigned int T2CON __attribute__((__sfr__));
__extension__ typedef struct tagT2CONBITS {
  union {
    struct {
      unsigned :1;
      unsigned TCS:1;
      unsigned :1;
      unsigned T32:1;
      unsigned TCKPS:2;
      unsigned TGATE:1;
      unsigned :6;
      unsigned TSIDL:1;
      unsigned :1;
      unsigned TON:1;
    };
    struct {
      unsigned :4;
      unsigned TCKPS0:1;
      unsigned TCKPS1:1;
    };
  };
} T2CONBITS;
extern volatile T2CONBITS T2CONbits __attribute__((__sfr__));

extern volatile unsigned int T3CON __attribute__((__sfr__));
__extension__ typedef struct tagT3CONBITS {
  union {
    struct {
      unsigned :1;
      unsigned TCS:1;
      unsigned :2;
      unsigned TCKPS:2;
      unsigned TGATE:1;
      unsigned :6;
      unsigned TSIDL:1;
      unsigned :1;
      unsigned TON:1;
    };
    struct {
      unsigned :4;
      unsigned TCKPS0:1;
      unsigned TCKPS1:1;
    };
  };
} T3CONBITS;
extern volatile T3CONBITS T3CONbits __attribute__((__sfr__));

extern volatile unsigned int TMR4 __attribute__((__sfr__));
extern volatile unsigned int TMR5HLD __attribute__((__sfr__));
extern volatile unsigned int TMR5 __attribute__((__sfr__));
extern volatile unsigned int PR4 __attribute__((__sfr__));
extern volatile unsigned int PR5 __attribute__((__sfr__));
extern volatile unsigned int T4CON __attribute__((__sfr__));
__extension__ typedef struct tagT4CONBITS {
  union {
    struct {
      unsigned :1;
      unsigned TCS:1;
      unsigned :1;
      unsigned T45:1;
      unsigned TCKPS:2;
      unsigned TGATE:1;
      unsigned :6;
      unsigned TSIDL:1;
      unsigned :1;
      unsigned TON:1;
    };
    struct {
      unsigned :4;
      unsigned TCKPS0:1;
      unsigned TCKPS1:1;
    };
  };
} T4CONBITS;
extern volatile T4CONBITS T4CONbits __attribute__((__sfr__));

extern volatile unsigned int T5CON __attribute__((__sfr__));
__extension__ typedef struct tagT5CONBITS {
  union {
    struct {
      unsigned :1;
      unsigned TCS:1;
      unsigned :2;
      unsigned TCKPS:2;
      unsigned TGATE:1;
      unsigned :6;
      unsigned TSIDL:1;
      unsigned :1;
      unsigned TON:1;
    };
    struct {
      unsigned :4;
      unsigned TCKPS0:1;
      unsigned TCKPS1:1;
    };
  };
} T5CONBITS;
extern volatile T5CONBITS T5CONbits __attribute__((__sfr__));

extern volatile unsigned int IC1CON1 __attribute__((__sfr__));
__extension__ typedef struct tagIC1CON1BITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned :3;
      unsigned ICTSEL:3;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
      unsigned :3;
      unsigned ICTSEL0:1;
      unsigned ICTSEL1:1;
      unsigned ICTSEL2:1;
    };
  };
} IC1CON1BITS;
extern volatile IC1CON1BITS IC1CON1bits __attribute__((__sfr__));

extern volatile unsigned int IC1CON2 __attribute__((__sfr__));
__extension__ typedef struct tagIC1CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned :1;
      unsigned TRIGSTAT:1;
      unsigned ICTRIG:1;
      unsigned IC32:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
    };
  };
} IC1CON2BITS;
extern volatile IC1CON2BITS IC1CON2bits __attribute__((__sfr__));



typedef struct tagIC {
        unsigned int icxbuf;
        unsigned int icxcon;
} IC, *PIC;


extern volatile IC IC1 __attribute__((__sfr__));
extern volatile IC IC2 __attribute__((__sfr__));
extern volatile IC IC3 __attribute__((__sfr__));
extern volatile IC IC4 __attribute__((__sfr__));
extern volatile IC IC5 __attribute__((__sfr__));

extern volatile unsigned int IC1BUF __attribute__((__sfr__));
extern volatile unsigned int IC1TMR __attribute__((__sfr__));
extern volatile unsigned int IC2CON1 __attribute__((__sfr__));
__extension__ typedef struct tagIC2CON1BITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned :3;
      unsigned ICTSEL:3;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
      unsigned :3;
      unsigned ICTSEL0:1;
      unsigned ICTSEL1:1;
      unsigned ICTSEL2:1;
    };
  };
} IC2CON1BITS;
extern volatile IC2CON1BITS IC2CON1bits __attribute__((__sfr__));

extern volatile unsigned int IC2CON2 __attribute__((__sfr__));
__extension__ typedef struct tagIC2CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned :1;
      unsigned TRIGSTAT:1;
      unsigned ICTRIG:1;
      unsigned IC32:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
    };
  };
} IC2CON2BITS;
extern volatile IC2CON2BITS IC2CON2bits __attribute__((__sfr__));

extern volatile unsigned int IC2BUF __attribute__((__sfr__));
extern volatile unsigned int IC2TMR __attribute__((__sfr__));
extern volatile unsigned int IC3CON1 __attribute__((__sfr__));
__extension__ typedef struct tagIC3CON1BITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned :3;
      unsigned ICTSEL:3;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
      unsigned :3;
      unsigned ICTSEL0:1;
      unsigned ICTSEL1:1;
      unsigned ICTSEL2:1;
    };
  };
} IC3CON1BITS;
extern volatile IC3CON1BITS IC3CON1bits __attribute__((__sfr__));

extern volatile unsigned int IC3CON2 __attribute__((__sfr__));
__extension__ typedef struct tagIC3CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned :1;
      unsigned TRIGSTAT:1;
      unsigned ICTRIG:1;
      unsigned IC32:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
    };
  };
} IC3CON2BITS;
extern volatile IC3CON2BITS IC3CON2bits __attribute__((__sfr__));

extern volatile unsigned int IC3BUF __attribute__((__sfr__));
extern volatile unsigned int IC3TMR __attribute__((__sfr__));
extern volatile unsigned int IC4CON1 __attribute__((__sfr__));
__extension__ typedef struct tagIC4CON1BITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned :3;
      unsigned ICTSEL:3;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
      unsigned :3;
      unsigned ICTSEL0:1;
      unsigned ICTSEL1:1;
      unsigned ICTSEL2:1;
    };
  };
} IC4CON1BITS;
extern volatile IC4CON1BITS IC4CON1bits __attribute__((__sfr__));

extern volatile unsigned int IC4CON2 __attribute__((__sfr__));
__extension__ typedef struct tagIC4CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned :1;
      unsigned TRIGSTAT:1;
      unsigned ICTRIG:1;
      unsigned IC32:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
    };
  };
} IC4CON2BITS;
extern volatile IC4CON2BITS IC4CON2bits __attribute__((__sfr__));

extern volatile unsigned int IC4BUF __attribute__((__sfr__));
extern volatile unsigned int IC4TMR __attribute__((__sfr__));
extern volatile unsigned int IC5CON1 __attribute__((__sfr__));
__extension__ typedef struct tagIC5CON1BITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned :3;
      unsigned ICTSEL:3;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
      unsigned :3;
      unsigned ICTSEL0:1;
      unsigned ICTSEL1:1;
      unsigned ICTSEL2:1;
    };
  };
} IC5CON1BITS;
extern volatile IC5CON1BITS IC5CON1bits __attribute__((__sfr__));

extern volatile unsigned int IC5CON2 __attribute__((__sfr__));
__extension__ typedef struct tagIC5CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned :1;
      unsigned TRIGSTAT:1;
      unsigned ICTRIG:1;
      unsigned IC32:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
    };
  };
} IC5CON2BITS;
extern volatile IC5CON2BITS IC5CON2bits __attribute__((__sfr__));

extern volatile unsigned int IC5BUF __attribute__((__sfr__));
extern volatile unsigned int IC5TMR __attribute__((__sfr__));
extern volatile unsigned int IC6CON1 __attribute__((__sfr__));
__extension__ typedef struct tagIC6CON1BITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned :3;
      unsigned ICTSEL:3;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
      unsigned :3;
      unsigned ICTSEL0:1;
      unsigned ICTSEL1:1;
      unsigned ICTSEL2:1;
    };
  };
} IC6CON1BITS;
extern volatile IC6CON1BITS IC6CON1bits __attribute__((__sfr__));

extern volatile unsigned int IC6CON2 __attribute__((__sfr__));
__extension__ typedef struct tagIC6CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned :1;
      unsigned TRIGSTAT:1;
      unsigned ICTRIG:1;
      unsigned IC32:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
    };
  };
} IC6CON2BITS;
extern volatile IC6CON2BITS IC6CON2bits __attribute__((__sfr__));

extern volatile unsigned int IC6BUF __attribute__((__sfr__));
extern volatile unsigned int IC6TMR __attribute__((__sfr__));
extern volatile unsigned int IC7CON1 __attribute__((__sfr__));
__extension__ typedef struct tagIC7CON1BITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned :3;
      unsigned ICTSEL:3;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
      unsigned :3;
      unsigned ICTSEL0:1;
      unsigned ICTSEL1:1;
      unsigned ICTSEL2:1;
    };
  };
} IC7CON1BITS;
extern volatile IC7CON1BITS IC7CON1bits __attribute__((__sfr__));

extern volatile unsigned int IC7CON2 __attribute__((__sfr__));
__extension__ typedef struct tagIC7CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned :1;
      unsigned TRIGSTAT:1;
      unsigned ICTRIG:1;
      unsigned IC32:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
    };
  };
} IC7CON2BITS;
extern volatile IC7CON2BITS IC7CON2bits __attribute__((__sfr__));

extern volatile unsigned int IC7BUF __attribute__((__sfr__));
extern volatile unsigned int IC7TMR __attribute__((__sfr__));
extern volatile unsigned int IC8CON1 __attribute__((__sfr__));
__extension__ typedef struct tagIC8CON1BITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned :3;
      unsigned ICTSEL:3;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
      unsigned :3;
      unsigned ICTSEL0:1;
      unsigned ICTSEL1:1;
      unsigned ICTSEL2:1;
    };
  };
} IC8CON1BITS;
extern volatile IC8CON1BITS IC8CON1bits __attribute__((__sfr__));

extern volatile unsigned int IC8CON2 __attribute__((__sfr__));
__extension__ typedef struct tagIC8CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned :1;
      unsigned TRIGSTAT:1;
      unsigned ICTRIG:1;
      unsigned IC32:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
    };
  };
} IC8CON2BITS;
extern volatile IC8CON2BITS IC8CON2bits __attribute__((__sfr__));

extern volatile unsigned int IC8BUF __attribute__((__sfr__));
extern volatile unsigned int IC8TMR __attribute__((__sfr__));
extern volatile unsigned int IC9CON1 __attribute__((__sfr__));
__extension__ typedef struct tagIC9CON1BITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned :3;
      unsigned ICTSEL:3;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
      unsigned :3;
      unsigned ICTSEL0:1;
      unsigned ICTSEL1:1;
      unsigned ICTSEL2:1;
    };
  };
} IC9CON1BITS;
extern volatile IC9CON1BITS IC9CON1bits __attribute__((__sfr__));

extern volatile unsigned int IC9CON2 __attribute__((__sfr__));
__extension__ typedef struct tagIC9CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned :1;
      unsigned TRIGSTAT:1;
      unsigned ICTRIG:1;
      unsigned IC32:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
    };
  };
} IC9CON2BITS;
extern volatile IC9CON2BITS IC9CON2bits __attribute__((__sfr__));

extern volatile unsigned int IC9BUF __attribute__((__sfr__));
extern volatile unsigned int IC9TMR __attribute__((__sfr__));
extern volatile unsigned int OC1CON1 __attribute__((__sfr__));
__extension__ typedef struct tagOC1CON1BITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned TRIGMODE:1;
      unsigned OCFLT:3;
      unsigned ENFLT:3;
      unsigned OCTSEL:3;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
      unsigned :1;
      unsigned OCFLT0:1;
      unsigned OCFLT1:1;
      unsigned OCFLT2:1;
      unsigned ENFLT0:1;
      unsigned ENFLT1:1;
      unsigned ENFLT2:1;
      unsigned OCTSEL0:1;
      unsigned OCTSEL1:1;
      unsigned OCTSEL2:1;
    };
  };
} OC1CON1BITS;
extern volatile OC1CON1BITS OC1CON1bits __attribute__((__sfr__));

extern volatile unsigned int OC1CON2 __attribute__((__sfr__));
__extension__ typedef struct tagOC1CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned OCTRIS:1;
      unsigned TRIGSTAT:1;
      unsigned OCTRIG:1;
      unsigned OC32:1;
      unsigned DCB:2;
      unsigned :1;
      unsigned OCINV:1;
      unsigned FLTTRIEN:1;
      unsigned FLTOUT:1;
      unsigned FLTMD:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
      unsigned :4;
      unsigned DCB0:1;
      unsigned DCB1:1;
      unsigned :4;
      unsigned FLTMODE:1;
    };
  };
} OC1CON2BITS;
extern volatile OC1CON2BITS OC1CON2bits __attribute__((__sfr__));



typedef struct tagOC {
        unsigned int ocxrs;
        unsigned int ocxr;
        unsigned int ocxcon;
} OC, *POC;


extern volatile OC OC1 __attribute__((__sfr__));
extern volatile OC OC2 __attribute__((__sfr__));
extern volatile OC OC3 __attribute__((__sfr__));
extern volatile OC OC4 __attribute__((__sfr__));
extern volatile OC OC5 __attribute__((__sfr__));

extern volatile unsigned int OC1RS __attribute__((__sfr__));
extern volatile unsigned int OC1R __attribute__((__sfr__));
extern volatile unsigned int OC1TMR __attribute__((__sfr__));
extern volatile unsigned int OC2CON1 __attribute__((__sfr__));
__extension__ typedef struct tagOC2CON1BITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned TRIGMODE:1;
      unsigned OCFLT:3;
      unsigned ENFLT:3;
      unsigned OCTSEL:3;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
      unsigned :1;
      unsigned OCFLT0:1;
      unsigned OCFLT1:1;
      unsigned OCFLT2:1;
      unsigned ENFLT0:1;
      unsigned ENFLT1:1;
      unsigned ENFLT2:1;
      unsigned OCTSEL0:1;
      unsigned OCTSEL1:1;
      unsigned OCTSEL2:1;
    };
  };
} OC2CON1BITS;
extern volatile OC2CON1BITS OC2CON1bits __attribute__((__sfr__));

extern volatile unsigned int OC2CON2 __attribute__((__sfr__));
__extension__ typedef struct tagOC2CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned OCTRIS:1;
      unsigned TRIGSTAT:1;
      unsigned OCTRIG:1;
      unsigned OC32:1;
      unsigned DCB:2;
      unsigned :1;
      unsigned OCINV:1;
      unsigned FLTTRIEN:1;
      unsigned FLTOUT:1;
      unsigned FLTMD:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
      unsigned :4;
      unsigned DCB0:1;
      unsigned DCB1:1;
      unsigned :4;
      unsigned FLTMODE:1;
    };
  };
} OC2CON2BITS;
extern volatile OC2CON2BITS OC2CON2bits __attribute__((__sfr__));

extern volatile unsigned int OC2RS __attribute__((__sfr__));
extern volatile unsigned int OC2R __attribute__((__sfr__));
extern volatile unsigned int OC2TMR __attribute__((__sfr__));
extern volatile unsigned int OC3CON1 __attribute__((__sfr__));
__extension__ typedef struct tagOC3CON1BITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned TRIGMODE:1;
      unsigned OCFLT:3;
      unsigned ENFLT:3;
      unsigned OCTSEL:3;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
      unsigned :1;
      unsigned OCFLT0:1;
      unsigned OCFLT1:1;
      unsigned OCFLT2:1;
      unsigned ENFLT0:1;
      unsigned ENFLT1:1;
      unsigned ENFLT2:1;
      unsigned OCTSEL0:1;
      unsigned OCTSEL1:1;
      unsigned OCTSEL2:1;
    };
  };
} OC3CON1BITS;
extern volatile OC3CON1BITS OC3CON1bits __attribute__((__sfr__));

extern volatile unsigned int OC3CON2 __attribute__((__sfr__));
__extension__ typedef struct tagOC3CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned OCTRIS:1;
      unsigned TRIGSTAT:1;
      unsigned OCTRIG:1;
      unsigned OC32:1;
      unsigned DCB:2;
      unsigned :1;
      unsigned OCINV:1;
      unsigned FLTTRIEN:1;
      unsigned FLTOUT:1;
      unsigned FLTMD:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
      unsigned :4;
      unsigned DCB0:1;
      unsigned DCB1:1;
      unsigned :4;
      unsigned FLTMODE:1;
    };
  };
} OC3CON2BITS;
extern volatile OC3CON2BITS OC3CON2bits __attribute__((__sfr__));

extern volatile unsigned int OC3RS __attribute__((__sfr__));
extern volatile unsigned int OC3R __attribute__((__sfr__));
extern volatile unsigned int OC3TMR __attribute__((__sfr__));
extern volatile unsigned int OC4CON1 __attribute__((__sfr__));
__extension__ typedef struct tagOC4CON1BITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned TRIGMODE:1;
      unsigned OCFLT:3;
      unsigned ENFLT:3;
      unsigned OCTSEL:3;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
      unsigned :1;
      unsigned OCFLT0:1;
      unsigned OCFLT1:1;
      unsigned OCFLT2:1;
      unsigned ENFLT0:1;
      unsigned ENFLT1:1;
      unsigned ENFLT2:1;
      unsigned OCTSEL0:1;
      unsigned OCTSEL1:1;
      unsigned OCTSEL2:1;
    };
  };
} OC4CON1BITS;
extern volatile OC4CON1BITS OC4CON1bits __attribute__((__sfr__));

extern volatile unsigned int OC4CON2 __attribute__((__sfr__));
__extension__ typedef struct tagOC4CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned OCTRIS:1;
      unsigned TRIGSTAT:1;
      unsigned OCTRIG:1;
      unsigned OC32:1;
      unsigned DCB:2;
      unsigned :1;
      unsigned OCINV:1;
      unsigned FLTTRIEN:1;
      unsigned FLTOUT:1;
      unsigned FLTMD:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
      unsigned :4;
      unsigned DCB0:1;
      unsigned DCB1:1;
      unsigned :4;
      unsigned FLTMODE:1;
    };
  };
} OC4CON2BITS;
extern volatile OC4CON2BITS OC4CON2bits __attribute__((__sfr__));

extern volatile unsigned int OC4RS __attribute__((__sfr__));
extern volatile unsigned int OC4R __attribute__((__sfr__));
extern volatile unsigned int OC4TMR __attribute__((__sfr__));
extern volatile unsigned int OC5CON1 __attribute__((__sfr__));
__extension__ typedef struct tagOC5CON1BITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned TRIGMODE:1;
      unsigned OCFLT:3;
      unsigned ENFLT:3;
      unsigned OCTSEL:3;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
      unsigned :1;
      unsigned OCFLT0:1;
      unsigned OCFLT1:1;
      unsigned OCFLT2:1;
      unsigned ENFLT0:1;
      unsigned ENFLT1:1;
      unsigned ENFLT2:1;
      unsigned OCTSEL0:1;
      unsigned OCTSEL1:1;
      unsigned OCTSEL2:1;
    };
  };
} OC5CON1BITS;
extern volatile OC5CON1BITS OC5CON1bits __attribute__((__sfr__));

extern volatile unsigned int OC5CON2 __attribute__((__sfr__));
__extension__ typedef struct tagOC5CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned OCTRIS:1;
      unsigned TRIGSTAT:1;
      unsigned OCTRIG:1;
      unsigned OC32:1;
      unsigned DCB:2;
      unsigned :1;
      unsigned OCINV:1;
      unsigned FLTTRIEN:1;
      unsigned FLTOUT:1;
      unsigned FLTMD:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
      unsigned :4;
      unsigned DCB0:1;
      unsigned DCB1:1;
      unsigned :4;
      unsigned FLTMODE:1;
    };
  };
} OC5CON2BITS;
extern volatile OC5CON2BITS OC5CON2bits __attribute__((__sfr__));

extern volatile unsigned int OC5RS __attribute__((__sfr__));
extern volatile unsigned int OC5R __attribute__((__sfr__));
extern volatile unsigned int OC5TMR __attribute__((__sfr__));
extern volatile unsigned int OC6CON1 __attribute__((__sfr__));
__extension__ typedef struct tagOC6CON1BITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned TRIGMODE:1;
      unsigned OCFLT:3;
      unsigned ENFLT:3;
      unsigned OCTSEL:3;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
      unsigned :1;
      unsigned OCFLT0:1;
      unsigned OCFLT1:1;
      unsigned OCFLT2:1;
      unsigned ENFLT0:1;
      unsigned ENFLT1:1;
      unsigned ENFLT2:1;
      unsigned OCTSEL0:1;
      unsigned OCTSEL1:1;
      unsigned OCTSEL2:1;
    };
  };
} OC6CON1BITS;
extern volatile OC6CON1BITS OC6CON1bits __attribute__((__sfr__));

extern volatile unsigned int OC6CON2 __attribute__((__sfr__));
__extension__ typedef struct tagOC6CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned OCTRIS:1;
      unsigned TRIGSTAT:1;
      unsigned OCTRIG:1;
      unsigned OC32:1;
      unsigned DCB:2;
      unsigned :1;
      unsigned OCINV:1;
      unsigned FLTTRIEN:1;
      unsigned FLTOUT:1;
      unsigned FLTMD:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
      unsigned :4;
      unsigned DCB0:1;
      unsigned DCB1:1;
      unsigned :4;
      unsigned FLTMODE:1;
    };
  };
} OC6CON2BITS;
extern volatile OC6CON2BITS OC6CON2bits __attribute__((__sfr__));

extern volatile unsigned int OC6RS __attribute__((__sfr__));
extern volatile unsigned int OC6R __attribute__((__sfr__));
extern volatile unsigned int OC6TMR __attribute__((__sfr__));
extern volatile unsigned int OC7CON1 __attribute__((__sfr__));
__extension__ typedef struct tagOC7CON1BITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned TRIGMODE:1;
      unsigned OCFLT:3;
      unsigned ENFLT:3;
      unsigned OCTSEL:3;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
      unsigned :1;
      unsigned OCFLT0:1;
      unsigned OCFLT1:1;
      unsigned OCFLT2:1;
      unsigned ENFLT0:1;
      unsigned ENFLT1:1;
      unsigned ENFLT2:1;
      unsigned OCTSEL0:1;
      unsigned OCTSEL1:1;
      unsigned OCTSEL2:1;
    };
  };
} OC7CON1BITS;
extern volatile OC7CON1BITS OC7CON1bits __attribute__((__sfr__));

extern volatile unsigned int OC7CON2 __attribute__((__sfr__));
__extension__ typedef struct tagOC7CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned OCTRIS:1;
      unsigned TRIGSTAT:1;
      unsigned OCTRIG:1;
      unsigned OC32:1;
      unsigned DCB:2;
      unsigned :1;
      unsigned OCINV:1;
      unsigned FLTTRIEN:1;
      unsigned FLTOUT:1;
      unsigned FLTMD:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
      unsigned :4;
      unsigned DCB0:1;
      unsigned DCB1:1;
      unsigned :4;
      unsigned FLTMODE:1;
    };
  };
} OC7CON2BITS;
extern volatile OC7CON2BITS OC7CON2bits __attribute__((__sfr__));

extern volatile unsigned int OC7RS __attribute__((__sfr__));
extern volatile unsigned int OC7R __attribute__((__sfr__));
extern volatile unsigned int OC7TMR __attribute__((__sfr__));
extern volatile unsigned int OC8CON1 __attribute__((__sfr__));
__extension__ typedef struct tagOC8CON1BITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned TRIGMODE:1;
      unsigned OCFLT:3;
      unsigned ENFLT:3;
      unsigned OCTSEL:3;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
      unsigned :1;
      unsigned OCFLT0:1;
      unsigned OCFLT1:1;
      unsigned OCFLT2:1;
      unsigned ENFLT0:1;
      unsigned ENFLT1:1;
      unsigned ENFLT2:1;
      unsigned OCTSEL0:1;
      unsigned OCTSEL1:1;
      unsigned OCTSEL2:1;
    };
  };
} OC8CON1BITS;
extern volatile OC8CON1BITS OC8CON1bits __attribute__((__sfr__));

extern volatile unsigned int OC8CON2 __attribute__((__sfr__));
__extension__ typedef struct tagOC8CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned OCTRIS:1;
      unsigned TRIGSTAT:1;
      unsigned OCTRIG:1;
      unsigned OC32:1;
      unsigned DCB:2;
      unsigned :1;
      unsigned OCINV:1;
      unsigned FLTTRIEN:1;
      unsigned FLTOUT:1;
      unsigned FLTMD:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
      unsigned :4;
      unsigned DCB0:1;
      unsigned DCB1:1;
      unsigned :4;
      unsigned FLTMODE:1;
    };
  };
} OC8CON2BITS;
extern volatile OC8CON2BITS OC8CON2bits __attribute__((__sfr__));

extern volatile unsigned int OC8RS __attribute__((__sfr__));
extern volatile unsigned int OC8R __attribute__((__sfr__));
extern volatile unsigned int OC8TMR __attribute__((__sfr__));
extern volatile unsigned int OC9CON1 __attribute__((__sfr__));
__extension__ typedef struct tagOC9CON1BITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned TRIGMODE:1;
      unsigned OCFLT:3;
      unsigned ENFLT:3;
      unsigned OCTSEL:3;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
      unsigned :1;
      unsigned OCFLT0:1;
      unsigned OCFLT1:1;
      unsigned OCFLT2:1;
      unsigned ENFLT0:1;
      unsigned ENFLT1:1;
      unsigned ENFLT2:1;
      unsigned OCTSEL0:1;
      unsigned OCTSEL1:1;
      unsigned OCTSEL2:1;
    };
  };
} OC9CON1BITS;
extern volatile OC9CON1BITS OC9CON1bits __attribute__((__sfr__));

extern volatile unsigned int OC9CON2 __attribute__((__sfr__));
__extension__ typedef struct tagOC9CON2BITS {
  union {
    struct {
      unsigned SYNCSEL:5;
      unsigned OCTRIS:1;
      unsigned TRIGSTAT:1;
      unsigned OCTRIG:1;
      unsigned OC32:1;
      unsigned DCB:2;
      unsigned :1;
      unsigned OCINV:1;
      unsigned FLTTRIEN:1;
      unsigned FLTOUT:1;
      unsigned FLTMD:1;
    };
    struct {
      unsigned SYNCSEL0:1;
      unsigned SYNCSEL1:1;
      unsigned SYNCSEL2:1;
      unsigned SYNCSEL3:1;
      unsigned SYNCSEL4:1;
      unsigned :4;
      unsigned DCB0:1;
      unsigned DCB1:1;
      unsigned :4;
      unsigned FLTMODE:1;
    };
  };
} OC9CON2BITS;
extern volatile OC9CON2BITS OC9CON2bits __attribute__((__sfr__));

extern volatile unsigned int OC9RS __attribute__((__sfr__));
extern volatile unsigned int OC9R __attribute__((__sfr__));
extern volatile unsigned int OC9TMR __attribute__((__sfr__));
extern volatile unsigned int I2C1RCV __attribute__((__sfr__));
extern volatile unsigned int I2C1TRN __attribute__((__sfr__));
extern volatile unsigned int I2C1BRG __attribute__((__sfr__));
extern volatile unsigned int I2C1CON __attribute__((__sfr__));
typedef struct tagI2C1CONBITS {
  unsigned SEN:1;
  unsigned RSEN:1;
  unsigned PEN:1;
  unsigned RCEN:1;
  unsigned ACKEN:1;
  unsigned ACKDT:1;
  unsigned STREN:1;
  unsigned GCEN:1;
  unsigned SMEN:1;
  unsigned DISSLW:1;
  unsigned A10M:1;
  unsigned IPMIEN:1;
  unsigned SCLREL:1;
  unsigned I2CSIDL:1;
  unsigned :1;
  unsigned I2CEN:1;
} I2C1CONBITS;
extern volatile I2C1CONBITS I2C1CONbits __attribute__((__sfr__));

extern volatile unsigned int I2C1STAT __attribute__((__sfr__));
__extension__ typedef struct tagI2C1STATBITS {
  union {
    struct {
      unsigned TBF:1;
      unsigned RBF:1;
      unsigned R_NOT_W:1;
      unsigned S:1;
      unsigned P:1;
      unsigned D_NOT_A:1;
      unsigned I2COV:1;
      unsigned IWCOL:1;
      unsigned ADD10:1;
      unsigned GCSTAT:1;
      unsigned BCL:1;
      unsigned :3;
      unsigned TRSTAT:1;
      unsigned ACKSTAT:1;
    };
    struct {
      unsigned :2;
      unsigned R_W:1;
      unsigned :2;
      unsigned D_A:1;
    };
  };
} I2C1STATBITS;
extern volatile I2C1STATBITS I2C1STATbits __attribute__((__sfr__));

extern volatile unsigned int I2C1ADD __attribute__((__sfr__));
extern volatile unsigned int I2C1MSK __attribute__((__sfr__));
extern volatile unsigned int I2C2RCV __attribute__((__sfr__));
extern volatile unsigned int I2C2TRN __attribute__((__sfr__));
extern volatile unsigned int I2C2BRG __attribute__((__sfr__));
extern volatile unsigned int I2C2CON __attribute__((__sfr__));
typedef struct tagI2C2CONBITS {
  unsigned SEN:1;
  unsigned RSEN:1;
  unsigned PEN:1;
  unsigned RCEN:1;
  unsigned ACKEN:1;
  unsigned ACKDT:1;
  unsigned STREN:1;
  unsigned GCEN:1;
  unsigned SMEN:1;
  unsigned DISSLW:1;
  unsigned A10M:1;
  unsigned IPMIEN:1;
  unsigned SCLREL:1;
  unsigned I2CSIDL:1;
  unsigned :1;
  unsigned I2CEN:1;
} I2C2CONBITS;
extern volatile I2C2CONBITS I2C2CONbits __attribute__((__sfr__));

extern volatile unsigned int I2C2STAT __attribute__((__sfr__));
__extension__ typedef struct tagI2C2STATBITS {
  union {
    struct {
      unsigned TBF:1;
      unsigned RBF:1;
      unsigned R_NOT_W:1;
      unsigned S:1;
      unsigned P:1;
      unsigned D_NOT_A:1;
      unsigned I2COV:1;
      unsigned IWCOL:1;
      unsigned ADD10:1;
      unsigned GCSTAT:1;
      unsigned BCL:1;
      unsigned :3;
      unsigned TRSTAT:1;
      unsigned ACKSTAT:1;
    };
    struct {
      unsigned :2;
      unsigned R_W:1;
      unsigned :2;
      unsigned D_A:1;
    };
  };
} I2C2STATBITS;
extern volatile I2C2STATBITS I2C2STATbits __attribute__((__sfr__));

extern volatile unsigned int I2C2ADD __attribute__((__sfr__));
extern volatile unsigned int I2C2MSK __attribute__((__sfr__));


typedef struct tagUART {
        unsigned int uxmode;
        unsigned int uxsta;
        unsigned int uxtxreg;
        unsigned int uxrxreg;
        unsigned int uxbrg;
} UART, *PUART;
# 2564 "/opt/microchip/xc16/v1.10/bin/bin/../../support/PIC24F/h/p24FJ256DA206.h" 3 4
extern volatile UART UART1 __attribute__((__sfr__));
extern volatile UART UART2 __attribute__((__sfr__));

extern volatile unsigned int U1MODE __attribute__((__sfr__));
__extension__ typedef struct tagU1MODEBITS {
  union {
    struct {
      unsigned STSEL:1;
      unsigned PDSEL:2;
      unsigned BRGH:1;
      unsigned RXINV:1;
      unsigned ABAUD:1;
      unsigned LPBACK:1;
      unsigned WAKE:1;
      unsigned UEN:2;
      unsigned :1;
      unsigned RTSMD:1;
      unsigned IREN:1;
      unsigned USIDL:1;
      unsigned :1;
      unsigned UARTEN:1;
    };
    struct {
      unsigned :1;
      unsigned PDSEL0:1;
      unsigned PDSEL1:1;
      unsigned :5;
      unsigned UEN0:1;
      unsigned UEN1:1;
    };
  };
} U1MODEBITS;
extern volatile U1MODEBITS U1MODEbits __attribute__((__sfr__));

extern volatile unsigned int U1STA __attribute__((__sfr__));
__extension__ typedef struct tagU1STABITS {
  union {
    struct {
      unsigned URXDA:1;
      unsigned OERR:1;
      unsigned FERR:1;
      unsigned PERR:1;
      unsigned RIDLE:1;
      unsigned ADDEN:1;
      unsigned URXISEL:2;
      unsigned TRMT:1;
      unsigned UTXBF:1;
      unsigned UTXEN:1;
      unsigned UTXBRK:1;
      unsigned :1;
      unsigned UTXISEL0:1;
      unsigned UTXINV:1;
      unsigned UTXISEL1:1;
    };
    struct {
      unsigned :6;
      unsigned URXISEL0:1;
      unsigned URXISEL1:1;
    };
  };
} U1STABITS;
extern volatile U1STABITS U1STAbits __attribute__((__sfr__));

extern volatile unsigned int U1TXREG __attribute__((__sfr__));
typedef struct tagU1TXREGBITS {
  unsigned UTXREG0:1;
  unsigned UTXREG1:1;
  unsigned UTXREG2:1;
  unsigned UTXREG3:1;
  unsigned UTXREG4:1;
  unsigned UTXREG5:1;
  unsigned UTXREG6:1;
  unsigned UTXREG7:1;
  unsigned UTXREG8:1;
} U1TXREGBITS;
extern volatile U1TXREGBITS U1TXREGbits __attribute__((__sfr__));

extern volatile unsigned int U1RXREG __attribute__((__sfr__));
typedef struct tagU1RXREGBITS {
  unsigned URXREG0:1;
  unsigned URXREG1:1;
  unsigned URXREG2:1;
  unsigned URXREG3:1;
  unsigned URXREG4:1;
  unsigned URXREG5:1;
  unsigned URXREG6:1;
  unsigned URXREG7:1;
  unsigned URXREG8:1;
} U1RXREGBITS;
extern volatile U1RXREGBITS U1RXREGbits __attribute__((__sfr__));

extern volatile unsigned int U1BRG __attribute__((__sfr__));
extern volatile unsigned int U2MODE __attribute__((__sfr__));
__extension__ typedef struct tagU2MODEBITS {
  union {
    struct {
      unsigned STSEL:1;
      unsigned PDSEL:2;
      unsigned BRGH:1;
      unsigned RXINV:1;
      unsigned ABAUD:1;
      unsigned LPBACK:1;
      unsigned WAKE:1;
      unsigned UEN:2;
      unsigned :1;
      unsigned RTSMD:1;
      unsigned IREN:1;
      unsigned USIDL:1;
      unsigned :1;
      unsigned UARTEN:1;
    };
    struct {
      unsigned :1;
      unsigned PDSEL0:1;
      unsigned PDSEL1:1;
      unsigned :5;
      unsigned UEN0:1;
      unsigned UEN1:1;
    };
  };
} U2MODEBITS;
extern volatile U2MODEBITS U2MODEbits __attribute__((__sfr__));

extern volatile unsigned int U2STA __attribute__((__sfr__));
__extension__ typedef struct tagU2STABITS {
  union {
    struct {
      unsigned URXDA:1;
      unsigned OERR:1;
      unsigned FERR:1;
      unsigned PERR:1;
      unsigned RIDLE:1;
      unsigned ADDEN:1;
      unsigned URXISEL:2;
      unsigned TRMT:1;
      unsigned UTXBF:1;
      unsigned UTXEN:1;
      unsigned UTXBRK:1;
      unsigned :1;
      unsigned UTXISEL0:1;
      unsigned UTXINV:1;
      unsigned UTXISEL1:1;
    };
    struct {
      unsigned :6;
      unsigned URXISEL0:1;
      unsigned URXISEL1:1;
    };
  };
} U2STABITS;
extern volatile U2STABITS U2STAbits __attribute__((__sfr__));

extern volatile unsigned int U2TXREG __attribute__((__sfr__));
typedef struct tagU2TXREGBITS {
  unsigned UTXREG0:1;
  unsigned UTXREG1:1;
  unsigned UTXREG2:1;
  unsigned UTXREG3:1;
  unsigned UTXREG4:1;
  unsigned UTXREG5:1;
  unsigned UTXREG6:1;
  unsigned UTXREG7:1;
  unsigned UTXREG8:1;
} U2TXREGBITS;
extern volatile U2TXREGBITS U2TXREGbits __attribute__((__sfr__));

extern volatile unsigned int U2RXREG __attribute__((__sfr__));
typedef struct tagU2RXREGBITS {
  unsigned URXREG0:1;
  unsigned URXREG1:1;
  unsigned URXREG2:1;
  unsigned URXREG3:1;
  unsigned URXREG4:1;
  unsigned URXREG5:1;
  unsigned URXREG6:1;
  unsigned URXREG7:1;
  unsigned URXREG8:1;
} U2RXREGBITS;
extern volatile U2RXREGBITS U2RXREGbits __attribute__((__sfr__));

extern volatile unsigned int U2BRG __attribute__((__sfr__));


typedef struct tagSPI {
        unsigned int spixstat;
        unsigned int spixcon1;
        unsigned int spixcon2;
        unsigned int unused;
        unsigned int spixbuf;
} SPI, *PSPI;





extern volatile SPI SPI1 __attribute__((__sfr__));
extern volatile SPI SPI2 __attribute__((__sfr__));

extern volatile unsigned int SPI1STAT __attribute__((__sfr__));
__extension__ typedef struct tagSPI1STATBITS {
  union {
    struct {
      unsigned SPIRBF:1;
      unsigned SPITBF:1;
      unsigned SISEL:3;
      unsigned SRXMPT:1;
      unsigned SPIROV:1;
      unsigned SRMPT:1;
      unsigned SPIBEC:3;
      unsigned :2;
      unsigned SPISIDL:1;
      unsigned :1;
      unsigned SPIEN:1;
    };
    struct {
      unsigned :2;
      unsigned SISEL0:1;
      unsigned SISEL1:1;
      unsigned SISEL2:1;
      unsigned :3;
      unsigned SPIBEC0:1;
      unsigned SPIBEC1:1;
      unsigned SPIBEC2:1;
    };
  };
} SPI1STATBITS;
extern volatile SPI1STATBITS SPI1STATbits __attribute__((__sfr__));

extern volatile unsigned int SPI1CON1 __attribute__((__sfr__));
__extension__ typedef struct tagSPI1CON1BITS {
  union {
    struct {
      unsigned PPRE:2;
      unsigned SPRE:3;
      unsigned MSTEN:1;
      unsigned CKP:1;
      unsigned SSEN:1;
      unsigned CKE:1;
      unsigned SMP:1;
      unsigned MODE16:1;
      unsigned DISSDO:1;
      unsigned DISSCK:1;
    };
    struct {
      unsigned PPRE0:1;
      unsigned PPRE1:1;
      unsigned SPRE0:1;
      unsigned SPRE1:1;
      unsigned SPRE2:1;
    };
  };
} SPI1CON1BITS;
extern volatile SPI1CON1BITS SPI1CON1bits __attribute__((__sfr__));

extern volatile unsigned int SPI1CON2 __attribute__((__sfr__));
typedef struct tagSPI1CON2BITS {
  unsigned SPIBEN:1;
  unsigned SPIFE:1;
  unsigned :11;
  unsigned SPIFPOL:1;
  unsigned SPIFSD:1;
  unsigned FRMEN:1;
} SPI1CON2BITS;
extern volatile SPI1CON2BITS SPI1CON2bits __attribute__((__sfr__));

extern volatile unsigned int SPI1BUF __attribute__((__sfr__));
extern volatile unsigned int U3MODE __attribute__((__sfr__));
__extension__ typedef struct tagU3MODEBITS {
  union {
    struct {
      unsigned STSEL:1;
      unsigned PDSEL:2;
      unsigned BRGH:1;
      unsigned RXINV:1;
      unsigned ABAUD:1;
      unsigned LPBACK:1;
      unsigned WAKE:1;
      unsigned UEN:2;
      unsigned :1;
      unsigned RTSMD:1;
      unsigned IREN:1;
      unsigned USIDL:1;
      unsigned :1;
      unsigned UARTEN:1;
    };
    struct {
      unsigned :1;
      unsigned PDSEL0:1;
      unsigned PDSEL1:1;
      unsigned :5;
      unsigned UEN0:1;
      unsigned UEN1:1;
    };
  };
} U3MODEBITS;
extern volatile U3MODEBITS U3MODEbits __attribute__((__sfr__));

extern volatile unsigned int U3STA __attribute__((__sfr__));
__extension__ typedef struct tagU3STABITS {
  union {
    struct {
      unsigned URXDA:1;
      unsigned OERR:1;
      unsigned FERR:1;
      unsigned PERR:1;
      unsigned RIDLE:1;
      unsigned ADDEN:1;
      unsigned URXISEL:2;
      unsigned TRMT:1;
      unsigned UTXBF:1;
      unsigned UTXEN:1;
      unsigned UTXBRK:1;
      unsigned :1;
      unsigned UTXISEL0:1;
      unsigned UTXINV:1;
      unsigned UTXISEL1:1;
    };
    struct {
      unsigned :6;
      unsigned URXISEL0:1;
      unsigned URXISEL1:1;
    };
  };
} U3STABITS;
extern volatile U3STABITS U3STAbits __attribute__((__sfr__));

extern volatile unsigned int U3TXREG __attribute__((__sfr__));
typedef struct tagU3TXREGBITS {
  unsigned UTXREG0:1;
  unsigned UTXREG1:1;
  unsigned UTXREG2:1;
  unsigned UTXREG3:1;
  unsigned UTXREG4:1;
  unsigned UTXREG5:1;
  unsigned UTXREG6:1;
  unsigned UTXREG7:1;
  unsigned UTXREG8:1;
} U3TXREGBITS;
extern volatile U3TXREGBITS U3TXREGbits __attribute__((__sfr__));

extern volatile unsigned int U3RXREG __attribute__((__sfr__));
typedef struct tagU3RXREGBITS {
  unsigned URXREG0:1;
  unsigned URXREG1:1;
  unsigned URXREG2:1;
  unsigned URXREG3:1;
  unsigned URXREG4:1;
  unsigned URXREG5:1;
  unsigned URXREG6:1;
  unsigned URXREG7:1;
  unsigned URXREG8:1;
} U3RXREGBITS;
extern volatile U3RXREGBITS U3RXREGbits __attribute__((__sfr__));

extern volatile unsigned int U3BRG __attribute__((__sfr__));
extern volatile unsigned int SPI2STAT __attribute__((__sfr__));
__extension__ typedef struct tagSPI2STATBITS {
  union {
    struct {
      unsigned SPIRBF:1;
      unsigned SPITBF:1;
      unsigned SISEL:3;
      unsigned SRXMPT:1;
      unsigned SPIROV:1;
      unsigned SRMPT:1;
      unsigned SPIBEC:3;
      unsigned :2;
      unsigned SPISIDL:1;
      unsigned :1;
      unsigned SPIEN:1;
    };
    struct {
      unsigned :2;
      unsigned SISEL0:1;
      unsigned SISEL1:1;
      unsigned SISEL2:1;
      unsigned :3;
      unsigned SPIBEC0:1;
      unsigned SPIBEC1:1;
      unsigned SPIBEC2:1;
    };
  };
} SPI2STATBITS;
extern volatile SPI2STATBITS SPI2STATbits __attribute__((__sfr__));

extern volatile unsigned int SPI2CON1 __attribute__((__sfr__));
__extension__ typedef struct tagSPI2CON1BITS {
  union {
    struct {
      unsigned PPRE:2;
      unsigned SPRE:3;
      unsigned MSTEN:1;
      unsigned CKP:1;
      unsigned SSEN:1;
      unsigned CKE:1;
      unsigned SMP:1;
      unsigned MODE16:1;
      unsigned DISSDO:1;
      unsigned DISSCK:1;
    };
    struct {
      unsigned PPRE0:1;
      unsigned PPRE1:1;
      unsigned SPRE0:1;
      unsigned SPRE1:1;
      unsigned SPRE2:1;
    };
  };
} SPI2CON1BITS;
extern volatile SPI2CON1BITS SPI2CON1bits __attribute__((__sfr__));

extern volatile unsigned int SPI2CON2 __attribute__((__sfr__));
typedef struct tagSPI2CON2BITS {
  unsigned SPIBEN:1;
  unsigned SPIFE:1;
  unsigned :11;
  unsigned SPIFPOL:1;
  unsigned SPIFSD:1;
  unsigned FRMEN:1;
} SPI2CON2BITS;
extern volatile SPI2CON2BITS SPI2CON2bits __attribute__((__sfr__));

extern volatile unsigned int SPI2BUF __attribute__((__sfr__));
extern volatile unsigned int I2C3RCV __attribute__((__sfr__));
extern volatile unsigned int I2C3TRN __attribute__((__sfr__));
extern volatile unsigned int I2C3BRG __attribute__((__sfr__));
extern volatile unsigned int I2C3CON __attribute__((__sfr__));
typedef struct tagI2C3CONBITS {
  unsigned SEN:1;
  unsigned RSEN:1;
  unsigned PEN:1;
  unsigned RCEN:1;
  unsigned ACKEN:1;
  unsigned ACKDT:1;
  unsigned STREN:1;
  unsigned GCEN:1;
  unsigned SMEN:1;
  unsigned DISSLW:1;
  unsigned A10M:1;
  unsigned IPMIEN:1;
  unsigned SCLREL:1;
  unsigned I2CSIDL:1;
  unsigned :1;
  unsigned I2CEN:1;
} I2C3CONBITS;
extern volatile I2C3CONBITS I2C3CONbits __attribute__((__sfr__));

extern volatile unsigned int I2C3STAT __attribute__((__sfr__));
__extension__ typedef struct tagI2C3STATBITS {
  union {
    struct {
      unsigned TBF:1;
      unsigned RBF:1;
      unsigned R_NOT_W:1;
      unsigned S:1;
      unsigned P:1;
      unsigned D_NOT_A:1;
      unsigned I2COV:1;
      unsigned IWCOL:1;
      unsigned ADD10:1;
      unsigned GCSTAT:1;
      unsigned BCL:1;
      unsigned :3;
      unsigned TRSTAT:1;
      unsigned ACKSTAT:1;
    };
    struct {
      unsigned :2;
      unsigned R_W:1;
      unsigned :2;
      unsigned D_A:1;
    };
  };
} I2C3STATBITS;
extern volatile I2C3STATBITS I2C3STATbits __attribute__((__sfr__));

extern volatile unsigned int I2C3ADD __attribute__((__sfr__));
extern volatile unsigned int I2C3MSK __attribute__((__sfr__));
extern volatile unsigned int SPI3STAT __attribute__((__sfr__));
__extension__ typedef struct tagSPI3STATBITS {
  union {
    struct {
      unsigned SPIRBF:1;
      unsigned SPITBF:1;
      unsigned SISEL:3;
      unsigned SRXMPT:1;
      unsigned SPIROV:1;
      unsigned SRMPT:1;
      unsigned SPIBEC:3;
      unsigned :2;
      unsigned SPISIDL:1;
      unsigned :1;
      unsigned SPIEN:1;
    };
    struct {
      unsigned :2;
      unsigned SISEL0:1;
      unsigned SISEL1:1;
      unsigned SISEL2:1;
      unsigned :3;
      unsigned SPIBEC0:1;
      unsigned SPIBEC1:1;
      unsigned SPIBEC2:1;
    };
  };
} SPI3STATBITS;
extern volatile SPI3STATBITS SPI3STATbits __attribute__((__sfr__));

extern volatile unsigned int SPI3CON1 __attribute__((__sfr__));
__extension__ typedef struct tagSPI3CON1BITS {
  union {
    struct {
      unsigned PPRE:2;
      unsigned SPRE:3;
      unsigned MSTEN:1;
      unsigned CKP:1;
      unsigned SSEN:1;
      unsigned CKE:1;
      unsigned SMP:1;
      unsigned MODE16:1;
      unsigned DISSDO:1;
      unsigned DISSCK:1;
    };
    struct {
      unsigned PPRE0:1;
      unsigned PPRE1:1;
      unsigned SPRE0:1;
      unsigned SPRE1:1;
      unsigned SPRE2:1;
    };
  };
} SPI3CON1BITS;
extern volatile SPI3CON1BITS SPI3CON1bits __attribute__((__sfr__));

extern volatile unsigned int SPI3CON2 __attribute__((__sfr__));
typedef struct tagSPI3CON2BITS {
  unsigned SPIBEN:1;
  unsigned SPIFE:1;
  unsigned :11;
  unsigned SPIFPOL:1;
  unsigned SPIFSD:1;
  unsigned FRMEN:1;
} SPI3CON2BITS;
extern volatile SPI3CON2BITS SPI3CON2bits __attribute__((__sfr__));

extern volatile unsigned int SPI3BUF __attribute__((__sfr__));
extern volatile unsigned int U4MODE __attribute__((__sfr__));
__extension__ typedef struct tagU4MODEBITS {
  union {
    struct {
      unsigned STSEL:1;
      unsigned PDSEL:2;
      unsigned BRGH:1;
      unsigned RXINV:1;
      unsigned ABAUD:1;
      unsigned LPBACK:1;
      unsigned WAKE:1;
      unsigned UEN:2;
      unsigned :1;
      unsigned RTSMD:1;
      unsigned IREN:1;
      unsigned USIDL:1;
      unsigned :1;
      unsigned UARTEN:1;
    };
    struct {
      unsigned :1;
      unsigned PDSEL0:1;
      unsigned PDSEL1:1;
      unsigned :5;
      unsigned UEN0:1;
      unsigned UEN1:1;
    };
  };
} U4MODEBITS;
extern volatile U4MODEBITS U4MODEbits __attribute__((__sfr__));

extern volatile unsigned int U4STA __attribute__((__sfr__));
__extension__ typedef struct tagU4STABITS {
  union {
    struct {
      unsigned URXDA:1;
      unsigned OERR:1;
      unsigned FERR:1;
      unsigned PERR:1;
      unsigned RIDLE:1;
      unsigned ADDEN:1;
      unsigned URXISEL:2;
      unsigned TRMT:1;
      unsigned UTXBF:1;
      unsigned UTXEN:1;
      unsigned UTXBRK:1;
      unsigned :1;
      unsigned UTXISEL0:1;
      unsigned UTXINV:1;
      unsigned UTXISEL1:1;
    };
    struct {
      unsigned :6;
      unsigned URXISEL0:1;
      unsigned URXISEL1:1;
    };
  };
} U4STABITS;
extern volatile U4STABITS U4STAbits __attribute__((__sfr__));

extern volatile unsigned int U4TXREG __attribute__((__sfr__));
typedef struct tagU4TXREGBITS {
  unsigned UTXREG0:1;
  unsigned UTXREG1:1;
  unsigned UTXREG2:1;
  unsigned UTXREG3:1;
  unsigned UTXREG4:1;
  unsigned UTXREG5:1;
  unsigned UTXREG6:1;
  unsigned UTXREG7:1;
  unsigned UTXREG8:1;
} U4TXREGBITS;
extern volatile U4TXREGBITS U4TXREGbits __attribute__((__sfr__));

extern volatile unsigned int U4RXREG __attribute__((__sfr__));
typedef struct tagU4RXREGBITS {
  unsigned URXREG0:1;
  unsigned URXREG1:1;
  unsigned URXREG2:1;
  unsigned URXREG3:1;
  unsigned URXREG4:1;
  unsigned URXREG5:1;
  unsigned URXREG6:1;
  unsigned URXREG7:1;
  unsigned URXREG8:1;
} U4RXREGBITS;
extern volatile U4RXREGBITS U4RXREGbits __attribute__((__sfr__));

extern volatile unsigned int U4BRG __attribute__((__sfr__));
extern volatile unsigned int TRISB __attribute__((__sfr__));
typedef struct tagTRISBBITS {
  unsigned TRISB0:1;
  unsigned TRISB1:1;
  unsigned TRISB2:1;
  unsigned TRISB3:1;
  unsigned TRISB4:1;
  unsigned TRISB5:1;
  unsigned TRISB6:1;
  unsigned TRISB7:1;
  unsigned TRISB8:1;
  unsigned TRISB9:1;
  unsigned TRISB10:1;
  unsigned TRISB11:1;
  unsigned TRISB12:1;
  unsigned TRISB13:1;
  unsigned TRISB14:1;
  unsigned TRISB15:1;
} TRISBBITS;
extern volatile TRISBBITS TRISBbits __attribute__((__sfr__));

extern volatile unsigned int PORTB __attribute__((__sfr__));
typedef struct tagPORTBBITS {
  unsigned RB0:1;
  unsigned RB1:1;
  unsigned RB2:1;
  unsigned RB3:1;
  unsigned RB4:1;
  unsigned RB5:1;
  unsigned RB6:1;
  unsigned RB7:1;
  unsigned RB8:1;
  unsigned RB9:1;
  unsigned RB10:1;
  unsigned RB11:1;
  unsigned RB12:1;
  unsigned RB13:1;
  unsigned RB14:1;
  unsigned RB15:1;
} PORTBBITS;
extern volatile PORTBBITS PORTBbits __attribute__((__sfr__));

extern volatile unsigned int LATB __attribute__((__sfr__));
typedef struct tagLATBBITS {
  unsigned LATB0:1;
  unsigned LATB1:1;
  unsigned LATB2:1;
  unsigned LATB3:1;
  unsigned LATB4:1;
  unsigned LATB5:1;
  unsigned LATB6:1;
  unsigned LATB7:1;
  unsigned LATB8:1;
  unsigned LATB9:1;
  unsigned LATB10:1;
  unsigned LATB11:1;
  unsigned LATB12:1;
  unsigned LATB13:1;
  unsigned LATB14:1;
  unsigned LATB15:1;
} LATBBITS;
extern volatile LATBBITS LATBbits __attribute__((__sfr__));

extern volatile unsigned int ODCB __attribute__((__sfr__));
typedef struct tagODCBBITS {
  unsigned ODB0:1;
  unsigned ODB1:1;
  unsigned ODB2:1;
  unsigned ODB3:1;
  unsigned ODB4:1;
  unsigned ODB5:1;
  unsigned ODB6:1;
  unsigned ODB7:1;
  unsigned ODB8:1;
  unsigned ODB9:1;
  unsigned ODB10:1;
  unsigned ODB11:1;
  unsigned ODB12:1;
  unsigned ODB13:1;
  unsigned ODB14:1;
  unsigned ODB15:1;
} ODCBBITS;
extern volatile ODCBBITS ODCBbits __attribute__((__sfr__));

extern volatile unsigned int TRISC __attribute__((__sfr__));
typedef struct tagTRISCBITS {
  unsigned :12;
  unsigned TRISC12:1;
  unsigned TRISC13:1;
  unsigned TRISC14:1;
  unsigned TRISC15:1;
} TRISCBITS;
extern volatile TRISCBITS TRISCbits __attribute__((__sfr__));

extern volatile unsigned int PORTC __attribute__((__sfr__));
typedef struct tagPORTCBITS {
  unsigned :12;
  unsigned RC12:1;
  unsigned RC13:1;
  unsigned RC14:1;
  unsigned RC15:1;
} PORTCBITS;
extern volatile PORTCBITS PORTCbits __attribute__((__sfr__));

extern volatile unsigned int LATC __attribute__((__sfr__));
typedef struct tagLATCBITS {
  unsigned :12;
  unsigned LATC12:1;
  unsigned LATC13:1;
  unsigned LATC14:1;
  unsigned LATC15:1;
} LATCBITS;
extern volatile LATCBITS LATCbits __attribute__((__sfr__));

extern volatile unsigned int ODCC __attribute__((__sfr__));
typedef struct tagODCCBITS {
  unsigned :12;
  unsigned ODC12:1;
  unsigned ODC13:1;
  unsigned ODC14:1;
  unsigned ODC15:1;
} ODCCBITS;
extern volatile ODCCBITS ODCCbits __attribute__((__sfr__));

extern volatile unsigned int TRISD __attribute__((__sfr__));
typedef struct tagTRISDBITS {
  unsigned TRISD0:1;
  unsigned TRISD1:1;
  unsigned TRISD2:1;
  unsigned TRISD3:1;
  unsigned TRISD4:1;
  unsigned TRISD5:1;
  unsigned TRISD6:1;
  unsigned TRISD7:1;
  unsigned TRISD8:1;
  unsigned TRISD9:1;
  unsigned TRISD10:1;
  unsigned TRISD11:1;
} TRISDBITS;
extern volatile TRISDBITS TRISDbits __attribute__((__sfr__));

extern volatile unsigned int PORTD __attribute__((__sfr__));
typedef struct tagPORTDBITS {
  unsigned RD0:1;
  unsigned RD1:1;
  unsigned RD2:1;
  unsigned RD3:1;
  unsigned RD4:1;
  unsigned RD5:1;
  unsigned RD6:1;
  unsigned RD7:1;
  unsigned RD8:1;
  unsigned RD9:1;
  unsigned RD10:1;
  unsigned RD11:1;
} PORTDBITS;
extern volatile PORTDBITS PORTDbits __attribute__((__sfr__));

extern volatile unsigned int LATD __attribute__((__sfr__));
typedef struct tagLATDBITS {
  unsigned LATD0:1;
  unsigned LATD1:1;
  unsigned LATD2:1;
  unsigned LATD3:1;
  unsigned LATD4:1;
  unsigned LATD5:1;
  unsigned LATD6:1;
  unsigned LATD7:1;
  unsigned LATD8:1;
  unsigned LATD9:1;
  unsigned LATD10:1;
  unsigned LATD11:1;
} LATDBITS;
extern volatile LATDBITS LATDbits __attribute__((__sfr__));

extern volatile unsigned int ODCD __attribute__((__sfr__));
typedef struct tagODCDBITS {
  unsigned ODD0:1;
  unsigned ODD1:1;
  unsigned ODD2:1;
  unsigned ODD3:1;
  unsigned ODD4:1;
  unsigned ODD5:1;
  unsigned ODD6:1;
  unsigned ODD7:1;
  unsigned ODD8:1;
  unsigned ODD9:1;
  unsigned ODD10:1;
  unsigned ODD11:1;
} ODCDBITS;
extern volatile ODCDBITS ODCDbits __attribute__((__sfr__));

extern volatile unsigned int TRISE __attribute__((__sfr__));
typedef struct tagTRISEBITS {
  unsigned TRISE0:1;
  unsigned TRISE1:1;
  unsigned TRISE2:1;
  unsigned TRISE3:1;
  unsigned TRISE4:1;
  unsigned TRISE5:1;
  unsigned TRISE6:1;
  unsigned TRISE7:1;
} TRISEBITS;
extern volatile TRISEBITS TRISEbits __attribute__((__sfr__));

extern volatile unsigned int PORTE __attribute__((__sfr__));
typedef struct tagPORTEBITS {
  unsigned RE0:1;
  unsigned RE1:1;
  unsigned RE2:1;
  unsigned RE3:1;
  unsigned RE4:1;
  unsigned RE5:1;
  unsigned RE6:1;
  unsigned RE7:1;
} PORTEBITS;
extern volatile PORTEBITS PORTEbits __attribute__((__sfr__));

extern volatile unsigned int LATE __attribute__((__sfr__));
typedef struct tagLATEBITS {
  unsigned LATE0:1;
  unsigned LATE1:1;
  unsigned LATE2:1;
  unsigned LATE3:1;
  unsigned LATE4:1;
  unsigned LATE5:1;
  unsigned LATE6:1;
  unsigned LATE7:1;
} LATEBITS;
extern volatile LATEBITS LATEbits __attribute__((__sfr__));

extern volatile unsigned int ODCE __attribute__((__sfr__));
typedef struct tagODCEBITS {
  unsigned ODE0:1;
  unsigned ODE1:1;
  unsigned ODE2:1;
  unsigned ODE3:1;
  unsigned ODE4:1;
  unsigned ODE5:1;
  unsigned ODE6:1;
  unsigned ODE7:1;
} ODCEBITS;
extern volatile ODCEBITS ODCEbits __attribute__((__sfr__));

extern volatile unsigned int TRISF __attribute__((__sfr__));
typedef struct tagTRISFBITS {
  unsigned TRISF0:1;
  unsigned TRISF1:1;
  unsigned :1;
  unsigned TRISF3:1;
  unsigned TRISF4:1;
  unsigned TRISF5:1;
  unsigned :1;
  unsigned TRISF7:1;
} TRISFBITS;
extern volatile TRISFBITS TRISFbits __attribute__((__sfr__));

extern volatile unsigned int PORTF __attribute__((__sfr__));
typedef struct tagPORTFBITS {
  unsigned RF0:1;
  unsigned RF1:1;
  unsigned :1;
  unsigned RF3:1;
  unsigned RF4:1;
  unsigned RF5:1;
  unsigned :1;
  unsigned RF7:1;
} PORTFBITS;
extern volatile PORTFBITS PORTFbits __attribute__((__sfr__));

extern volatile unsigned int LATF __attribute__((__sfr__));
typedef struct tagLATFBITS {
  unsigned LATF0:1;
  unsigned LATF1:1;
  unsigned :1;
  unsigned LATF3:1;
  unsigned LATF4:1;
  unsigned LATF5:1;
  unsigned :1;
  unsigned LATF7:1;
} LATFBITS;
extern volatile LATFBITS LATFbits __attribute__((__sfr__));

extern volatile unsigned int ODCF __attribute__((__sfr__));
typedef struct tagODCFBITS {
  unsigned ODF0:1;
  unsigned ODF1:1;
  unsigned :1;
  unsigned ODF3:1;
  unsigned ODF4:1;
  unsigned ODF5:1;
  unsigned :1;
  unsigned ODF7:1;
} ODCFBITS;
extern volatile ODCFBITS ODCFbits __attribute__((__sfr__));

extern volatile unsigned int TRISG __attribute__((__sfr__));
typedef struct tagTRISGBITS {
  unsigned :2;
  unsigned TRISG2:1;
  unsigned TRISG3:1;
  unsigned :2;
  unsigned TRISG6:1;
  unsigned TRISG7:1;
  unsigned TRISG8:1;
  unsigned TRISG9:1;
} TRISGBITS;
extern volatile TRISGBITS TRISGbits __attribute__((__sfr__));

extern volatile unsigned int PORTG __attribute__((__sfr__));
typedef struct tagPORTGBITS {
  unsigned :2;
  unsigned RG2:1;
  unsigned RG3:1;
  unsigned :2;
  unsigned RG6:1;
  unsigned RG7:1;
  unsigned RG8:1;
  unsigned RG9:1;
} PORTGBITS;
extern volatile PORTGBITS PORTGbits __attribute__((__sfr__));

extern volatile unsigned int LATG __attribute__((__sfr__));
typedef struct tagLATGBITS {
  unsigned :2;
  unsigned LATG2:1;
  unsigned LATG3:1;
  unsigned :2;
  unsigned LATG6:1;
  unsigned LATG7:1;
  unsigned LATG8:1;
  unsigned LATG9:1;
} LATGBITS;
extern volatile LATGBITS LATGbits __attribute__((__sfr__));

extern volatile unsigned int ODCG __attribute__((__sfr__));
typedef struct tagODCGBITS {
  unsigned :2;
  unsigned ODG2:1;
  unsigned ODG3:1;
  unsigned :2;
  unsigned ODG6:1;
  unsigned ODG7:1;
  unsigned ODG8:1;
  unsigned ODG9:1;
} ODCGBITS;
extern volatile ODCGBITS ODCGbits __attribute__((__sfr__));

extern volatile unsigned int PADCFG1 __attribute__((__sfr__));
__extension__ typedef struct tagPADCFG1BITS {
  union {
    struct {
      unsigned :1;
      unsigned RTSECSEL:1;
    };
    struct {
      unsigned :1;
      unsigned RTSECSEL0:1;
    };
  };
} PADCFG1BITS;
extern volatile PADCFG1BITS PADCFG1bits __attribute__((__sfr__));

extern volatile unsigned int ADC1BUF0 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF1 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF2 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF3 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF4 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF5 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF6 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF7 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF8 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF9 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUFA __attribute__((__sfr__));
extern volatile unsigned int ADC1BUFB __attribute__((__sfr__));
extern volatile unsigned int ADC1BUFC __attribute__((__sfr__));
extern volatile unsigned int ADC1BUFD __attribute__((__sfr__));
extern volatile unsigned int ADC1BUFE __attribute__((__sfr__));
extern volatile unsigned int ADC1BUFF __attribute__((__sfr__));
extern volatile unsigned int AD1CON1 __attribute__((__sfr__));
__extension__ typedef struct tagAD1CON1BITS {
  union {
    struct {
      unsigned DONE:1;
      unsigned SAMP:1;
      unsigned ASAM:1;
      unsigned :2;
      unsigned SSRC:3;
      unsigned FORM:2;
      unsigned :3;
      unsigned ADSIDL:1;
      unsigned :1;
      unsigned ADON:1;
    };
    struct {
      unsigned :5;
      unsigned SSRC0:1;
      unsigned SSRC1:1;
      unsigned SSRC2:1;
      unsigned FORM0:1;
      unsigned FORM1:1;
    };
  };
} AD1CON1BITS;
extern volatile AD1CON1BITS AD1CON1bits __attribute__((__sfr__));

extern volatile unsigned int AD1CON2 __attribute__((__sfr__));
__extension__ typedef struct tagAD1CON2BITS {
  union {
    struct {
      unsigned ALTS:1;
      unsigned BUFM:1;
      unsigned SMPI:5;
      unsigned BUFS:1;
      unsigned :2;
      unsigned CSCNA:1;
      unsigned :2;
      unsigned VCFG:3;
    };
    struct {
      unsigned :2;
      unsigned SMPI0:1;
      unsigned SMPI1:1;
      unsigned SMPI2:1;
      unsigned SMPI3:1;
      unsigned SMPI4:1;
      unsigned :6;
      unsigned VCFG0:1;
      unsigned VCFG1:1;
      unsigned VCFG2:1;
    };
  };
} AD1CON2BITS;
extern volatile AD1CON2BITS AD1CON2bits __attribute__((__sfr__));

extern volatile unsigned int AD1CON3 __attribute__((__sfr__));
__extension__ typedef struct tagAD1CON3BITS {
  union {
    struct {
      unsigned ADCS:8;
      unsigned SAMC:5;
      unsigned :2;
      unsigned ADRC:1;
    };
    struct {
      unsigned ADCS0:1;
      unsigned ADCS1:1;
      unsigned ADCS2:1;
      unsigned ADCS3:1;
      unsigned ADCS4:1;
      unsigned ADCS5:1;
      unsigned ADCS6:1;
      unsigned ADCS7:1;
      unsigned SAMC0:1;
      unsigned SAMC1:1;
      unsigned SAMC2:1;
      unsigned SAMC3:1;
      unsigned SAMC4:1;
    };
  };
} AD1CON3BITS;
extern volatile AD1CON3BITS AD1CON3bits __attribute__((__sfr__));

extern volatile unsigned int AD1CHS __attribute__((__sfr__));
__extension__ typedef struct tagAD1CHSBITS {
  union {
    struct {
      unsigned CH0SA:5;
      unsigned :2;
      unsigned CH0NA:1;
      unsigned CH0SB:5;
      unsigned :2;
      unsigned CH0NB:1;
    };
    struct {
      unsigned CH0SA0:1;
      unsigned CH0SA1:1;
      unsigned CH0SA2:1;
      unsigned CH0SA3:1;
      unsigned CH0SA4:1;
      unsigned :3;
      unsigned CH0SB0:1;
      unsigned CH0SB1:1;
      unsigned CH0SB2:1;
      unsigned CH0SB3:1;
      unsigned CH0SB4:1;
    };
  };
} AD1CHSBITS;
extern volatile AD1CHSBITS AD1CHSbits __attribute__((__sfr__));

extern volatile unsigned int AD1CHS0 __attribute__((__sfr__));
__extension__ typedef struct tagAD1CHS0BITS {
  union {
    struct {
      unsigned CH0SA:5;
      unsigned :2;
      unsigned CH0NA:1;
      unsigned CH0SB:5;
      unsigned :2;
      unsigned CH0NB:1;
    };
    struct {
      unsigned CH0SA0:1;
      unsigned CH0SA1:1;
      unsigned CH0SA2:1;
      unsigned CH0SA3:1;
      unsigned CH0SA4:1;
      unsigned :3;
      unsigned CH0SB0:1;
      unsigned CH0SB1:1;
      unsigned CH0SB2:1;
      unsigned CH0SB3:1;
      unsigned CH0SB4:1;
    };
  };
} AD1CHS0BITS;
extern volatile AD1CHS0BITS AD1CHS0bits __attribute__((__sfr__));

extern volatile unsigned int AD1CSSH __attribute__((__sfr__));
typedef struct tagAD1CSSHBITS {
  unsigned :8;
  unsigned CSSL24:1;
  unsigned CSSL25:1;
  unsigned CSSL26:1;
  unsigned CSSL27:1;
} AD1CSSHBITS;
extern volatile AD1CSSHBITS AD1CSSHbits __attribute__((__sfr__));

extern volatile unsigned int AD1CSSL __attribute__((__sfr__));
typedef struct tagAD1CSSLBITS {
  unsigned CSSL0:1;
  unsigned CSSL1:1;
  unsigned CSSL2:1;
  unsigned CSSL3:1;
  unsigned CSSL4:1;
  unsigned CSSL5:1;
  unsigned CSSL6:1;
  unsigned CSSL7:1;
  unsigned CSSL8:1;
  unsigned CSSL9:1;
  unsigned CSSL10:1;
  unsigned CSSL11:1;
  unsigned CSSL12:1;
  unsigned CSSL13:1;
  unsigned CSSL14:1;
  unsigned CSSL15:1;
} AD1CSSLBITS;
extern volatile AD1CSSLBITS AD1CSSLbits __attribute__((__sfr__));

extern volatile unsigned int CTMUCON __attribute__((__sfr__));
__extension__ typedef struct tagCTMUCONBITS {
  union {
    struct {
      unsigned EDG1STAT:1;
      unsigned EDG2STAT:1;
      unsigned EDG1SEL:2;
      unsigned EDG1POL:1;
      unsigned EDG2SEL:2;
      unsigned EDG2POL:1;
      unsigned CTTRIG:1;
      unsigned IDISSEN:1;
      unsigned EDGSEQEN:1;
      unsigned EDGEN:1;
      unsigned TGEN:1;
      unsigned CTMUSIDL:1;
      unsigned :1;
      unsigned CTMUEN:1;
    };
    struct {
      unsigned :2;
      unsigned EDG1SEL0:1;
      unsigned EDG1SEL1:1;
      unsigned :1;
      unsigned EDG2SEL0:1;
      unsigned EDG2SEL1:1;
    };
  };
} CTMUCONBITS;
extern volatile CTMUCONBITS CTMUCONbits __attribute__((__sfr__));

extern volatile unsigned int CTMUICON __attribute__((__sfr__));
__extension__ typedef struct tagCTMUICONBITS {
  union {
    struct {
      unsigned :8;
      unsigned IRNG:2;
      unsigned ITRIM:6;
    };
    struct {
      unsigned :8;
      unsigned IRNG0:1;
      unsigned IRNG1:1;
      unsigned ITRIM0:1;
      unsigned ITRIM1:1;
      unsigned ITRIM2:1;
      unsigned ITRIM3:1;
      unsigned ITRIM4:1;
      unsigned ITRIM5:1;
    };
  };
} CTMUICONBITS;
extern volatile CTMUICONBITS CTMUICONbits __attribute__((__sfr__));

extern volatile unsigned int ADC1BUF10 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF11 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF12 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF13 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF14 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF15 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF16 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF17 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF18 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF19 __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF1A __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF1B __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF1C __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF1D __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF1E __attribute__((__sfr__));
extern volatile unsigned int ADC1BUF1F __attribute__((__sfr__));
extern volatile unsigned int U1OTGIR __attribute__((__sfr__));
typedef struct tagU1OTGIRBITS {
  unsigned VBUSVDIF:1;
  unsigned :1;
  unsigned SESENDIF:1;
  unsigned SESVDIF:1;
  unsigned ACTVIF:1;
  unsigned LSTATEIF:1;
  unsigned T1MSECIF:1;
  unsigned IDIF:1;
} U1OTGIRBITS;
extern volatile U1OTGIRBITS U1OTGIRbits __attribute__((__sfr__));

extern volatile unsigned int U1OTGIE __attribute__((__sfr__));
typedef struct tagU1OTGIEBITS {
  unsigned VBUSVDIE:1;
  unsigned :1;
  unsigned SESENDIE:1;
  unsigned SESVDIE:1;
  unsigned ACTVIE:1;
  unsigned LSTATEIE:1;
  unsigned T1MSECIE:1;
  unsigned IDIE:1;
} U1OTGIEBITS;
extern volatile U1OTGIEBITS U1OTGIEbits __attribute__((__sfr__));

extern volatile unsigned int U1OTGSTAT __attribute__((__sfr__));
typedef struct tagU1OTGSTATBITS {
  unsigned VBUSVD:1;
  unsigned :1;
  unsigned SESEND:1;
  unsigned SESVD:1;
  unsigned :1;
  unsigned LSTATE:1;
  unsigned :1;
  unsigned ID:1;
} U1OTGSTATBITS;
extern volatile U1OTGSTATBITS U1OTGSTATbits __attribute__((__sfr__));

extern volatile unsigned int U1OTGCON __attribute__((__sfr__));
typedef struct tagU1OTGCONBITS {
  unsigned VBUSDIS:1;
  unsigned VBUSCHG:1;
  unsigned OTGEN:1;
  unsigned VBUSON:1;
  unsigned DMPULDWN:1;
  unsigned DPPULDWN:1;
  unsigned DMPULUP:1;
  unsigned DPPULUP:1;
} U1OTGCONBITS;
extern volatile U1OTGCONBITS U1OTGCONbits __attribute__((__sfr__));

extern volatile unsigned int U1PWRC __attribute__((__sfr__));
__extension__ typedef struct tagU1PWRCBITS {
  union {
    struct {
      unsigned USBPWR:1;
      unsigned USUSPND:1;
      unsigned :2;
      unsigned USLPGRD:1;
      unsigned :2;
      unsigned UACTPND:1;
    };
    struct {
      unsigned :1;
      unsigned USUSPEND:1;
    };
  };
} U1PWRCBITS;
extern volatile U1PWRCBITS U1PWRCbits __attribute__((__sfr__));

extern volatile unsigned int U1IR __attribute__((__sfr__));
__extension__ typedef struct tagU1IRBITS {
  union {
    struct {
      unsigned URSTIF:1;
      unsigned UERRIF:1;
      unsigned SOFIF:1;
      unsigned TRNIF:1;
      unsigned IDLEIF:1;
      unsigned RESUMEIF:1;
      unsigned ATTACHIF:1;
      unsigned STALLIF:1;
    };
    struct {
      unsigned DETACHIF:1;
    };
  };
} U1IRBITS;
extern volatile U1IRBITS U1IRbits __attribute__((__sfr__));

extern volatile unsigned int U1IE __attribute__((__sfr__));
__extension__ typedef struct tagU1IEBITS {
  union {
    struct {
      unsigned URSTIE:1;
      unsigned UERRIE:1;
      unsigned SOFIE:1;
      unsigned TRNIE:1;
      unsigned IDLEIE:1;
      unsigned RESUMEIE:1;
      unsigned ATTACHIE:1;
      unsigned STALLIE:1;
    };
    struct {
      unsigned DETACHIE:1;
    };
  };
} U1IEBITS;
extern volatile U1IEBITS U1IEbits __attribute__((__sfr__));

extern volatile unsigned int U1EIR __attribute__((__sfr__));
__extension__ typedef struct tagU1EIRBITS {
  union {
    struct {
      unsigned PIDEF:1;
      unsigned CRC5EF:1;
      unsigned CRC16EF:1;
      unsigned DFN8EF:1;
      unsigned BTOEF:1;
      unsigned DMAEF:1;
      unsigned :1;
      unsigned BTSEF:1;
    };
    struct {
      unsigned :1;
      unsigned EOFEF:1;
    };
  };
} U1EIRBITS;
extern volatile U1EIRBITS U1EIRbits __attribute__((__sfr__));

extern volatile unsigned int U1EIE __attribute__((__sfr__));
__extension__ typedef struct tagU1EIEBITS {
  union {
    struct {
      unsigned PIDEE:1;
      unsigned CRC5EE:1;
      unsigned CRC16EE:1;
      unsigned DFN8EE:1;
      unsigned BTOEE:1;
      unsigned DMAEE:1;
      unsigned :1;
      unsigned BTSEE:1;
    };
    struct {
      unsigned :1;
      unsigned EOFEE:1;
    };
  };
} U1EIEBITS;
extern volatile U1EIEBITS U1EIEbits __attribute__((__sfr__));

extern volatile unsigned int U1STAT __attribute__((__sfr__));
__extension__ typedef struct tagU1STATBITS {
  union {
    struct {
      unsigned :2;
      unsigned PPBI:1;
      unsigned DIR:1;
      unsigned ENDPT:4;
    };
    struct {
      unsigned :4;
      unsigned ENDPT0:1;
      unsigned ENDPT1:1;
      unsigned ENDPT2:1;
      unsigned ENDPT3:1;
    };
  };
} U1STATBITS;
extern volatile U1STATBITS U1STATbits __attribute__((__sfr__));

extern volatile unsigned int U1CON __attribute__((__sfr__));
__extension__ typedef struct tagU1CONBITS {
  union {
    struct {
      unsigned USBEN:1;
      unsigned PPBRST:1;
      unsigned RESUME:1;
      unsigned HOSTEN:1;
      unsigned USBRST:1;
      unsigned PKTDIS:1;
      unsigned SE0:1;
      unsigned JSTATE:1;
    };
    struct {
      unsigned SOFEN:1;
      unsigned :3;
      unsigned RESET:1;
      unsigned TOKBUSY:1;
    };
  };
} U1CONBITS;
extern volatile U1CONBITS U1CONbits __attribute__((__sfr__));

extern volatile unsigned int U1ADDR __attribute__((__sfr__));
__extension__ typedef struct tagU1ADDRBITS {
  union {
    struct {
      unsigned DEVADDR:7;
      unsigned LOWSPDEN:1;
    };
    struct {
      unsigned DEVADDR0:1;
      unsigned DEVADDR1:1;
      unsigned DEVADDR2:1;
      unsigned DEVADDR3:1;
      unsigned DEVADDR4:1;
      unsigned DEVADDR5:1;
      unsigned DEVADDR6:1;
      unsigned LSPDEN:1;
    };
  };
} U1ADDRBITS;
extern volatile U1ADDRBITS U1ADDRbits __attribute__((__sfr__));

extern volatile unsigned int U1BDTP1 __attribute__((__sfr__));
__extension__ typedef struct tagU1BDTP1BITS {
  union {
    struct {
      unsigned :1;
      unsigned BDTPTRL:7;
    };
    struct {
      unsigned :1;
      unsigned BDTPTRL0:1;
      unsigned BDTPTRL1:1;
      unsigned BDTPTRL2:1;
      unsigned BDTPTRL3:1;
      unsigned BDTPTRL4:1;
      unsigned BDTPTRL5:1;
      unsigned BDTPTRL6:1;
    };
  };
} U1BDTP1BITS;
extern volatile U1BDTP1BITS U1BDTP1bits __attribute__((__sfr__));

extern volatile unsigned int U1FRML __attribute__((__sfr__));
typedef struct tagU1FRMLBITS {
  unsigned FRM0:1;
  unsigned FRM1:1;
  unsigned FRM2:1;
  unsigned FRM3:1;
  unsigned FRM4:1;
  unsigned FRM5:1;
  unsigned FRM6:1;
  unsigned FRM7:1;
} U1FRMLBITS;
extern volatile U1FRMLBITS U1FRMLbits __attribute__((__sfr__));

extern volatile unsigned int U1FRMH __attribute__((__sfr__));
typedef struct tagU1FRMHBITS {
  unsigned FRM8:1;
  unsigned FRM9:1;
  unsigned FRM10:1;
} U1FRMHBITS;
extern volatile U1FRMHBITS U1FRMHbits __attribute__((__sfr__));

extern volatile unsigned int U1TOK __attribute__((__sfr__));
__extension__ typedef struct tagU1TOKBITS {
  union {
    struct {
      unsigned EP:4;
      unsigned PID:4;
    };
    struct {
      unsigned EP0:1;
      unsigned EP1:1;
      unsigned EP2:1;
      unsigned EP3:1;
      unsigned PID0:1;
      unsigned PID1:1;
      unsigned PID2:1;
      unsigned PID3:1;
    };
  };
} U1TOKBITS;
extern volatile U1TOKBITS U1TOKbits __attribute__((__sfr__));

extern volatile unsigned int U1SOF __attribute__((__sfr__));
__extension__ typedef struct tagU1SOFBITS {
  union {
    struct {
      unsigned CNT:8;
    };
    struct {
      unsigned CNT0:1;
      unsigned CNT1:1;
      unsigned CNT2:1;
      unsigned CNT3:1;
      unsigned CNT4:1;
      unsigned CNT5:1;
      unsigned CNT6:1;
      unsigned CNT7:1;
    };
  };
} U1SOFBITS;
extern volatile U1SOFBITS U1SOFbits __attribute__((__sfr__));

extern volatile unsigned int U1CNFG1 __attribute__((__sfr__));
__extension__ typedef struct tagU1CNFG1BITS {
  union {
    struct {
      unsigned PPB:2;
      unsigned :2;
      unsigned USBSIDL:1;
      unsigned :1;
      unsigned UOEMON:1;
      unsigned UTEYE:1;
    };
    struct {
      unsigned PPB0:1;
      unsigned PPB1:1;
    };
  };
} U1CNFG1BITS;
extern volatile U1CNFG1BITS U1CNFG1bits __attribute__((__sfr__));

extern volatile unsigned int U1CNFG2 __attribute__((__sfr__));
typedef struct tagU1CNFG2BITS {
  unsigned UTRDIS:1;
  unsigned UVCMPDIS:1;
  unsigned UVBUSDIS:1;
  unsigned EXTI2CEN:1;
  unsigned PUVBUS:1;
  unsigned UVCMPSEL:1;
} U1CNFG2BITS;
extern volatile U1CNFG2BITS U1CNFG2bits __attribute__((__sfr__));

extern volatile unsigned int U1EP0 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP0BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
      unsigned :1;
      unsigned RETRYDIS:1;
      unsigned LSPD:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
      unsigned :3;
      unsigned LOWSPD:1;
    };
  };
} U1EP0BITS;
extern volatile U1EP0BITS U1EP0bits __attribute__((__sfr__));

extern volatile unsigned int U1EP1 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP1BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP1BITS;
extern volatile U1EP1BITS U1EP1bits __attribute__((__sfr__));

extern volatile unsigned int U1EP2 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP2BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP2BITS;
extern volatile U1EP2BITS U1EP2bits __attribute__((__sfr__));

extern volatile unsigned int U1EP3 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP3BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP3BITS;
extern volatile U1EP3BITS U1EP3bits __attribute__((__sfr__));

extern volatile unsigned int U1EP4 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP4BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP4BITS;
extern volatile U1EP4BITS U1EP4bits __attribute__((__sfr__));

extern volatile unsigned int U1EP5 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP5BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP5BITS;
extern volatile U1EP5BITS U1EP5bits __attribute__((__sfr__));

extern volatile unsigned int U1EP6 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP6BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP6BITS;
extern volatile U1EP6BITS U1EP6bits __attribute__((__sfr__));

extern volatile unsigned int U1EP7 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP7BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP7BITS;
extern volatile U1EP7BITS U1EP7bits __attribute__((__sfr__));

extern volatile unsigned int U1EP8 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP8BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP8BITS;
extern volatile U1EP8BITS U1EP8bits __attribute__((__sfr__));

extern volatile unsigned int U1EP9 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP9BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP9BITS;
extern volatile U1EP9BITS U1EP9bits __attribute__((__sfr__));

extern volatile unsigned int U1EP10 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP10BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP10BITS;
extern volatile U1EP10BITS U1EP10bits __attribute__((__sfr__));

extern volatile unsigned int U1EP11 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP11BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP11BITS;
extern volatile U1EP11BITS U1EP11bits __attribute__((__sfr__));

extern volatile unsigned int U1EP12 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP12BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP12BITS;
extern volatile U1EP12BITS U1EP12bits __attribute__((__sfr__));

extern volatile unsigned int U1EP13 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP13BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP13BITS;
extern volatile U1EP13BITS U1EP13bits __attribute__((__sfr__));

extern volatile unsigned int U1EP14 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP14BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP14BITS;
extern volatile U1EP14BITS U1EP14bits __attribute__((__sfr__));

extern volatile unsigned int U1EP15 __attribute__((__sfr__));
__extension__ typedef struct tagU1EP15BITS {
  union {
    struct {
      unsigned EPHSHK:1;
      unsigned EPSTALL:1;
      unsigned EPTXEN:1;
      unsigned EPRXEN:1;
      unsigned EPCONDIS:1;
    };
    struct {
      unsigned :2;
      unsigned EPINEN:1;
      unsigned EPOUTEN:1;
    };
  };
} U1EP15BITS;
extern volatile U1EP15BITS U1EP15bits __attribute__((__sfr__));

extern volatile unsigned int U1PWMRRS __attribute__((__sfr__));
__extension__ typedef struct tagU1PWMRRSBITS {
  union {
    struct {
      unsigned PER:8;
      unsigned DC:8;
    };
    struct {
      unsigned PER0:1;
      unsigned PER1:1;
      unsigned PER2:1;
      unsigned PER3:1;
      unsigned PER4:1;
      unsigned PER5:1;
      unsigned PER6:1;
      unsigned PER7:1;
      unsigned DC0:1;
      unsigned DC1:1;
      unsigned DC2:1;
      unsigned DC3:1;
      unsigned DC4:1;
      unsigned DC5:1;
      unsigned DC6:1;
      unsigned DC7:1;
    };
    struct {
      unsigned USBRS0:1;
      unsigned USBRS1:1;
      unsigned USBRS2:1;
      unsigned USBRS3:1;
      unsigned USBRS4:1;
      unsigned USBRS5:1;
      unsigned USBRS6:1;
      unsigned USBRS7:1;
      unsigned USBR0:1;
      unsigned USBR1:1;
      unsigned USBR2:1;
      unsigned USBR3:1;
      unsigned USBR4:1;
      unsigned USBR5:1;
      unsigned USBR6:1;
      unsigned USBR7:1;
    };
    struct {
      unsigned USBRS:8;
      unsigned USBR:8;
    };
  };
} U1PWMRRSBITS;
extern volatile U1PWMRRSBITS U1PWMRRSbits __attribute__((__sfr__));

extern volatile unsigned int U1PWMCON __attribute__((__sfr__));
typedef struct tagU1PWMCONBITS {
  unsigned :8;
  unsigned CNTEN:1;
  unsigned PWMPOL:1;
  unsigned :5;
  unsigned PWMEN:1;
} U1PWMCONBITS;
extern volatile U1PWMCONBITS U1PWMCONbits __attribute__((__sfr__));

extern volatile unsigned int ANCFG __attribute__((__sfr__));
typedef struct tagANCFGBITS {
  unsigned VBGEN:1;
  unsigned VBG2EN:1;
  unsigned VBG6EN:1;
} ANCFGBITS;
extern volatile ANCFGBITS ANCFGbits __attribute__((__sfr__));

extern volatile unsigned int ANSB __attribute__((__sfr__));
typedef struct tagANSBBITS {
  unsigned ANSB0:1;
  unsigned ANSB1:1;
  unsigned ANSB2:1;
  unsigned ANSB3:1;
  unsigned ANSB4:1;
  unsigned ANSB5:1;
  unsigned ANSB6:1;
  unsigned ANSB7:1;
  unsigned ANSB8:1;
  unsigned ANSB9:1;
  unsigned ANSB10:1;
  unsigned ANSB11:1;
  unsigned ANSB12:1;
  unsigned ANSB13:1;
  unsigned ANSB14:1;
  unsigned ANSB15:1;
} ANSBBITS;
extern volatile ANSBBITS ANSBbits __attribute__((__sfr__));

extern volatile unsigned int ANSC __attribute__((__sfr__));
typedef struct tagANSCBITS {
  unsigned :13;
  unsigned ANSC13:1;
  unsigned ANSC14:1;
} ANSCBITS;
extern volatile ANSCBITS ANSCbits __attribute__((__sfr__));

extern volatile unsigned int ANSD __attribute__((__sfr__));
typedef struct tagANSDBITS {
  unsigned :6;
  unsigned ANSD6:1;
  unsigned ANSD7:1;
} ANSDBITS;
extern volatile ANSDBITS ANSDbits __attribute__((__sfr__));

extern volatile unsigned int ANSF __attribute__((__sfr__));
typedef struct tagANSFBITS {
  unsigned ANSF0:1;
} ANSFBITS;
extern volatile ANSFBITS ANSFbits __attribute__((__sfr__));

extern volatile unsigned int ANSG __attribute__((__sfr__));
typedef struct tagANSGBITS {
  unsigned :6;
  unsigned ANSG6:1;
  unsigned ANSG7:1;
  unsigned ANSG8:1;
  unsigned ANSG9:1;
} ANSGBITS;
extern volatile ANSGBITS ANSGbits __attribute__((__sfr__));

extern volatile unsigned int ALRMVAL __attribute__((__sfr__));
extern volatile unsigned int ALCFGRPT __attribute__((__sfr__));
__extension__ typedef struct tagALCFGRPTBITS {
  union {
    struct {
      unsigned ARPT:8;
      unsigned ALRMPTR:2;
      unsigned AMASK:4;
      unsigned CHIME:1;
      unsigned ALRMEN:1;
    };
    struct {
      unsigned ARPT0:1;
      unsigned ARPT1:1;
      unsigned ARPT2:1;
      unsigned ARPT3:1;
      unsigned ARPT4:1;
      unsigned ARPT5:1;
      unsigned ARPT6:1;
      unsigned ARPT7:1;
      unsigned ALRMPTR0:1;
      unsigned ALRMPTR1:1;
      unsigned AMASK0:1;
      unsigned AMASK1:1;
      unsigned AMASK2:1;
      unsigned AMASK3:1;
    };
  };
} ALCFGRPTBITS;
extern volatile ALCFGRPTBITS ALCFGRPTbits __attribute__((__sfr__));

extern volatile unsigned int RTCVAL __attribute__((__sfr__));
extern volatile unsigned int RCFGCAL __attribute__((__sfr__));
__extension__ typedef struct tagRCFGCALBITS {
  union {
    struct {
      unsigned CAL:8;
      unsigned RTCPTR:2;
      unsigned RTCOE:1;
      unsigned HALFSEC:1;
      unsigned RTCSYNC:1;
      unsigned RTCWREN:1;
      unsigned :1;
      unsigned RTCEN:1;
    };
    struct {
      unsigned CAL0:1;
      unsigned CAL1:1;
      unsigned CAL2:1;
      unsigned CAL3:1;
      unsigned CAL4:1;
      unsigned CAL5:1;
      unsigned CAL6:1;
      unsigned CAL7:1;
      unsigned RTCPTR0:1;
      unsigned RTCPTR1:1;
    };
  };
} RCFGCALBITS;
extern volatile RCFGCALBITS RCFGCALbits __attribute__((__sfr__));

extern volatile unsigned int CMSTAT __attribute__((__sfr__));
typedef struct tagCMSTATBITS {
  unsigned C1OUT:1;
  unsigned C2OUT:1;
  unsigned C3OUT:1;
  unsigned :5;
  unsigned C1EVT:1;
  unsigned C2EVT:1;
  unsigned C3EVT:1;
  unsigned :4;
  unsigned CMIDL:1;
} CMSTATBITS;
extern volatile CMSTATBITS CMSTATbits __attribute__((__sfr__));

extern volatile unsigned int CVRCON __attribute__((__sfr__));
__extension__ typedef struct tagCVRCONBITS {
  union {
    struct {
      unsigned CVR:4;
      unsigned CVRSS:1;
      unsigned CVRR:1;
      unsigned CVROE:1;
      unsigned CVREN:1;
      unsigned CVREFM:2;
      unsigned CVREFP:1;
    };
    struct {
      unsigned CVR0:1;
      unsigned CVR1:1;
      unsigned CVR2:1;
      unsigned CVR3:1;
      unsigned :4;
      unsigned CVREFM0:1;
      unsigned CVREFM1:1;
    };
  };
} CVRCONBITS;
extern volatile CVRCONBITS CVRCONbits __attribute__((__sfr__));

extern volatile unsigned int CM1CON __attribute__((__sfr__));
__extension__ typedef struct tagCM1CONBITS {
  union {
    struct {
      unsigned CCH:2;
      unsigned :2;
      unsigned CREF:1;
      unsigned :1;
      unsigned EVPOL:2;
      unsigned COUT:1;
      unsigned CEVT:1;
      unsigned :3;
      unsigned CPOL:1;
      unsigned COE:1;
      unsigned CON:1;
    };
    struct {
      unsigned CCH0:1;
      unsigned CCH1:1;
      unsigned :4;
      unsigned EVPOL0:1;
      unsigned EVPOL1:1;
      unsigned :7;
      unsigned CEN:1;
    };
  };
} CM1CONBITS;
extern volatile CM1CONBITS CM1CONbits __attribute__((__sfr__));

extern volatile unsigned int CM2CON __attribute__((__sfr__));
__extension__ typedef struct tagCM2CONBITS {
  union {
    struct {
      unsigned CCH:2;
      unsigned :2;
      unsigned CREF:1;
      unsigned :1;
      unsigned EVPOL:2;
      unsigned COUT:1;
      unsigned CEVT:1;
      unsigned :3;
      unsigned CPOL:1;
      unsigned COE:1;
      unsigned CON:1;
    };
    struct {
      unsigned CCH0:1;
      unsigned CCH1:1;
      unsigned :4;
      unsigned EVPOL0:1;
      unsigned EVPOL1:1;
      unsigned :7;
      unsigned CEN:1;
    };
  };
} CM2CONBITS;
extern volatile CM2CONBITS CM2CONbits __attribute__((__sfr__));

extern volatile unsigned int CM3CON __attribute__((__sfr__));
__extension__ typedef struct tagCM3CONBITS {
  union {
    struct {
      unsigned CCH:2;
      unsigned :2;
      unsigned CREF:1;
      unsigned :1;
      unsigned EVPOL:2;
      unsigned COUT:1;
      unsigned CEVT:1;
      unsigned :3;
      unsigned CPOL:1;
      unsigned COE:1;
      unsigned CON:1;
    };
    struct {
      unsigned CCH0:1;
      unsigned CCH1:1;
      unsigned :4;
      unsigned EVPOL0:1;
      unsigned EVPOL1:1;
      unsigned :7;
      unsigned CEN:1;
    };
  };
} CM3CONBITS;
extern volatile CM3CONBITS CM3CONbits __attribute__((__sfr__));

extern volatile unsigned int CRCCON1 __attribute__((__sfr__));
__extension__ typedef struct tagCRCCON1BITS {
  union {
    struct {
      unsigned :3;
      unsigned LENDIAN:1;
      unsigned CRCGO:1;
      unsigned CRCISEL:1;
      unsigned CRCMPT:1;
      unsigned CRCFUL:1;
      unsigned VWORD:5;
      unsigned CSIDL:1;
      unsigned :1;
      unsigned CRCEN:1;
    };
    struct {
      unsigned :8;
      unsigned VWORD0:1;
      unsigned VWORD1:1;
      unsigned VWORD2:1;
      unsigned VWORD3:1;
      unsigned VWORD4:1;
    };
  };
} CRCCON1BITS;
extern volatile CRCCON1BITS CRCCON1bits __attribute__((__sfr__));

extern volatile unsigned int CRCCON2 __attribute__((__sfr__));
__extension__ typedef struct tagCRCCON2BITS {
  union {
    struct {
      unsigned PLEN:5;
      unsigned :3;
      unsigned DWIDTH:5;
    };
    struct {
      unsigned PLEN0:1;
      unsigned PLEN1:1;
      unsigned PLEN2:1;
      unsigned PLEN3:1;
      unsigned PLEN4:1;
      unsigned :3;
      unsigned DWIDTH0:1;
      unsigned DWIDTH1:1;
      unsigned DWIDTH2:1;
      unsigned DWIDTH3:1;
      unsigned DWIDTH4:1;
    };
  };
} CRCCON2BITS;
extern volatile CRCCON2BITS CRCCON2bits __attribute__((__sfr__));

extern volatile unsigned int CRCXORL __attribute__((__sfr__));
typedef struct tagCRCXORLBITS {
  unsigned :1;
  unsigned X1:1;
  unsigned X2:1;
  unsigned X3:1;
  unsigned X4:1;
  unsigned X5:1;
  unsigned X6:1;
  unsigned X7:1;
  unsigned X8:1;
  unsigned X9:1;
  unsigned X10:1;
  unsigned X11:1;
  unsigned X12:1;
  unsigned X13:1;
  unsigned X14:1;
  unsigned X15:1;
} CRCXORLBITS;
extern volatile CRCXORLBITS CRCXORLbits __attribute__((__sfr__));

extern volatile unsigned int CRCXORH __attribute__((__sfr__));
typedef struct tagCRCXORHBITS {
  unsigned X16:1;
  unsigned X17:1;
  unsigned X18:1;
  unsigned X19:1;
  unsigned X20:1;
  unsigned X21:1;
  unsigned X22:1;
  unsigned X23:1;
  unsigned X24:1;
  unsigned X25:1;
  unsigned X26:1;
  unsigned X27:1;
  unsigned X28:1;
  unsigned X29:1;
  unsigned X30:1;
  unsigned X31:1;
} CRCXORHBITS;
extern volatile CRCXORHBITS CRCXORHbits __attribute__((__sfr__));

extern volatile unsigned int CRCDAT __attribute__((__sfr__));
extern volatile unsigned long CRCDAT32 __attribute__((__sfr__));
extern volatile unsigned char CRCDAT8 __attribute__((__sfr__));
extern volatile unsigned int CRCDATL __attribute__((__sfr__));
extern volatile unsigned int CRCDATH __attribute__((__sfr__));
extern volatile unsigned int CRCWDATL __attribute__((__sfr__));
extern volatile unsigned int CRCWDATH __attribute__((__sfr__));
extern volatile unsigned int RPINR0 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR0BITS {
  union {
    struct {
      unsigned :8;
      unsigned INT1R:6;
    };
    struct {
      unsigned :8;
      unsigned INT1R0:1;
      unsigned INT1R1:1;
      unsigned INT1R2:1;
      unsigned INT1R3:1;
      unsigned INT1R4:1;
      unsigned INT1R5:1;
    };
  };
} RPINR0BITS;
extern volatile RPINR0BITS RPINR0bits __attribute__((__sfr__));

extern volatile unsigned int RPINR1 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR1BITS {
  union {
    struct {
      unsigned INT2R:6;
      unsigned :2;
      unsigned INT3R:6;
    };
    struct {
      unsigned INT2R0:1;
      unsigned INT2R1:1;
      unsigned INT2R2:1;
      unsigned INT2R3:1;
      unsigned INT2R4:1;
      unsigned INT2R5:1;
      unsigned :2;
      unsigned INT3R0:1;
      unsigned INT3R1:1;
      unsigned INT3R2:1;
      unsigned INT3R3:1;
      unsigned INT3R4:1;
      unsigned INT3R5:1;
    };
  };
} RPINR1BITS;
extern volatile RPINR1BITS RPINR1bits __attribute__((__sfr__));

extern volatile unsigned int RPINR2 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR2BITS {
  union {
    struct {
      unsigned INT4R:6;
    };
    struct {
      unsigned INT4R0:1;
      unsigned INT4R1:1;
      unsigned INT4R2:1;
      unsigned INT4R3:1;
      unsigned INT4R4:1;
      unsigned INT4R5:1;
    };
  };
} RPINR2BITS;
extern volatile RPINR2BITS RPINR2bits __attribute__((__sfr__));

extern volatile unsigned int RPINR3 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR3BITS {
  union {
    struct {
      unsigned T2CKR:6;
      unsigned :2;
      unsigned T3CKR:6;
    };
    struct {
      unsigned T2CKR0:1;
      unsigned T2CKR1:1;
      unsigned T2CKR2:1;
      unsigned T2CKR3:1;
      unsigned T2CKR4:1;
      unsigned T2CKR5:1;
      unsigned :2;
      unsigned T3CKR0:1;
      unsigned T3CKR1:1;
      unsigned T3CKR2:1;
      unsigned T3CKR3:1;
      unsigned T3CKR4:1;
      unsigned T3CKR5:1;
    };
  };
} RPINR3BITS;
extern volatile RPINR3BITS RPINR3bits __attribute__((__sfr__));

extern volatile unsigned int RPINR4 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR4BITS {
  union {
    struct {
      unsigned T4CKR:6;
      unsigned :2;
      unsigned T5CKR:6;
    };
    struct {
      unsigned T4CKR0:1;
      unsigned T4CKR1:1;
      unsigned T4CKR2:1;
      unsigned T4CKR3:1;
      unsigned T4CKR4:1;
      unsigned T4CKR5:1;
      unsigned :2;
      unsigned T5CKR0:1;
      unsigned T5CKR1:1;
      unsigned T5CKR2:1;
      unsigned T5CKR3:1;
      unsigned T5CKR4:1;
      unsigned T5CKR5:1;
    };
  };
} RPINR4BITS;
extern volatile RPINR4BITS RPINR4bits __attribute__((__sfr__));

extern volatile unsigned int RPINR7 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR7BITS {
  union {
    struct {
      unsigned IC1R:6;
      unsigned :2;
      unsigned IC2R:6;
    };
    struct {
      unsigned IC1R0:1;
      unsigned IC1R1:1;
      unsigned IC1R2:1;
      unsigned IC1R3:1;
      unsigned IC1R4:1;
      unsigned IC1R5:1;
      unsigned :2;
      unsigned IC2R0:1;
      unsigned IC2R1:1;
      unsigned IC2R2:1;
      unsigned IC2R3:1;
      unsigned IC2R4:1;
      unsigned IC2R5:1;
    };
  };
} RPINR7BITS;
extern volatile RPINR7BITS RPINR7bits __attribute__((__sfr__));

extern volatile unsigned int RPINR8 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR8BITS {
  union {
    struct {
      unsigned IC3R:6;
      unsigned :2;
      unsigned IC4R:6;
    };
    struct {
      unsigned IC3R0:1;
      unsigned IC3R1:1;
      unsigned IC3R2:1;
      unsigned IC3R3:1;
      unsigned IC3R4:1;
      unsigned IC3R5:1;
      unsigned :2;
      unsigned IC4R0:1;
      unsigned IC4R1:1;
      unsigned IC4R2:1;
      unsigned IC4R3:1;
      unsigned IC4R4:1;
      unsigned IC4R5:1;
    };
  };
} RPINR8BITS;
extern volatile RPINR8BITS RPINR8bits __attribute__((__sfr__));

extern volatile unsigned int RPINR9 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR9BITS {
  union {
    struct {
      unsigned IC5R:6;
      unsigned :2;
      unsigned IC6R:6;
    };
    struct {
      unsigned IC5R0:1;
      unsigned IC5R1:1;
      unsigned IC5R2:1;
      unsigned IC5R3:1;
      unsigned IC5R4:1;
      unsigned IC5R5:1;
      unsigned :2;
      unsigned IC6R0:1;
      unsigned IC6R1:1;
      unsigned IC6R2:1;
      unsigned IC6R3:1;
      unsigned IC6R4:1;
      unsigned IC6R5:1;
    };
  };
} RPINR9BITS;
extern volatile RPINR9BITS RPINR9bits __attribute__((__sfr__));

extern volatile unsigned int RPINR10 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR10BITS {
  union {
    struct {
      unsigned IC7R:6;
      unsigned :2;
      unsigned IC8R:6;
    };
    struct {
      unsigned IC7R0:1;
      unsigned IC7R1:1;
      unsigned IC7R2:1;
      unsigned IC7R3:1;
      unsigned IC7R4:1;
      unsigned IC7R5:1;
      unsigned :2;
      unsigned IC8R0:1;
      unsigned IC8R1:1;
      unsigned IC8R2:1;
      unsigned IC8R3:1;
      unsigned IC8R4:1;
      unsigned IC8R5:1;
    };
  };
} RPINR10BITS;
extern volatile RPINR10BITS RPINR10bits __attribute__((__sfr__));

extern volatile unsigned int RPINR11 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR11BITS {
  union {
    struct {
      unsigned OCFAR:6;
      unsigned :2;
      unsigned OCFBR:6;
    };
    struct {
      unsigned OCFAR0:1;
      unsigned OCFAR1:1;
      unsigned OCFAR2:1;
      unsigned OCFAR3:1;
      unsigned OCFAR4:1;
      unsigned OCFAR5:1;
      unsigned :2;
      unsigned OCFBR0:1;
      unsigned OCFBR1:1;
      unsigned OCFBR2:1;
      unsigned OCFBR3:1;
      unsigned OCFBR4:1;
      unsigned OCFBR5:1;
    };
  };
} RPINR11BITS;
extern volatile RPINR11BITS RPINR11bits __attribute__((__sfr__));

extern volatile unsigned int RPINR15 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR15BITS {
  union {
    struct {
      unsigned :8;
      unsigned IC9R:6;
    };
    struct {
      unsigned :8;
      unsigned IC9R0:1;
      unsigned IC9R1:1;
      unsigned IC9R2:1;
      unsigned IC9R3:1;
      unsigned IC9R4:1;
      unsigned IC9R5:1;
    };
  };
} RPINR15BITS;
extern volatile RPINR15BITS RPINR15bits __attribute__((__sfr__));

extern volatile unsigned int RPINR17 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR17BITS {
  union {
    struct {
      unsigned :8;
      unsigned U3RXR:6;
    };
    struct {
      unsigned :8;
      unsigned U3RXR0:1;
      unsigned U3RXR1:1;
      unsigned U3RXR2:1;
      unsigned U3RXR3:1;
      unsigned U3RXR4:1;
      unsigned U3RXR5:1;
    };
  };
} RPINR17BITS;
extern volatile RPINR17BITS RPINR17bits __attribute__((__sfr__));

extern volatile unsigned int RPINR18 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR18BITS {
  union {
    struct {
      unsigned U1RXR:6;
      unsigned :2;
      unsigned U1CTSR:6;
    };
    struct {
      unsigned U1RXR0:1;
      unsigned U1RXR1:1;
      unsigned U1RXR2:1;
      unsigned U1RXR3:1;
      unsigned U1RXR4:1;
      unsigned U1RXR5:1;
      unsigned :2;
      unsigned U1CTSR0:1;
      unsigned U1CTSR1:1;
      unsigned U1CTSR2:1;
      unsigned U1CTSR3:1;
      unsigned U1CTSR4:1;
      unsigned U1CTSR5:1;
    };
  };
} RPINR18BITS;
extern volatile RPINR18BITS RPINR18bits __attribute__((__sfr__));

extern volatile unsigned int RPINR19 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR19BITS {
  union {
    struct {
      unsigned U2RXR:6;
      unsigned :2;
      unsigned U2CTSR:6;
    };
    struct {
      unsigned U2RXR0:1;
      unsigned U2RXR1:1;
      unsigned U2RXR2:1;
      unsigned U2RXR3:1;
      unsigned U2RXR4:1;
      unsigned U2RXR5:1;
      unsigned :2;
      unsigned U2CTSR0:1;
      unsigned U2CTSR1:1;
      unsigned U2CTSR2:1;
      unsigned U2CTSR3:1;
      unsigned U2CTSR4:1;
      unsigned U2CTSR5:1;
    };
  };
} RPINR19BITS;
extern volatile RPINR19BITS RPINR19bits __attribute__((__sfr__));

extern volatile unsigned int RPINR20 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR20BITS {
  union {
    struct {
      unsigned SDI1R:6;
      unsigned :2;
      unsigned SCK1R:6;
    };
    struct {
      unsigned SDI1R0:1;
      unsigned SDI1R1:1;
      unsigned SDI1R2:1;
      unsigned SDI1R3:1;
      unsigned SDI1R4:1;
      unsigned SDI1R5:1;
      unsigned :2;
      unsigned SCK1R0:1;
      unsigned SCK1R1:1;
      unsigned SCK1R2:1;
      unsigned SCK1R3:1;
      unsigned SCK1R4:1;
      unsigned SCK1R5:1;
    };
  };
} RPINR20BITS;
extern volatile RPINR20BITS RPINR20bits __attribute__((__sfr__));

extern volatile unsigned int RPINR21 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR21BITS {
  union {
    struct {
      unsigned SS1R:6;
      unsigned :2;
      unsigned U3CTSR:6;
    };
    struct {
      unsigned SS1R0:1;
      unsigned SS1R1:1;
      unsigned SS1R2:1;
      unsigned SS1R3:1;
      unsigned SS1R4:1;
      unsigned SS1R5:1;
      unsigned :2;
      unsigned U3CTSR0:1;
      unsigned U3CTSR1:1;
      unsigned U3CTSR2:1;
      unsigned U3CTSR3:1;
      unsigned U3CTSR4:1;
      unsigned U3CTSR5:1;
    };
  };
} RPINR21BITS;
extern volatile RPINR21BITS RPINR21bits __attribute__((__sfr__));

extern volatile unsigned int RPINR22 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR22BITS {
  union {
    struct {
      unsigned SDI2R:6;
      unsigned :2;
      unsigned SCK2R:6;
    };
    struct {
      unsigned SDI2R0:1;
      unsigned SDI2R1:1;
      unsigned SDI2R2:1;
      unsigned SDI2R3:1;
      unsigned SDI2R4:1;
      unsigned SDI2R5:1;
      unsigned :2;
      unsigned SCK2R0:1;
      unsigned SCK2R1:1;
      unsigned SCK2R2:1;
      unsigned SCK2R3:1;
      unsigned SCK2R4:1;
      unsigned SCK2R5:1;
    };
  };
} RPINR22BITS;
extern volatile RPINR22BITS RPINR22bits __attribute__((__sfr__));

extern volatile unsigned int RPINR23 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR23BITS {
  union {
    struct {
      unsigned SS2R:6;
    };
    struct {
      unsigned SS2R0:1;
      unsigned SS2R1:1;
      unsigned SS2R2:1;
      unsigned SS2R3:1;
      unsigned SS2R4:1;
      unsigned SS2R5:1;
    };
  };
} RPINR23BITS;
extern volatile RPINR23BITS RPINR23bits __attribute__((__sfr__));

extern volatile unsigned int RPINR27 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR27BITS {
  union {
    struct {
      unsigned U4RXR:6;
      unsigned :2;
      unsigned U4CTSR:6;
    };
    struct {
      unsigned U4RXR0:1;
      unsigned U4RXR1:1;
      unsigned U4RXR2:1;
      unsigned U4RXR3:1;
      unsigned U4RXR4:1;
      unsigned U4RXR5:1;
      unsigned :2;
      unsigned U4CTSR0:1;
      unsigned U4CTSR1:1;
      unsigned U4CTSR2:1;
      unsigned U4CTSR3:1;
      unsigned U4CTSR4:1;
      unsigned U4CTSR5:1;
    };
  };
} RPINR27BITS;
extern volatile RPINR27BITS RPINR27bits __attribute__((__sfr__));

extern volatile unsigned int RPINR28 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR28BITS {
  union {
    struct {
      unsigned SDI3R:6;
      unsigned :2;
      unsigned SCK3R:6;
    };
    struct {
      unsigned SDI3R0:1;
      unsigned SDI3R1:1;
      unsigned SDI3R2:1;
      unsigned SDI3R3:1;
      unsigned SDI3R4:1;
      unsigned SDI3R5:1;
      unsigned :2;
      unsigned SCK3R0:1;
      unsigned SCK3R1:1;
      unsigned SCK3R2:1;
      unsigned SCK3R3:1;
      unsigned SCK3R4:1;
      unsigned SCK3R5:1;
    };
  };
} RPINR28BITS;
extern volatile RPINR28BITS RPINR28bits __attribute__((__sfr__));

extern volatile unsigned int RPINR29 __attribute__((__sfr__));
__extension__ typedef struct tagRPINR29BITS {
  union {
    struct {
      unsigned SS3R:6;
    };
    struct {
      unsigned SS3R0:1;
      unsigned SS3R1:1;
      unsigned SS3R2:1;
      unsigned SS3R3:1;
      unsigned SS3R4:1;
      unsigned SS3R5:1;
    };
  };
} RPINR29BITS;
extern volatile RPINR29BITS RPINR29bits __attribute__((__sfr__));

extern volatile unsigned int RPOR0 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR0BITS {
  union {
    struct {
      unsigned RP0R:6;
      unsigned :2;
      unsigned RP1R:6;
    };
    struct {
      unsigned RP0R0:1;
      unsigned RP0R1:1;
      unsigned RP0R2:1;
      unsigned RP0R3:1;
      unsigned RP0R4:1;
      unsigned RP0R5:1;
      unsigned :2;
      unsigned RP1R0:1;
      unsigned RP1R1:1;
      unsigned RP1R2:1;
      unsigned RP1R3:1;
      unsigned RP1R4:1;
      unsigned RP1R5:1;
    };
  };
} RPOR0BITS;
extern volatile RPOR0BITS RPOR0bits __attribute__((__sfr__));

extern volatile unsigned int RPOR1 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR1BITS {
  union {
    struct {
      unsigned RP2R:6;
      unsigned :2;
      unsigned RP3R:6;
    };
    struct {
      unsigned RP2R0:1;
      unsigned RP2R1:1;
      unsigned RP2R2:1;
      unsigned RP2R3:1;
      unsigned RP2R4:1;
      unsigned RP2R5:1;
      unsigned :2;
      unsigned RP3R0:1;
      unsigned RP3R1:1;
      unsigned RP3R2:1;
      unsigned RP3R3:1;
      unsigned RP3R4:1;
      unsigned RP3R5:1;
    };
  };
} RPOR1BITS;
extern volatile RPOR1BITS RPOR1bits __attribute__((__sfr__));

extern volatile unsigned int RPOR2 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR2BITS {
  union {
    struct {
      unsigned RP4R:6;
    };
    struct {
      unsigned RP4R0:1;
      unsigned RP4R1:1;
      unsigned RP4R2:1;
      unsigned RP4R3:1;
      unsigned RP4R4:1;
      unsigned RP4R5:1;
    };
  };
} RPOR2BITS;
extern volatile RPOR2BITS RPOR2bits __attribute__((__sfr__));

extern volatile unsigned int RPOR3 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR3BITS {
  union {
    struct {
      unsigned RP6R:6;
      unsigned :2;
      unsigned RP7R:6;
    };
    struct {
      unsigned RP6R0:1;
      unsigned RP6R1:1;
      unsigned RP6R2:1;
      unsigned RP6R3:1;
      unsigned RP6R4:1;
      unsigned RP6R5:1;
      unsigned :2;
      unsigned RP7R0:1;
      unsigned RP7R1:1;
      unsigned RP7R2:1;
      unsigned RP7R3:1;
      unsigned RP7R4:1;
      unsigned RP7R5:1;
    };
  };
} RPOR3BITS;
extern volatile RPOR3BITS RPOR3bits __attribute__((__sfr__));

extern volatile unsigned int RPOR4 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR4BITS {
  union {
    struct {
      unsigned RP8R:6;
      unsigned :2;
      unsigned RP9R:6;
    };
    struct {
      unsigned RP8R0:1;
      unsigned RP8R1:1;
      unsigned RP8R2:1;
      unsigned RP8R3:1;
      unsigned RP8R4:1;
      unsigned RP8R5:1;
      unsigned :2;
      unsigned RP9R0:1;
      unsigned RP9R1:1;
      unsigned RP9R2:1;
      unsigned RP9R3:1;
      unsigned RP9R4:1;
      unsigned RP9R5:1;
    };
  };
} RPOR4BITS;
extern volatile RPOR4BITS RPOR4bits __attribute__((__sfr__));

extern volatile unsigned int RPOR5 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR5BITS {
  union {
    struct {
      unsigned RP10R:6;
      unsigned :2;
      unsigned RP11R:6;
    };
    struct {
      unsigned RP10R0:1;
      unsigned RP10R1:1;
      unsigned RP10R2:1;
      unsigned RP10R3:1;
      unsigned RP10R4:1;
      unsigned RP10R5:1;
      unsigned :2;
      unsigned RP11R0:1;
      unsigned RP11R1:1;
      unsigned RP11R2:1;
      unsigned RP11R3:1;
      unsigned RP11R4:1;
      unsigned RP11R5:1;
    };
  };
} RPOR5BITS;
extern volatile RPOR5BITS RPOR5bits __attribute__((__sfr__));

extern volatile unsigned int RPOR6 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR6BITS {
  union {
    struct {
      unsigned RP12R:6;
      unsigned :2;
      unsigned RP13R:6;
    };
    struct {
      unsigned RP12R0:1;
      unsigned RP12R1:1;
      unsigned RP12R2:1;
      unsigned RP12R3:1;
      unsigned RP12R4:1;
      unsigned RP12R5:1;
      unsigned :2;
      unsigned RP13R0:1;
      unsigned RP13R1:1;
      unsigned RP13R2:1;
      unsigned RP13R3:1;
      unsigned RP13R4:1;
      unsigned RP13R5:1;
    };
  };
} RPOR6BITS;
extern volatile RPOR6BITS RPOR6bits __attribute__((__sfr__));

extern volatile unsigned int RPOR7 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR7BITS {
  union {
    struct {
      unsigned RP14R:6;
    };
    struct {
      unsigned RP14R0:1;
      unsigned RP14R1:1;
      unsigned RP14R2:1;
      unsigned RP14R3:1;
      unsigned RP14R4:1;
      unsigned RP14R5:1;
    };
  };
} RPOR7BITS;
extern volatile RPOR7BITS RPOR7bits __attribute__((__sfr__));

extern volatile unsigned int RPOR8 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR8BITS {
  union {
    struct {
      unsigned RP16R:6;
      unsigned :2;
      unsigned RP17R:6;
    };
    struct {
      unsigned RP16R0:1;
      unsigned RP16R1:1;
      unsigned RP16R2:1;
      unsigned RP16R3:1;
      unsigned RP16R4:1;
      unsigned RP16R5:1;
      unsigned :2;
      unsigned RP17R0:1;
      unsigned RP17R1:1;
      unsigned RP17R2:1;
      unsigned RP17R3:1;
      unsigned RP17R4:1;
      unsigned RP17R5:1;
    };
  };
} RPOR8BITS;
extern volatile RPOR8BITS RPOR8bits __attribute__((__sfr__));

extern volatile unsigned int RPOR9 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR9BITS {
  union {
    struct {
      unsigned RP18R:6;
      unsigned :2;
      unsigned RP19R:6;
    };
    struct {
      unsigned RP18R0:1;
      unsigned RP18R1:1;
      unsigned RP18R2:1;
      unsigned RP18R3:1;
      unsigned RP18R4:1;
      unsigned RP18R5:1;
      unsigned :2;
      unsigned RP19R0:1;
      unsigned RP19R1:1;
      unsigned RP19R2:1;
      unsigned RP19R3:1;
      unsigned RP19R4:1;
      unsigned RP19R5:1;
    };
  };
} RPOR9BITS;
extern volatile RPOR9BITS RPOR9bits __attribute__((__sfr__));

extern volatile unsigned int RPOR10 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR10BITS {
  union {
    struct {
      unsigned RP20R:6;
      unsigned :2;
      unsigned RP21R:6;
    };
    struct {
      unsigned RP20R0:1;
      unsigned RP20R1:1;
      unsigned RP20R2:1;
      unsigned RP20R3:1;
      unsigned RP20R4:1;
      unsigned RP20R5:1;
      unsigned :2;
      unsigned RP21R0:1;
      unsigned RP21R1:1;
      unsigned RP21R2:1;
      unsigned RP21R3:1;
      unsigned RP21R4:1;
      unsigned RP21R5:1;
    };
  };
} RPOR10BITS;
extern volatile RPOR10BITS RPOR10bits __attribute__((__sfr__));

extern volatile unsigned int RPOR11 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR11BITS {
  union {
    struct {
      unsigned RP22R:6;
      unsigned :2;
      unsigned RP23R:6;
    };
    struct {
      unsigned RP22R0:1;
      unsigned RP22R1:1;
      unsigned RP22R2:1;
      unsigned RP22R3:1;
      unsigned RP22R4:1;
      unsigned RP22R5:1;
      unsigned :2;
      unsigned RP23R0:1;
      unsigned RP23R1:1;
      unsigned RP23R2:1;
      unsigned RP23R3:1;
      unsigned RP23R4:1;
      unsigned RP23R5:1;
    };
  };
} RPOR11BITS;
extern volatile RPOR11BITS RPOR11bits __attribute__((__sfr__));

extern volatile unsigned int RPOR12 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR12BITS {
  union {
    struct {
      unsigned RP24R:6;
      unsigned :2;
      unsigned RP25R:6;
    };
    struct {
      unsigned RP24R0:1;
      unsigned RP24R1:1;
      unsigned RP24R2:1;
      unsigned RP24R3:1;
      unsigned RP24R4:1;
      unsigned RP24R5:1;
      unsigned :2;
      unsigned RP25R0:1;
      unsigned RP25R1:1;
      unsigned RP25R2:1;
      unsigned RP25R3:1;
      unsigned RP25R4:1;
      unsigned RP25R5:1;
    };
  };
} RPOR12BITS;
extern volatile RPOR12BITS RPOR12bits __attribute__((__sfr__));

extern volatile unsigned int RPOR13 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR13BITS {
  union {
    struct {
      unsigned RP26R:6;
      unsigned :2;
      unsigned RP27R:6;
    };
    struct {
      unsigned RP26R0:1;
      unsigned RP26R1:1;
      unsigned RP26R2:1;
      unsigned RP26R3:1;
      unsigned RP26R4:1;
      unsigned RP26R5:1;
      unsigned :2;
      unsigned RP27R0:1;
      unsigned RP27R1:1;
      unsigned RP27R2:1;
      unsigned RP27R3:1;
      unsigned RP27R4:1;
      unsigned RP27R5:1;
    };
  };
} RPOR13BITS;
extern volatile RPOR13BITS RPOR13bits __attribute__((__sfr__));

extern volatile unsigned int RPOR14 __attribute__((__sfr__));
__extension__ typedef struct tagRPOR14BITS {
  union {
    struct {
      unsigned RP28R:6;
      unsigned :2;
      unsigned RP29R:6;
    };
    struct {
      unsigned RP28R0:1;
      unsigned RP28R1:1;
      unsigned RP28R2:1;
      unsigned RP28R3:1;
      unsigned RP28R4:1;
      unsigned RP28R5:1;
      unsigned :2;
      unsigned RP29R0:1;
      unsigned RP29R1:1;
      unsigned RP29R2:1;
      unsigned RP29R3:1;
      unsigned RP29R4:1;
      unsigned RP29R5:1;
    };
  };
} RPOR14BITS;
extern volatile RPOR14BITS RPOR14bits __attribute__((__sfr__));

extern volatile unsigned int G1CMDL __attribute__((__sfr__));
typedef struct tagG1CMDLBITS {
  unsigned GCMD0:1;
  unsigned GCMD1:1;
  unsigned GCMD2:1;
  unsigned GCMD3:1;
  unsigned GCMD4:1;
  unsigned GCMD5:1;
  unsigned GCMD6:1;
  unsigned GCMD7:1;
  unsigned GCMD8:1;
  unsigned GCMD9:1;
  unsigned GCMD10:1;
  unsigned GCMD11:1;
  unsigned GCMD12:1;
  unsigned GCMD13:1;
  unsigned GCMD14:1;
  unsigned GCMD15:1;
} G1CMDLBITS;
extern volatile G1CMDLBITS G1CMDLbits __attribute__((__sfr__));

extern volatile unsigned int G1CMDH __attribute__((__sfr__));
typedef struct tagG1CMDHBITS {
  unsigned GCMD16:1;
  unsigned GCMD17:1;
  unsigned GCMD18:1;
  unsigned GCMD19:1;
  unsigned GCMD20:1;
  unsigned GCMD21:1;
  unsigned GCMD22:1;
  unsigned GCMD23:1;
  unsigned GCMD24:1;
  unsigned GCMD25:1;
  unsigned GCMD26:1;
  unsigned GCMD27:1;
  unsigned GCMD28:1;
  unsigned GCMD29:1;
  unsigned GCMD30:1;
  unsigned GCMD31:1;
} G1CMDHBITS;
extern volatile G1CMDHBITS G1CMDHbits __attribute__((__sfr__));

extern volatile unsigned int G1CON1 __attribute__((__sfr__));
__extension__ typedef struct tagG1CON1BITS {
  union {
    struct {
      unsigned GCMDCNT:5;
      unsigned PUBPP:3;
      unsigned GCMDWMK:5;
      unsigned G1SIDL:1;
      unsigned :1;
      unsigned G1EN:1;
    };
    struct {
      unsigned GCMDCNT0:1;
      unsigned GCMDCNT1:1;
      unsigned GCMDCNT2:1;
      unsigned GCMDCNT3:1;
      unsigned GCMDCNT4:1;
      unsigned PUBPP0:1;
      unsigned PUBPP1:1;
      unsigned PUBPP2:1;
      unsigned GCMDWMK0:1;
      unsigned GCMDWMK1:1;
      unsigned GCMDWMK2:1;
      unsigned GCMDWMK3:1;
      unsigned GCMDWMK4:1;
    };
  };
} G1CON1BITS;
extern volatile G1CON1BITS G1CON1bits __attribute__((__sfr__));

extern volatile unsigned int G1STAT __attribute__((__sfr__));
typedef struct tagG1STATBITS {
  unsigned CMDMPT:1;
  unsigned CMDFUL:1;
  unsigned CMDLV:1;
  unsigned HMRGN:1;
  unsigned VMRGN:1;
  unsigned CHRBUSY:1;
  unsigned RCCBUSY:1;
  unsigned IPUBUSY:1;
  unsigned :7;
  unsigned PUBUSY:1;
} G1STATBITS;
extern volatile G1STATBITS G1STATbits __attribute__((__sfr__));

extern volatile unsigned int G1IE __attribute__((__sfr__));
typedef struct tagG1IEBITS {
  unsigned CMDMPTIE:1;
  unsigned CMDFULIE:1;
  unsigned CMDLVIE:1;
  unsigned HMRGNIE:1;
  unsigned VMRGNIE:1;
  unsigned CHRIE:1;
  unsigned RCCIE:1;
  unsigned IPUIE:1;
  unsigned :7;
  unsigned PUIE:1;
} G1IEBITS;
extern volatile G1IEBITS G1IEbits __attribute__((__sfr__));

extern volatile unsigned int G1IR __attribute__((__sfr__));
typedef struct tagG1IRBITS {
  unsigned CMDMPTIF:1;
  unsigned CMDFULIF:1;
  unsigned CMDLVIF:1;
  unsigned HMRGNIF:1;
  unsigned VMRGNIF:1;
  unsigned CHRIF:1;
  unsigned RCCIF:1;
  unsigned IPUIF:1;
  unsigned :7;
  unsigned PUIF:1;
} G1IRBITS;
extern volatile G1IRBITS G1IRbits __attribute__((__sfr__));

extern volatile unsigned int G1W1ADRL __attribute__((__sfr__));
typedef struct tagG1W1ADRLBITS {
  unsigned W1ADR0:1;
  unsigned W1ADR1:1;
  unsigned W1ADR2:1;
  unsigned W1ADR3:1;
  unsigned W1ADR4:1;
  unsigned W1ADR5:1;
  unsigned W1ADR6:1;
  unsigned W1ADR7:1;
  unsigned W1ADR8:1;
  unsigned W1ADR9:1;
  unsigned W1ADR10:1;
  unsigned W1ADR11:1;
  unsigned W1ADR12:1;
  unsigned W1ADR13:1;
  unsigned W1ADR14:1;
  unsigned W1ADR15:1;
} G1W1ADRLBITS;
extern volatile G1W1ADRLBITS G1W1ADRLbits __attribute__((__sfr__));

extern volatile unsigned int G1W1ADRH __attribute__((__sfr__));
typedef struct tagG1W1ADRHBITS {
  unsigned W1ADR16:1;
  unsigned W1ADR17:1;
  unsigned W1ADR18:1;
  unsigned W1ADR19:1;
  unsigned W1ADR20:1;
  unsigned W1ADR21:1;
  unsigned W1ADR22:1;
  unsigned W1ADR23:1;
} G1W1ADRHBITS;
extern volatile G1W1ADRHBITS G1W1ADRHbits __attribute__((__sfr__));

extern volatile unsigned int G1W2ADRL __attribute__((__sfr__));
typedef struct tagG1W2ADRLBITS {
  unsigned W2ADR0:1;
  unsigned W2ADR1:1;
  unsigned W2ADR2:1;
  unsigned W2ADR3:1;
  unsigned W2ADR4:1;
  unsigned W2ADR5:1;
  unsigned W2ADR6:1;
  unsigned W2ADR7:1;
  unsigned W2ADR8:1;
  unsigned W2ADR9:1;
  unsigned W2ADR10:1;
  unsigned W2ADR11:1;
  unsigned W2ADR12:1;
  unsigned W2ADR13:1;
  unsigned W2ADR14:1;
  unsigned W2ADR15:1;
} G1W2ADRLBITS;
extern volatile G1W2ADRLBITS G1W2ADRLbits __attribute__((__sfr__));

extern volatile unsigned int G1W2ADRH __attribute__((__sfr__));
typedef struct tagG1W2ADRHBITS {
  unsigned W2ADR16:1;
  unsigned W2ADR17:1;
  unsigned W2ADR18:1;
  unsigned W2ADR19:1;
  unsigned W2ADR20:1;
  unsigned W2ADR21:1;
  unsigned W2ADR22:1;
  unsigned W2ADR23:1;
} G1W2ADRHBITS;
extern volatile G1W2ADRHBITS G1W2ADRHbits __attribute__((__sfr__));

extern volatile unsigned int G1PUW __attribute__((__sfr__));
__extension__ typedef struct tagG1PUWBITS {
  union {
    struct {
      unsigned PUW:11;
    };
    struct {
      unsigned PUW0:1;
      unsigned PUW1:1;
      unsigned PUW2:1;
      unsigned PUW3:1;
      unsigned PUW4:1;
      unsigned PUW5:1;
      unsigned PUW6:1;
      unsigned PUW7:1;
      unsigned PUW8:1;
      unsigned PUW9:1;
      unsigned PUW10:1;
    };
  };
} G1PUWBITS;
extern volatile G1PUWBITS G1PUWbits __attribute__((__sfr__));

extern volatile unsigned int G1PUH __attribute__((__sfr__));
__extension__ typedef struct tagG1PUHBITS {
  union {
    struct {
      unsigned PUH:11;
    };
    struct {
      unsigned PUH0:1;
      unsigned PUH1:1;
      unsigned PUH2:1;
      unsigned PUH3:1;
      unsigned PUH4:1;
      unsigned PUH5:1;
      unsigned PUH6:1;
      unsigned PUH7:1;
      unsigned PUH8:1;
      unsigned PUH9:1;
      unsigned PUH10:1;
    };
  };
} G1PUHBITS;
extern volatile G1PUHBITS G1PUHbits __attribute__((__sfr__));

extern volatile unsigned int G1DPADRL __attribute__((__sfr__));
typedef struct tagG1DPADRLBITS {
  unsigned DPADR0:1;
  unsigned DPADR1:1;
  unsigned DPADR2:1;
  unsigned DPADR3:1;
  unsigned DPADR4:1;
  unsigned DPADR5:1;
  unsigned DPADR6:1;
  unsigned DPADR7:1;
  unsigned DPADR8:1;
  unsigned DPADR9:1;
  unsigned DPADR10:1;
  unsigned DPADR11:1;
  unsigned DPADR12:1;
  unsigned DPADR13:1;
  unsigned DPADR14:1;
  unsigned DPADR15:1;
} G1DPADRLBITS;
extern volatile G1DPADRLBITS G1DPADRLbits __attribute__((__sfr__));

extern volatile unsigned int G1DPADRH __attribute__((__sfr__));
typedef struct tagG1DPADRHBITS {
  unsigned DPADR16:1;
  unsigned DPADR17:1;
  unsigned DPADR18:1;
  unsigned DPADR19:1;
  unsigned DPADR20:1;
  unsigned DPADR21:1;
  unsigned DPADR22:1;
  unsigned DPADR23:1;
} G1DPADRHBITS;
extern volatile G1DPADRHBITS G1DPADRHbits __attribute__((__sfr__));

extern volatile unsigned int G1DPW __attribute__((__sfr__));
__extension__ typedef struct tagG1DPWBITS {
  union {
    struct {
      unsigned DPW:11;
    };
    struct {
      unsigned DPW0:1;
      unsigned DPW1:1;
      unsigned DPW2:1;
      unsigned DPW3:1;
      unsigned DPW4:1;
      unsigned DPW5:1;
      unsigned DPW6:1;
      unsigned DPW7:1;
      unsigned DPW8:1;
      unsigned DPW9:1;
      unsigned DPW10:1;
    };
  };
} G1DPWBITS;
extern volatile G1DPWBITS G1DPWbits __attribute__((__sfr__));

extern volatile unsigned int G1DPH __attribute__((__sfr__));
__extension__ typedef struct tagG1DPHBITS {
  union {
    struct {
      unsigned DPH:11;
    };
    struct {
      unsigned DPH0:1;
      unsigned DPH1:1;
      unsigned DPH2:1;
      unsigned DPH3:1;
      unsigned DPH4:1;
      unsigned DPH5:1;
      unsigned DPH6:1;
      unsigned DPH7:1;
      unsigned DPH8:1;
      unsigned DPH9:1;
      unsigned DPH10:1;
    };
  };
} G1DPHBITS;
extern volatile G1DPHBITS G1DPHbits __attribute__((__sfr__));

extern volatile unsigned int G1DPWT __attribute__((__sfr__));
__extension__ typedef struct tagG1DPWTBITS {
  union {
    struct {
      unsigned DPWT:11;
    };
    struct {
      unsigned DPWT0:1;
      unsigned DPWT1:1;
      unsigned DPWT2:1;
      unsigned DPWT3:1;
      unsigned DPWT4:1;
      unsigned DPWT5:1;
      unsigned DPWT6:1;
      unsigned DPWT7:1;
      unsigned DPWT8:1;
      unsigned DPWT9:1;
      unsigned DPWT10:1;
    };
  };
} G1DPWTBITS;
extern volatile G1DPWTBITS G1DPWTbits __attribute__((__sfr__));

extern volatile unsigned int G1DPHT __attribute__((__sfr__));
__extension__ typedef struct tagG1DPHTBITS {
  union {
    struct {
      unsigned DPHT:11;
    };
    struct {
      unsigned DPHT0:1;
      unsigned DPHT1:1;
      unsigned DPHT2:1;
      unsigned DPHT3:1;
      unsigned DPHT4:1;
      unsigned DPHT5:1;
      unsigned DPHT6:1;
      unsigned DPHT7:1;
      unsigned DPHT8:1;
      unsigned DPHT9:1;
      unsigned DPHT10:1;
    };
  };
} G1DPHTBITS;
extern volatile G1DPHTBITS G1DPHTbits __attribute__((__sfr__));

extern volatile unsigned int G1CON2 __attribute__((__sfr__));
__extension__ typedef struct tagG1CON2BITS {
  union {
    struct {
      unsigned DPMODE:3;
      unsigned :2;
      unsigned DPBPP:3;
      unsigned DPTEST:2;
      unsigned :2;
      unsigned DPSTGER:2;
      unsigned DPGWDTH:2;
    };
    struct {
      unsigned DPMODE0:1;
      unsigned DPMODE1:1;
      unsigned DPMODE2:1;
      unsigned :2;
      unsigned DPBPP0:1;
      unsigned DPBPP1:1;
      unsigned DPBPP2:1;
      unsigned DPTEST0:1;
      unsigned DPTEST1:1;
      unsigned :2;
      unsigned DPSTGER0:1;
      unsigned DPSTGER1:1;
      unsigned DPGWDTH0:1;
      unsigned DPGWDTH1:1;
    };
  };
} G1CON2BITS;
extern volatile G1CON2BITS G1CON2bits __attribute__((__sfr__));

extern volatile unsigned int G1CON3 __attribute__((__sfr__));
typedef struct tagG1CON3BITS {
  unsigned DPHSOE:1;
  unsigned DPVSOE:1;
  unsigned DPENOE:1;
  unsigned DPPWROE:1;
  unsigned DPHSPOL:1;
  unsigned DPVSPOL:1;
  unsigned DPENPOL:1;
  unsigned DPCLKPOL:1;
  unsigned DPPOWER:1;
  unsigned DPPINOE:1;
} G1CON3BITS;
extern volatile G1CON3BITS G1CON3bits __attribute__((__sfr__));

extern volatile unsigned int G1ACTDA __attribute__((__sfr__));
__extension__ typedef struct tagG1ACTDABITS {
  union {
    struct {
      unsigned ACTPIX:8;
      unsigned ACTLINE:8;
    };
    struct {
      unsigned ACTPIX0:1;
      unsigned ACTPIX1:1;
      unsigned ACTPIX2:1;
      unsigned ACTPIX3:1;
      unsigned ACTPIX4:1;
      unsigned ACTPIX5:1;
      unsigned ACTPIX6:1;
      unsigned ACTPIX7:1;
      unsigned ACTLINE0:1;
      unsigned ACTLINE1:1;
      unsigned ACTLINE2:1;
      unsigned ACTLINE3:1;
      unsigned ACTLINE4:1;
      unsigned ACTLINE5:1;
      unsigned ACTLINE6:1;
      unsigned ACTLINE7:1;
    };
  };
} G1ACTDABITS;
extern volatile G1ACTDABITS G1ACTDAbits __attribute__((__sfr__));

extern volatile unsigned int G1HSYNC __attribute__((__sfr__));
__extension__ typedef struct tagG1HSYNCBITS {
  union {
    struct {
      unsigned HSST:8;
      unsigned HSLEN:8;
    };
    struct {
      unsigned HSST0:1;
      unsigned HSST1:1;
      unsigned HSST2:1;
      unsigned HSST3:1;
      unsigned HSST4:1;
      unsigned HSST5:1;
      unsigned HSST6:1;
      unsigned HSST7:1;
      unsigned HSLEN0:1;
      unsigned HSLEN1:1;
      unsigned HSLEN2:1;
      unsigned HSLEN3:1;
      unsigned HSLEN4:1;
      unsigned HSLEN5:1;
      unsigned HSLEN6:1;
      unsigned HSLEN7:1;
    };
  };
} G1HSYNCBITS;
extern volatile G1HSYNCBITS G1HSYNCbits __attribute__((__sfr__));

extern volatile unsigned int G1VSYNC __attribute__((__sfr__));
__extension__ typedef struct tagG1VSYNCBITS {
  union {
    struct {
      unsigned VSST:8;
      unsigned VSLEN:8;
    };
    struct {
      unsigned VSST0:1;
      unsigned VSST1:1;
      unsigned VSST2:1;
      unsigned VSST3:1;
      unsigned VSST4:1;
      unsigned VSST5:1;
      unsigned VSST6:1;
      unsigned VSST7:1;
      unsigned VSLEN0:1;
      unsigned VSLEN1:1;
      unsigned VSLEN2:1;
      unsigned VSLEN3:1;
      unsigned VSLEN4:1;
      unsigned VSLEN5:1;
      unsigned VSLEN6:1;
      unsigned VSLEN7:1;
    };
  };
} G1VSYNCBITS;
extern volatile G1VSYNCBITS G1VSYNCbits __attribute__((__sfr__));

extern volatile unsigned int G1DBLCON __attribute__((__sfr__));
__extension__ typedef struct tagG1DBLCONBITS {
  union {
    struct {
      unsigned HENST:8;
      unsigned VENST:8;
    };
    struct {
      unsigned HENST0:1;
      unsigned HENST1:1;
      unsigned HENST2:1;
      unsigned HENST3:1;
      unsigned HENST4:1;
      unsigned HENST5:1;
      unsigned HENST6:1;
      unsigned HENST7:1;
      unsigned VENST0:1;
      unsigned VENST1:1;
      unsigned VENST2:1;
      unsigned VENST3:1;
      unsigned VENST4:1;
      unsigned VENST5:1;
      unsigned VENST6:1;
      unsigned VENST7:1;
    };
  };
} G1DBLCONBITS;
extern volatile G1DBLCONBITS G1DBLCONbits __attribute__((__sfr__));

extern volatile unsigned int G1CLUT __attribute__((__sfr__));
__extension__ typedef struct tagG1CLUTBITS {
  union {
    struct {
      unsigned CLUTADR:8;
      unsigned CLUTRWEN:1;
      unsigned CLUTTRD:1;
      unsigned :4;
      unsigned CLUTBUSY:1;
      unsigned CLUTEN:1;
    };
    struct {
      unsigned CLUTADR0:1;
      unsigned CLUTADR1:1;
      unsigned CLUTADR2:1;
      unsigned CLUTADR3:1;
      unsigned CLUTADR4:1;
      unsigned CLUTADR5:1;
      unsigned CLUTADR6:1;
      unsigned CLUTADR7:1;
    };
  };
} G1CLUTBITS;
extern volatile G1CLUTBITS G1CLUTbits __attribute__((__sfr__));

extern volatile unsigned int G1CLUTWR __attribute__((__sfr__));
__extension__ typedef struct tagG1CLUTWRBITS {
  union {
    struct {
      unsigned CLUTWR:16;
    };
    struct {
      unsigned CLUTWR0:1;
      unsigned CLUTWR1:1;
      unsigned CLUTWR2:1;
      unsigned CLUTWR3:1;
      unsigned CLUTWR4:1;
      unsigned CLUTWR5:1;
      unsigned CLUTWR6:1;
      unsigned CLUTWR7:1;
      unsigned CLUTWR8:1;
      unsigned CLUTWR9:1;
      unsigned CLUTWR10:1;
      unsigned CLUTWR11:1;
      unsigned CLUTWR12:1;
      unsigned CLUTWR13:1;
      unsigned CLUTWR14:1;
      unsigned CLUTWR15:1;
    };
  };
} G1CLUTWRBITS;
extern volatile G1CLUTWRBITS G1CLUTWRbits __attribute__((__sfr__));

extern volatile unsigned int G1CLUTRD __attribute__((__sfr__));
__extension__ typedef struct tagG1CLUTRDBITS {
  union {
    struct {
      unsigned CLUTRD:16;
    };
    struct {
      unsigned CLUTRD0:1;
      unsigned CLUTRD1:1;
      unsigned CLUTRD2:1;
      unsigned CLUTRD3:1;
      unsigned CLUTRD4:1;
      unsigned CLUTRD5:1;
      unsigned CLUTRD6:1;
      unsigned CLUTRD7:1;
      unsigned CLUTRD8:1;
      unsigned CLUTRD9:1;
      unsigned CLUTRD10:1;
      unsigned CLUTRD11:1;
      unsigned CLUTRD12:1;
      unsigned CLUTRD13:1;
      unsigned CLUTRD14:1;
      unsigned CLUTRD15:1;
    };
  };
} G1CLUTRDBITS;
extern volatile G1CLUTRDBITS G1CLUTRDbits __attribute__((__sfr__));

extern volatile unsigned int G1MRGN __attribute__((__sfr__));
__extension__ typedef struct tagG1MRGNBITS {
  union {
    struct {
      unsigned HBAMGN:8;
      unsigned VBAMGN:8;
    };
    struct {
      unsigned HBAMGN0:1;
      unsigned HBAMGN1:1;
      unsigned HBAMGN2:1;
      unsigned HBAMGN3:1;
      unsigned HBAMGN4:1;
      unsigned HBAMGN5:1;
      unsigned HBAMGN6:1;
      unsigned HBAMGN7:1;
      unsigned VBAMGN0:1;
      unsigned VBAMGN1:1;
      unsigned VBAMGN2:1;
      unsigned VBAMGN3:1;
      unsigned VBAMGN4:1;
      unsigned VBAMGN5:1;
      unsigned VBAMGN6:1;
      unsigned VBAMGN7:1;
    };
  };
} G1MRGNBITS;
extern volatile G1MRGNBITS G1MRGNbits __attribute__((__sfr__));

extern volatile unsigned int G1CHRX __attribute__((__sfr__));
__extension__ typedef struct tagG1CHRXBITS {
  union {
    struct {
      unsigned CURPOSX:11;
    };
    struct {
      unsigned CURPOSX0:1;
      unsigned CURPOSX1:1;
      unsigned CURPOSX2:1;
      unsigned CURPOSX3:1;
      unsigned CURPOSX4:1;
      unsigned CURPOSX5:1;
      unsigned CURPOSX6:1;
      unsigned CURPOSX7:1;
      unsigned CURPOSX8:1;
      unsigned CURPOSX9:1;
      unsigned CURPOSX10:1;
    };
  };
} G1CHRXBITS;
extern volatile G1CHRXBITS G1CHRXbits __attribute__((__sfr__));

extern volatile unsigned int G1CHRY __attribute__((__sfr__));
__extension__ typedef struct tagG1CHRYBITS {
  union {
    struct {
      unsigned CURPOSY:11;
    };
    struct {
      unsigned CURPOSY0:1;
      unsigned CURPOSY1:1;
      unsigned CURPOSY2:1;
      unsigned CURPOSY3:1;
      unsigned CURPOSY4:1;
      unsigned CURPOSY5:1;
      unsigned CURPOSY6:1;
      unsigned CURPOSY7:1;
      unsigned CURPOSY8:1;
      unsigned CURPOSY9:1;
      unsigned CURPOSY10:1;
    };
  };
} G1CHRYBITS;
extern volatile G1CHRYBITS G1CHRYbits __attribute__((__sfr__));

extern volatile unsigned int G1IPU __attribute__((__sfr__));
typedef struct tagG1IPUBITS {
  unsigned BFINAL:1;
  unsigned IPUDONE:1;
  unsigned WRAPERR:1;
  unsigned LENERR:1;
  unsigned BLCKERR:1;
  unsigned HUFFERR:1;
} G1IPUBITS;
extern volatile G1IPUBITS G1IPUbits __attribute__((__sfr__));

extern volatile unsigned int G1DBEN __attribute__((__sfr__));
__extension__ typedef struct tagG1DBENBITS {
  union {
    struct {
      unsigned GDBEN:16;
    };
    struct {
      unsigned GDBEN0:1;
      unsigned GDBEN1:1;
      unsigned GDBEN2:1;
      unsigned GDBEN3:1;
      unsigned GDBEN4:1;
      unsigned GDBEN5:1;
      unsigned GDBEN6:1;
      unsigned GDBEN7:1;
      unsigned GDBEN8:1;
      unsigned GDBEN9:1;
      unsigned GDBEN10:1;
      unsigned GDBEN11:1;
      unsigned GDBEN12:1;
      unsigned GDBEN13:1;
      unsigned GDBEN14:1;
      unsigned GDBEN15:1;
    };
  };
} G1DBENBITS;
extern volatile G1DBENBITS G1DBENbits __attribute__((__sfr__));

extern volatile unsigned int RCON __attribute__((__sfr__));
__extension__ typedef struct tagRCONBITS {
  union {
    struct {
      unsigned POR:1;
      unsigned BOR:1;
      unsigned IDLE:1;
      unsigned SLEEP:1;
      unsigned WDTO:1;
      unsigned SWDTEN:1;
      unsigned SWR:1;
      unsigned EXTR:1;
      unsigned VREGS:1;
      unsigned CM:1;
      unsigned :4;
      unsigned IOPUWR:1;
      unsigned TRAPR:1;
    };
    struct {
      unsigned :8;
      unsigned PMSLP:1;
    };
  };
} RCONBITS;
extern volatile RCONBITS RCONbits __attribute__((__sfr__));

extern volatile unsigned int OSCCON __attribute__((__sfr__));
__extension__ typedef struct tagOSCCONBITS {
  union {
    struct {
      unsigned OSWEN:1;
      unsigned SOSCEN:1;
      unsigned POSCEN:1;
      unsigned CF:1;
      unsigned :1;
      unsigned LOCK:1;
      unsigned IOLOCK:1;
      unsigned CLKLOCK:1;
      unsigned NOSC:3;
      unsigned :1;
      unsigned COSC:3;
    };
    struct {
      unsigned :1;
      unsigned LPOSCEN:1;
      unsigned :6;
      unsigned NOSC0:1;
      unsigned NOSC1:1;
      unsigned NOSC2:1;
      unsigned :1;
      unsigned COSC0:1;
      unsigned COSC1:1;
      unsigned COSC2:1;
    };
  };
} OSCCONBITS;
extern volatile OSCCONBITS OSCCONbits __attribute__((__sfr__));

extern volatile unsigned char OSCCONL __attribute__((__sfr__));
extern volatile unsigned char OSCCONH __attribute__((__sfr__));
extern volatile unsigned int CLKDIV __attribute__((__sfr__));
__extension__ typedef struct tagCLKDIVBITS {
  union {
    struct {
      unsigned :4;
      unsigned G1CLKSEL:1;
      unsigned PLLEN:1;
      unsigned CPDIV:2;
      unsigned RCDIV:3;
      unsigned DOZEN:1;
      unsigned DOZE:3;
      unsigned ROI:1;
    };
    struct {
      unsigned :6;
      unsigned CPDIV0:1;
      unsigned CPDIV1:1;
      unsigned RCDIV0:1;
      unsigned RCDIV1:1;
      unsigned RCDIV2:1;
      unsigned :1;
      unsigned DOZE0:1;
      unsigned DOZE1:1;
      unsigned DOZE2:1;
    };
  };
} CLKDIVBITS;
extern volatile CLKDIVBITS CLKDIVbits __attribute__((__sfr__));

extern volatile unsigned int CLKDIV2 __attribute__((__sfr__));
__extension__ typedef struct tagCLKDIV2BITS {
  union {
    struct {
      unsigned :9;
      unsigned GCLKDIV:7;
    };
    struct {
      unsigned :9;
      unsigned GCLKDIV0:1;
      unsigned GCLKDIV1:1;
      unsigned GCLKDIV2:1;
      unsigned GCLKDIV3:1;
      unsigned GCLKDIV4:1;
      unsigned GCLKDIV5:1;
      unsigned GCLKDIV6:1;
    };
  };
} CLKDIV2BITS;
extern volatile CLKDIV2BITS CLKDIV2bits __attribute__((__sfr__));

extern volatile unsigned int OSCTUN __attribute__((__sfr__));
__extension__ typedef struct tagOSCTUNBITS {
  union {
    struct {
      unsigned TUN:6;
    };
    struct {
      unsigned TUN0:1;
      unsigned TUN1:1;
      unsigned TUN2:1;
      unsigned TUN3:1;
      unsigned TUN4:1;
      unsigned TUN5:1;
    };
  };
} OSCTUNBITS;
extern volatile OSCTUNBITS OSCTUNbits __attribute__((__sfr__));

extern volatile unsigned int REFOCON __attribute__((__sfr__));
__extension__ typedef struct tagREFOCONBITS {
  union {
    struct {
      unsigned :8;
      unsigned RODIV:4;
      unsigned ROSEL:1;
      unsigned ROSSLP:1;
      unsigned :1;
      unsigned ROEN:1;
    };
    struct {
      unsigned :8;
      unsigned RODIV0:1;
      unsigned RODIV1:1;
      unsigned RODIV2:1;
      unsigned RODIV3:1;
      unsigned :3;
      unsigned ROON:1;
    };
  };
} REFOCONBITS;
extern volatile REFOCONBITS REFOCONbits __attribute__((__sfr__));

extern volatile unsigned int NVMCON __attribute__((__sfr__));
__extension__ typedef struct tagNVMCONBITS {
  union {
    struct {
      unsigned NVMOP:4;
      unsigned :2;
      unsigned ERASE:1;
      unsigned :6;
      unsigned WRERR:1;
      unsigned WREN:1;
      unsigned WR:1;
    };
    struct {
      unsigned NVMOP0:1;
      unsigned NVMOP1:1;
      unsigned NVMOP2:1;
      unsigned NVMOP3:1;
    };
    struct {
      unsigned PROGOP0:1;
      unsigned PROGOP1:1;
      unsigned PROGOP2:1;
      unsigned PROGOP3:1;
    };
    struct {
      unsigned PROGOP:4;
    };
  };
} NVMCONBITS;
extern volatile NVMCONBITS NVMCONbits __attribute__((__sfr__));

extern volatile unsigned int NVMKEY __attribute__((__sfr__));
extern volatile unsigned int PMD1 __attribute__((__sfr__));
typedef struct tagPMD1BITS {
  unsigned ADC1MD:1;
  unsigned :2;
  unsigned SPI1MD:1;
  unsigned SPI2MD:1;
  unsigned U1MD:1;
  unsigned U2MD:1;
  unsigned I2C1MD:1;
  unsigned :3;
  unsigned T1MD:1;
  unsigned T2MD:1;
  unsigned T3MD:1;
  unsigned T4MD:1;
  unsigned T5MD:1;
} PMD1BITS;
extern volatile PMD1BITS PMD1bits __attribute__((__sfr__));

extern volatile unsigned int PMD2 __attribute__((__sfr__));
typedef struct tagPMD2BITS {
  unsigned OC1MD:1;
  unsigned OC2MD:1;
  unsigned OC3MD:1;
  unsigned OC4MD:1;
  unsigned OC5MD:1;
  unsigned OC6MD:1;
  unsigned OC7MD:1;
  unsigned OC8MD:1;
  unsigned IC1MD:1;
  unsigned IC2MD:1;
  unsigned IC3MD:1;
  unsigned IC4MD:1;
  unsigned IC5MD:1;
  unsigned IC6MD:1;
  unsigned IC7MD:1;
  unsigned IC8MD:1;
} PMD2BITS;
extern volatile PMD2BITS PMD2bits __attribute__((__sfr__));

extern volatile unsigned int PMD3 __attribute__((__sfr__));
__extension__ typedef struct tagPMD3BITS {
  union {
    struct {
      unsigned :1;
      unsigned I2C2MD:1;
      unsigned I2C3MD:1;
      unsigned U3MD:1;
      unsigned :3;
      unsigned CRCMD:1;
      unsigned :1;
      unsigned RTCCMD:1;
      unsigned CMPMD:1;
    };
    struct {
      unsigned :7;
      unsigned CRCPMD:1;
    };
  };
} PMD3BITS;
extern volatile PMD3BITS PMD3bits __attribute__((__sfr__));

extern volatile unsigned int PMD4 __attribute__((__sfr__));
typedef struct tagPMD4BITS {
  unsigned USB1MD:1;
  unsigned LVDMD:1;
  unsigned CTMUMD:1;
  unsigned REFOMD:1;
  unsigned :1;
  unsigned U4MD:1;
  unsigned UPWMMD:1;
} PMD4BITS;
extern volatile PMD4BITS PMD4bits __attribute__((__sfr__));

extern volatile unsigned int PMD5 __attribute__((__sfr__));
typedef struct tagPMD5BITS {
  unsigned OC9MD:1;
  unsigned :7;
  unsigned IC9MD:1;
} PMD5BITS;
extern volatile PMD5BITS PMD5bits __attribute__((__sfr__));

extern volatile unsigned int PMD6 __attribute__((__sfr__));
typedef struct tagPMD6BITS {
  unsigned SPI3MD:1;
  unsigned :5;
  unsigned GFX1MD:1;
} PMD6BITS;
extern volatile PMD6BITS PMD6bits __attribute__((__sfr__));
# 10818 "/opt/microchip/xc16/v1.10/bin/bin/../../support/PIC24F/h/p24FJ256DA206.h" 3 4
extern __attribute__((space(prog))) int _CONFIG4;
# 10834 "/opt/microchip/xc16/v1.10/bin/bin/../../support/PIC24F/h/p24FJ256DA206.h" 3 4
extern __attribute__((space(prog))) int _CONFIG3;
# 11241 "/opt/microchip/xc16/v1.10/bin/bin/../../support/PIC24F/h/p24FJ256DA206.h" 3 4
extern __attribute__((space(prog))) int _CONFIG2;
# 11341 "/opt/microchip/xc16/v1.10/bin/bin/../../support/PIC24F/h/p24FJ256DA206.h" 3 4
extern __attribute__((space(prog))) int _CONFIG1;
# 259 "/opt/microchip/xc16/v1.10/bin/bin/../../support/PIC24F/h/p24Fxxxx.h" 2 3 4
# 37 "byte_queue.h" 2


typedef struct {
  BYTE* buf;
  int capacity;
  int read_cursor;
  int write_cursor;
  int size;
} BYTE_QUEUE;





static inline void ByteQueueClear(BYTE_QUEUE* q) {
  q->size = 0;
  q->read_cursor = 0;
  q->write_cursor = 0;
}

static inline void ByteQueueInit(BYTE_QUEUE* q, BYTE* buf, int capacity) {
  q->buf = buf;
  q->capacity = capacity;
  ByteQueueClear(q);
}

void ByteQueuePushBuffer(BYTE_QUEUE* q, const void* buf, int len);
void ByteQueuePeek(BYTE_QUEUE* q, const BYTE** data, int* size);


void ByteQueuePeekMax(BYTE_QUEUE* q, int max_size, const BYTE** data1,
                      int* size1, const BYTE** data2, int* size2);
void ByteQueuePull(BYTE_QUEUE* q, int size);
void ByteQueuePullToBuffer(BYTE_QUEUE* q, void* buffer, int size);

void ByteQueuePushByte(BYTE_QUEUE* q, BYTE b);
BYTE ByteQueuePullByte(BYTE_QUEUE* q);

static inline int ByteQueueSize(BYTE_QUEUE* q) { return q->size; }
static inline int ByteQueueRemaining(BYTE_QUEUE* q) { return q->capacity - q->size; }
# 37 "protocol.c" 2
# 1 "features.h" 1
# 35 "features.h"
void SetPinDigitalOut(int pin, int value, int open_drain);
void SetPinDigitalIn(int pin, int pull);
void SetPinAnalogIn(int pin);
void SetPinPwm(int pin, int pwm_num, int enable);
void SetPinUart(int pin, int uart_num, int dir, int enable);
void SetPinSpi(int pin, int spi_num, int mode, int enable);
void SetPinInCap(int pin, int incap_num, int enable);
void HardReset();
void SoftReset();
void CheckInterface(BYTE interface_id[8]);
# 38 "protocol.c" 2
# 1 "pwm.h" 1
# 34 "pwm.h"
void PWMInit();
void SetPwmDutyCycle(int pwm_num, int dc, int fraction);
void SetPwmPeriod(int pwm_num, int period, int scale);
# 39 "protocol.c" 2
# 1 "adc.h" 1
# 46 "adc.h"
void ADCInit();




void ADCSetScan(int pin, int enable);




void ADCClrScan(int pin);
# 40 "protocol.c" 2
# 1 "digital.h" 1
# 34 "digital.h"
void SetDigitalOutLevel(int pin, int value);
void SetChangeNotify(int pin, int changeNotify);
# 41 "protocol.c" 2
# 1 "../common/logging.h" 1
# 42 "protocol.c" 2
# 1 "../common/platform.h" 1
# 43 "protocol.c" 2
# 1 "uart.h" 1
# 33 "uart.h"
void UARTInit();
void UARTConfig(int uart_num, int rate, int speed4x, int two_stop_bits,
                int parity);
void UARTTransmit(int uart_num, const void* data, int size);
void UARTTasks();
# 44 "protocol.c" 2
# 1 "spi.h" 1
# 34 "spi.h"
void SPIInit();
void SPIConfigMaster(int spi_num, int scale, int div, int smp_end, int clk_edge,
                     int clk_pol);
void SPITasks();
void SPITransmit(int spi_num, int dest, const void* data, int data_size,
                 int total_size, int trim_rx);
# 45 "protocol.c" 2
# 1 "i2c.h" 1
# 34 "i2c.h"
void I2CInit();
void I2CTasks();

void I2CConfigMaster(int i2c_num, int rate, int smbus_levels);
void I2CWriteRead(int i2c_num, unsigned int addr, const void* data,
                  int write_bytes, int read_bytes);
# 46 "protocol.c" 2
# 1 "sync.h" 1
# 45 "sync.h"
static inline BYTE SyncInterruptLevel(BYTE level) {
    BYTE ret = SRbits.IPL;
    SRbits.IPL = level;
    return ret;
}
# 47 "protocol.c" 2
# 1 "icsp.h" 1
# 35 "icsp.h"
void ICSPConfigure(int enable);
void ICSPSix(DWORD inst);
void ICSPRegout();
void ICSPEnter();
void ICSPExit();
void ICSPTasks();
# 48 "protocol.c" 2
# 1 "incap.h" 1
# 34 "incap.h"
void InCapInit();
# 49 "incap.h"
void InCapConfig(int incap_num, int double_prec, int mode, int clock);
# 49 "protocol.c" 2



const BYTE incoming_arg_size[MESSAGE_TYPE_LIMIT] = {
  sizeof(HARD_RESET_ARGS),
  sizeof(SOFT_RESET_ARGS),
  sizeof(CHECK_INTERFACE_ARGS),
  sizeof(SET_PIN_DIGITAL_OUT_ARGS),
  sizeof(SET_DIGITAL_OUT_LEVEL_ARGS),
  sizeof(SET_PIN_DIGITAL_IN_ARGS),
  sizeof(SET_CHANGE_NOTIFY_ARGS),
  sizeof(REGISTER_PERIODIC_DIGITAL_SAMPLING_ARGS),
  sizeof(SET_PIN_PWM_ARGS),
  sizeof(SET_PWM_DUTY_CYCLE_ARGS),
  sizeof(SET_PWM_PERIOD_ARGS),
  sizeof(SET_PIN_ANALOG_IN_ARGS),
  sizeof(SET_ANALOG_IN_SAMPLING_ARGS),
  sizeof(UART_CONFIG_ARGS),
  sizeof(UART_DATA_ARGS),
  sizeof(SET_PIN_UART_ARGS),
  sizeof(SPI_CONFIGURE_MASTER_ARGS),
  sizeof(SPI_MASTER_REQUEST_ARGS),
  sizeof(SET_PIN_SPI_ARGS),
  sizeof(I2C_CONFIGURE_MASTER_ARGS),
  sizeof(I2C_WRITE_READ_ARGS),
  sizeof(RESERVED_ARGS),
  sizeof(ICSP_SIX_ARGS),
  sizeof(ICSP_REGOUT_ARGS),
  sizeof(ICSP_PROG_ENTER_ARGS),
  sizeof(ICSP_PROG_EXIT_ARGS),
  sizeof(ICSP_CONFIG_ARGS),
  sizeof(INCAP_CONFIG_ARGS),
  sizeof(SET_PIN_INCAP_ARGS),
  sizeof(SOFT_CLOSE_ARGS)


};

const BYTE outgoing_arg_size[MESSAGE_TYPE_LIMIT] = {
  sizeof(ESTABLISH_CONNECTION_ARGS),
  sizeof(SOFT_RESET_ARGS),
  sizeof(CHECK_INTERFACE_RESPONSE_ARGS),
  sizeof(RESERVED_ARGS),
  sizeof(REPORT_DIGITAL_IN_STATUS_ARGS),
  sizeof(RESERVED_ARGS),
  sizeof(SET_CHANGE_NOTIFY_ARGS),
  sizeof(REGISTER_PERIODIC_DIGITAL_SAMPLING_ARGS),
  sizeof(RESERVED_ARGS),
  sizeof(RESERVED_ARGS),
  sizeof(RESERVED_ARGS),
  sizeof(REPORT_ANALOG_IN_STATUS_ARGS),
  sizeof(REPORT_ANALOG_IN_FORMAT_ARGS),
  sizeof(UART_STATUS_ARGS),
  sizeof(UART_DATA_ARGS),
  sizeof(UART_REPORT_TX_STATUS_ARGS),
  sizeof(SPI_STATUS_ARGS),
  sizeof(SPI_DATA_ARGS),
  sizeof(SPI_REPORT_TX_STATUS_ARGS),
  sizeof(I2C_STATUS_ARGS),
  sizeof(I2C_RESULT_ARGS),
  sizeof(I2C_REPORT_TX_STATUS_ARGS),
  sizeof(ICSP_REPORT_RX_STATUS_ARGS),
  sizeof(ICSP_RESULT_ARGS),
  sizeof(RESERVED_ARGS),
  sizeof(RESERVED_ARGS),
  sizeof(ICSP_CONFIG_ARGS),
  sizeof(INCAP_STATUS_ARGS),
  sizeof(INCAP_REPORT_ARGS),
  sizeof(SOFT_CLOSE_ARGS)



};

typedef enum {
  STATE_OPEN,
  STATE_CLOSING,
  STATE_CLOSED
} STATE;

static BYTE tx_queue_buf[8192] __attribute__((far)); static BYTE_QUEUE tx_queue = { tx_queue_buf, 8192, 0, 0, 0 };
static int bytes_out;
static int max_packet;
static STATE state;

typedef enum {
  WAIT_TYPE,
  WAIT_ARGS,
  WAIT_VAR_ARGS
} RX_MESSAGE_STATE;

static INCOMING_MESSAGE rx_msg;
static int rx_buffer_cursor;
static int rx_message_remaining;
static RX_MESSAGE_STATE rx_message_state;

static inline BYTE OutgoingMessageLength(const OUTGOING_MESSAGE* msg) {
  return 1 + outgoing_arg_size[msg->type];
}

static inline BYTE IncomingVarArgSize(const INCOMING_MESSAGE* msg) {
  switch (msg->type) {
    case UART_DATA:
      return msg->args.uart_data.size + 1;

    case SPI_MASTER_REQUEST:
      if (msg->args.spi_master_request.data_size_neq_total) {
        return msg->args.spi_master_request.data_size
            + msg->args.spi_master_request.res_size_neq_total;
      } else {
        return msg->args.spi_master_request.total_size
            + msg->args.spi_master_request.res_size_neq_total;
      }

    case I2C_WRITE_READ:
      return msg->args.i2c_write_read.write_size;


    default:
      return 0;
  }
}

void AppProtocolInit(CHANNEL_HANDLE h) {
  _prog_addressT p;
  bytes_out = 0;
  rx_buffer_cursor = 0;
  rx_message_remaining = 1;
  rx_message_state = WAIT_TYPE;
  ByteQueueClear(&tx_queue);
  max_packet = ConnectionGetMaxPacket(h);
  state = STATE_OPEN;

  OUTGOING_MESSAGE msg;
  msg.type = ESTABLISH_CONNECTION;
  msg.args.establish_connection.magic = 0x4F494F49L;

  (p = __builtin_tbladdress(&hardware_version));
  _memcpy_helper(p, msg.args.establish_connection.hw_impl_ver, 8, 0);
  (p = __builtin_tbladdress(&bootloader_version));
  _memcpy_helper(p, msg.args.establish_connection.bl_impl_ver, 8, 0);

  memcpy(msg.args.establish_connection.fw_impl_ver, "IOIO0326", 8);

  AppProtocolSendMessage(&msg);
}

void AppProtocolSendMessage(const OUTGOING_MESSAGE* msg) {
  if (state != STATE_OPEN) return;
  BYTE prev = SyncInterruptLevel(1);
  ByteQueuePushBuffer(&tx_queue, (const BYTE*) msg, OutgoingMessageLength(msg));
  SyncInterruptLevel(prev);
}

void AppProtocolSendMessageWithVarArg(const OUTGOING_MESSAGE* msg, const void* data, int size) {
  if (state != STATE_OPEN) return;
  BYTE prev = SyncInterruptLevel(1);
  ByteQueuePushBuffer(&tx_queue, (const BYTE*) msg, OutgoingMessageLength(msg));
  ByteQueuePushBuffer(&tx_queue, data, size);
  SyncInterruptLevel(prev);
}

void AppProtocolSendMessageWithVarArgSplit(const OUTGOING_MESSAGE* msg,
                                           const void* data1, int size1,
                                           const void* data2, int size2) {
  if (state != STATE_OPEN) return;
  BYTE prev = SyncInterruptLevel(1);
  ByteQueuePushBuffer(&tx_queue, (const BYTE*) msg, OutgoingMessageLength(msg));
  ByteQueuePushBuffer(&tx_queue, data1, size1);
  ByteQueuePushBuffer(&tx_queue, data2, size2);
  SyncInterruptLevel(prev);
}

void AppProtocolTasks(CHANNEL_HANDLE h) {
  if (state == STATE_CLOSED) return;
  if (state == STATE_CLOSING && ByteQueueSize(&tx_queue) == 0) {
    ;
    ConnectionCloseChannel(h);
    state = STATE_CLOSED;
    return;
  }
  UARTTasks();
  SPITasks();
  I2CTasks();
  ICSPTasks();
  if (ConnectionCanSend(h)) {
    BYTE prev = SyncInterruptLevel(1);
    const BYTE* data;
    if (bytes_out) {
      ByteQueuePull(&tx_queue, bytes_out);
      bytes_out = 0;
    }
    ByteQueuePeek(&tx_queue, &data, &bytes_out);
    if (bytes_out > 0) {
      if (bytes_out > max_packet) bytes_out = max_packet;
      ConnectionSend(h, data, bytes_out);
    }
    SyncInterruptLevel(prev);
  }
}

static void Echo() {
  AppProtocolSendMessage((const OUTGOING_MESSAGE*) &rx_msg);
}

static BOOL MessageDone() {

  switch (rx_msg.type) {
    case HARD_RESET:
      do { if (!(rx_msg.args.hard_reset.magic == 0x4F494F49L)) { ; return FALSE; }} while(0);
      HardReset();
      break;

    case SOFT_RESET:
      SoftReset();
      Echo();
      break;

    case SET_PIN_DIGITAL_OUT:
      do { if (!(rx_msg.args.set_pin_digital_out.pin < 49)) { ; return FALSE; }} while(0);
      SetPinDigitalOut(rx_msg.args.set_pin_digital_out.pin,
                       rx_msg.args.set_pin_digital_out.value,
                       rx_msg.args.set_pin_digital_out.open_drain);
      break;

    case SET_DIGITAL_OUT_LEVEL:
      do { if (!(rx_msg.args.set_digital_out_level.pin < 49)) { ; return FALSE; }} while(0);
      SetDigitalOutLevel(rx_msg.args.set_digital_out_level.pin,
                         rx_msg.args.set_digital_out_level.value);
      break;

    case SET_PIN_DIGITAL_IN:
      do { if (!(rx_msg.args.set_pin_digital_in.pin < 49)) { ; return FALSE; }} while(0);
      do { if (!(rx_msg.args.set_pin_digital_in.pull < 3)) { ; return FALSE; }} while(0);
      SetPinDigitalIn(rx_msg.args.set_pin_digital_in.pin, rx_msg.args.set_pin_digital_in.pull);
      break;

    case SET_CHANGE_NOTIFY:
      do { if (!(rx_msg.args.set_change_notify.pin < 49)) { ; return FALSE; }} while(0);
      if (rx_msg.args.set_change_notify.cn) {
        Echo();
      }
      SetChangeNotify(rx_msg.args.set_change_notify.pin, rx_msg.args.set_change_notify.cn);
      if (!rx_msg.args.set_change_notify.cn) {
        Echo();
      }
      break;

    case SET_PIN_PWM:
      do { if (!(rx_msg.args.set_pin_pwm.pin < 49)) { ; return FALSE; }} while(0);
      do { if (!(rx_msg.args.set_pin_pwm.pwm_num < 9)) { ; return FALSE; }} while(0);
      SetPinPwm(rx_msg.args.set_pin_pwm.pin, rx_msg.args.set_pin_pwm.pwm_num,
                rx_msg.args.set_pin_pwm.enable);
      break;

    case SET_PWM_DUTY_CYCLE:
      do { if (!(rx_msg.args.set_pwm_duty_cycle.pwm_num < 9)) { ; return FALSE; }} while(0);
      SetPwmDutyCycle(rx_msg.args.set_pwm_duty_cycle.pwm_num,
                      rx_msg.args.set_pwm_duty_cycle.dc,
                      rx_msg.args.set_pwm_duty_cycle.fraction);
      break;

    case SET_PWM_PERIOD:
      do { if (!(rx_msg.args.set_pwm_period.pwm_num < 9)) { ; return FALSE; }} while(0);
      SetPwmPeriod(rx_msg.args.set_pwm_period.pwm_num,
                   rx_msg.args.set_pwm_period.period,
                   rx_msg.args.set_pwm_period.scale_l
                   | (rx_msg.args.set_pwm_period.scale_h) << 1);
      break;

    case SET_PIN_ANALOG_IN:
      do { if (!(rx_msg.args.set_pin_analog_in.pin < 49)) { ; return FALSE; }} while(0);
      SetPinAnalogIn(rx_msg.args.set_pin_analog_in.pin);
      break;

    case UART_DATA:
      do { if (!(rx_msg.args.uart_data.uart_num < 4)) { ; return FALSE; }} while(0);
      UARTTransmit(rx_msg.args.uart_data.uart_num,
                   rx_msg.args.uart_data.data,
                   rx_msg.args.uart_data.size + 1);
      break;

    case UART_CONFIG:
      do { if (!(rx_msg.args.uart_config.uart_num < 4)) { ; return FALSE; }} while(0);
      do { if (!(rx_msg.args.uart_config.parity < 3)) { ; return FALSE; }} while(0);
      UARTConfig(rx_msg.args.uart_config.uart_num,
                 rx_msg.args.uart_config.rate,
                 rx_msg.args.uart_config.speed4x,
                 rx_msg.args.uart_config.two_stop_bits,
                 rx_msg.args.uart_config.parity);
      break;

    case SET_PIN_UART:
      do { if (!(rx_msg.args.set_pin_uart.pin < 49)) { ; return FALSE; }} while(0);
      do { if (!(rx_msg.args.set_pin_uart.uart_num < 4)) { ; return FALSE; }} while(0);
      SetPinUart(rx_msg.args.set_pin_uart.pin,
                 rx_msg.args.set_pin_uart.uart_num,
                 rx_msg.args.set_pin_uart.dir,
                 rx_msg.args.set_pin_uart.enable);
      break;

    case SPI_MASTER_REQUEST:
      do { if (!(rx_msg.args.spi_master_request.spi_num < 3)) { ; return FALSE; }} while(0);
      do { if (!(rx_msg.args.spi_master_request.ss_pin < 49)) { ; return FALSE; }} while(0);
      {
        const BYTE total_size = rx_msg.args.spi_master_request.total_size + 1;
        const BYTE data_size = rx_msg.args.spi_master_request.data_size_neq_total
            ? rx_msg.args.spi_master_request.data_size
            : total_size;
        const BYTE res_size = rx_msg.args.spi_master_request.res_size_neq_total
            ? rx_msg.args.spi_master_request.vararg[
                rx_msg.args.spi_master_request.data_size_neq_total]
            : total_size;
        const BYTE* const data = &rx_msg.args.spi_master_request.vararg[
            rx_msg.args.spi_master_request.data_size_neq_total
            + rx_msg.args.spi_master_request.res_size_neq_total];

        SPITransmit(rx_msg.args.spi_master_request.spi_num,
                    rx_msg.args.spi_master_request.ss_pin,
                    data,
                    data_size,
                    total_size,
                    total_size - res_size);
      }
      break;

    case SPI_CONFIGURE_MASTER:
      do { if (!(rx_msg.args.spi_configure_master.spi_num < 3)) { ; return FALSE; }} while(0);
      SPIConfigMaster(rx_msg.args.spi_configure_master.spi_num,
                      rx_msg.args.spi_configure_master.scale,
                      rx_msg.args.spi_configure_master.div,
                      rx_msg.args.spi_configure_master.smp_end,
                      rx_msg.args.spi_configure_master.clk_edge,
                      rx_msg.args.spi_configure_master.clk_pol);
      break;

    case SET_PIN_SPI:
      do { if (!(rx_msg.args.set_pin_spi.mode < 3)) { ; return FALSE; }} while(0);
      do { if (!((!rx_msg.args.set_pin_spi.enable && rx_msg.args.set_pin_spi.mode == 1) || rx_msg.args.set_pin_spi.pin < 49)) { ; return FALSE; }} while(0)

                                                      ;
      do { if (!((!rx_msg.args.set_pin_spi.enable && rx_msg.args.set_pin_spi.mode != 1) || rx_msg.args.set_pin_spi.spi_num < 3)) { ; return FALSE; }} while(0)

                                                                 ;
      SetPinSpi(rx_msg.args.set_pin_spi.pin,
                rx_msg.args.set_pin_spi.spi_num,
                rx_msg.args.set_pin_spi.mode,
                rx_msg.args.set_pin_spi.enable);
      break;

    case I2C_CONFIGURE_MASTER:
      do { if (!(rx_msg.args.i2c_configure_master.i2c_num < 3)) { ; return FALSE; }} while(0);
      I2CConfigMaster(rx_msg.args.i2c_configure_master.i2c_num,
                      rx_msg.args.i2c_configure_master.rate,
                      rx_msg.args.i2c_configure_master.smbus_levels);
      break;

    case I2C_WRITE_READ:
      do { if (!(rx_msg.args.i2c_write_read.i2c_num < 3)) { ; return FALSE; }} while(0);
      {
        unsigned int addr;
        if (rx_msg.args.i2c_write_read.ten_bit_addr) {
          addr = rx_msg.args.i2c_write_read.addr_lsb;
          addr = addr << 8
                  | ((rx_msg.args.i2c_write_read.addr_msb << 1)
                    | 0b11110000);
        } else {
          do { if (!(rx_msg.args.i2c_write_read.addr_msb == 0 && rx_msg.args.i2c_write_read.addr_lsb >> 7 == 0 && rx_msg.args.i2c_write_read.addr_lsb >> 2 != 0b0011110)) { ; return FALSE; }} while(0)

                                                                         ;
          addr = rx_msg.args.i2c_write_read.addr_lsb << 1;
        }
        I2CWriteRead(rx_msg.args.i2c_write_read.i2c_num,
                     addr,
                     rx_msg.args.i2c_write_read.data,
                     rx_msg.args.i2c_write_read.write_size,
                     rx_msg.args.i2c_write_read.read_size);
      }
      break;

    case SET_ANALOG_IN_SAMPLING:
      do { if (!(rx_msg.args.set_analog_pin_sampling.pin < 49)) { ; return FALSE; }} while(0);
      ADCSetScan(rx_msg.args.set_analog_pin_sampling.pin,
                 rx_msg.args.set_analog_pin_sampling.enable);
      break;

    case CHECK_INTERFACE:
      CheckInterface(rx_msg.args.check_interface.interface_id);
      break;

    case ICSP_SIX:
      ICSPSix(rx_msg.args.icsp_six.inst);
      break;

    case ICSP_REGOUT:
      ICSPRegout();
      break;

    case ICSP_PROG_ENTER:
      ICSPEnter();
      break;

    case ICSP_PROG_EXIT:
      ICSPExit();
      break;

    case ICSP_CONFIG:
      if (rx_msg.args.icsp_config.enable) {
        Echo();
      }
      ICSPConfigure(rx_msg.args.icsp_config.enable);
      if (!rx_msg.args.icsp_config.enable) {
        Echo();
      }
      break;

    case INCAP_CONFIG:
      do { if (!(rx_msg.args.incap_config.incap_num < 9)) { ; return FALSE; }} while(0);
      do { if (!(!rx_msg.args.incap_config.double_prec || 0 == (rx_msg.args.incap_config.incap_num & 0x01))) { ; return FALSE; }} while(0)
                                                                ;
      do { if (!(rx_msg.args.incap_config.mode < 6)) { ; return FALSE; }} while(0);
      do { if (!(rx_msg.args.incap_config.clock < 4)) { ; return FALSE; }} while(0);
      InCapConfig(rx_msg.args.incap_config.incap_num,
                  rx_msg.args.incap_config.double_prec,
                  rx_msg.args.incap_config.mode,
                  rx_msg.args.incap_config.clock);
      break;

    case SET_PIN_INCAP:
      do { if (!(rx_msg.args.set_pin_incap.incap_num < 9)) { ; return FALSE; }} while(0);
      do { if (!(!rx_msg.args.set_pin_incap.enable || rx_msg.args.set_pin_incap.pin < 49)) { ; return FALSE; }} while(0)
                                                        ;
      SetPinInCap(rx_msg.args.set_pin_incap.pin,
                  rx_msg.args.set_pin_incap.incap_num,
                  rx_msg.args.set_pin_incap.enable);
      break;

    case SOFT_CLOSE:
      ;
      Echo();
      state = STATE_CLOSING;
      break;




    default:
      return FALSE;
  }
  return TRUE;
}

BOOL AppProtocolHandleIncoming(const BYTE* data, UINT32 data_len) {
  ((void)0);
  if (state != STATE_OPEN) {
    ;
    return FALSE;
  }

  while (data_len > 0) {

    if (data_len >= rx_message_remaining) {
      memcpy(((BYTE *) &rx_msg) + rx_buffer_cursor, data, rx_message_remaining);
      data += rx_message_remaining;
      data_len -= rx_message_remaining;
      rx_buffer_cursor += rx_message_remaining;
      rx_message_remaining = 0;
    } else {
      memcpy(((BYTE *) &rx_msg) + rx_buffer_cursor, data, data_len);
      rx_buffer_cursor += data_len;
      rx_message_remaining -= data_len;
      data_len = 0;
    }


    if (rx_message_remaining == 0) {
      switch (rx_message_state) {
        case WAIT_TYPE:
          rx_message_state = WAIT_ARGS;
          rx_message_remaining = incoming_arg_size[rx_msg.type];
          if (rx_message_remaining) break;


        case WAIT_ARGS:
          rx_message_state = WAIT_VAR_ARGS;
          rx_message_remaining = IncomingVarArgSize(&rx_msg);
          if (rx_message_remaining) break;


        case WAIT_VAR_ARGS:
          rx_message_state = WAIT_TYPE;
          rx_message_remaining = 1;
          rx_buffer_cursor = 0;
          if (!MessageDone()) return FALSE;
          break;
      }
    }
  }
  return TRUE;
}
