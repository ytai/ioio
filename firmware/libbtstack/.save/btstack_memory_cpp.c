# 1 "src/btstack_memory.c"
# 1 "/usr/local/google/home/dchristian/IOIO/ioio.git/firmware/libbtstack//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "src/btstack_memory.c"
# 46 "src/btstack_memory.c"
# 1 "src/btstack_memory.h" 1
# 44 "src/btstack_memory.h"
       





void btstack_memory_init(void);

void * btstack_memory_hci_connection_get(void);
void btstack_memory_hci_connection_free(void *hci_connection);
void * btstack_memory_l2cap_service_get(void);
void btstack_memory_l2cap_service_free(void *l2cap_service);
void * btstack_memory_l2cap_channel_get(void);
void btstack_memory_l2cap_channel_free(void *l2cap_channel);
void * btstack_memory_rfcomm_multiplexer_get(void);
void btstack_memory_rfcomm_multiplexer_free(void *rfcomm_multiplexer);
void * btstack_memory_rfcomm_service_get(void);
void btstack_memory_rfcomm_service_free(void *rfcomm_service);
void * btstack_memory_rfcomm_channel_get(void);
void btstack_memory_rfcomm_channel_free(void *rfcomm_channel);
void * btstack_memory_db_mem_device_name_get(void);
void btstack_memory_db_mem_device_name_free(void *db_mem_device_name);
void * btstack_memory_db_mem_device_link_key_get(void);
void btstack_memory_db_mem_device_link_key_free(void *db_mem_device_link_key);
void * btstack_memory_db_mem_service_get(void);
void btstack_memory_db_mem_service_free(void *db_mem_service);
# 47 "src/btstack_memory.c" 2
# 1 "./include/btstack/memory_pool.h" 1
# 43 "./include/btstack/memory_pool.h"
       

typedef void * memory_pool_t;


void memory_pool_create(memory_pool_t *pool, void * storage, int count, int block_size);


void * memory_pool_get(memory_pool_t *pool);


void memory_pool_free(memory_pool_t *pool, void * block);
# 48 "src/btstack_memory.c" 2

# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdlib.h" 1 3 4






# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stddef.h" 1 3 4



typedef int ptrdiff_t;
typedef unsigned int size_t;
typedef short unsigned int wchar_t;






extern int errno;
# 8 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdlib.h" 2 3 4
# 16 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdlib.h" 3 4
typedef struct {
 int quot;
 int rem;
} div_t;
typedef struct {
 unsigned quot;
 unsigned rem;
} udiv_t;
typedef struct {
 long quot;
 long rem;
} ldiv_t;
typedef struct {
 unsigned long quot;
 unsigned long rem;
} uldiv_t;
# 47 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdlib.h" 3 4
extern double atof(const char *);
extern double strtod(const char *, char **);


extern int atoi(const char *);




extern long atol(const char *);

extern long long atoll(const char *);

extern long strtol(const char *, char **, int);
extern unsigned long strtoul(const char *, char **, int);
extern long long strtoll(const char *, char **, int);
extern unsigned long long strtoull(const char *, char **, int);
extern int rand(void);
extern void srand(unsigned int);
extern void * calloc(size_t, size_t);
extern div_t div(int numer, int denom);
extern udiv_t udiv(unsigned numer, unsigned denom);
extern ldiv_t ldiv(long numer, long denom);
extern uldiv_t uldiv(unsigned long numer,unsigned long denom);




extern void * malloc(size_t);
extern void free(void *);
extern void * realloc(void *, size_t);

extern void abort(void);
extern void exit(int);
extern int atexit(void (*)(void));
extern char * getenv(const char *);
extern char ** environ;
extern int system(char *);
extern void qsort(void *, size_t, size_t, int (*)(const void *, const void *));
extern void * bsearch(const void *, void *, size_t, size_t, int(*)(const void *, const void *));
extern int abs(int);
extern long labs(long);


extern char * itoa(char * buf, int val, int base);
extern char * utoa(char * buf, unsigned val, int base);
extern char * ltoa(char * buf, long val, int base);
extern char * ultoa(char * buf, unsigned long val, int base);
# 50 "src/btstack_memory.c" 2

# 1 "./config.h" 1
# 52 "src/btstack_memory.c" 2
# 1 "src/hci.h" 1
# 44 "src/hci.h"
       

# 1 "./config.h" 1
# 47 "src/hci.h" 2

# 1 "./include/btstack/hci_cmds.h" 1
# 38 "./include/btstack/hci_cmds.h"
       

# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 1 3 4
# 13 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 3 4
typedef signed char int8_t;






typedef signed int int16_t;






typedef signed long int int32_t;






typedef signed long long int int64_t;
# 43 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 3 4
typedef unsigned char uint8_t;





typedef unsigned int uint16_t;





typedef unsigned long int uint32_t;





typedef unsigned long long int uint64_t;
# 70 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 3 4
typedef signed char int_least8_t;






typedef signed int int_least16_t;






typedef signed long int int_least24_t;






typedef signed long int int_least32_t;






typedef signed long long int int_least64_t;






typedef unsigned char uint_least8_t;





typedef unsigned int uint_least16_t;





typedef long int uint_least24_t;





typedef unsigned long int uint_least32_t;





typedef unsigned long long int uint_least64_t;







typedef signed int int_fast8_t;






typedef signed int int_fast16_t;






typedef signed long int int_fast24_t;






typedef signed long int int_fast32_t;






typedef signed long long int int_fast64_t;






typedef unsigned int uint_fast8_t;





typedef unsigned int uint_fast16_t;





typedef unsigned long int uint_fast24_t;





typedef unsigned long int uint_fast32_t;





typedef unsigned long long int uint_fast64_t;




typedef long long int intmax_t;
typedef unsigned long long int uintmax_t;
# 41 "./include/btstack/hci_cmds.h" 2
# 237 "./include/btstack/hci_cmds.h"
typedef enum {
    HCI_POWER_OFF = 0,
    HCI_POWER_ON,
 HCI_POWER_SLEEP
} HCI_POWER_MODE;




typedef enum {
    HCI_STATE_OFF = 0,
    HCI_STATE_INITIALIZING,
    HCI_STATE_WORKING,
    HCI_STATE_HALTING,
 HCI_STATE_SLEEPING,
 HCI_STATE_FALLING_ASLEEP
} HCI_STATE;




 typedef struct {
    uint16_t opcode;
    const char *format;
} hci_cmd_t;



extern const hci_cmd_t btstack_get_state;
extern const hci_cmd_t btstack_set_power_mode;
extern const hci_cmd_t btstack_set_acl_capture_mode;
extern const hci_cmd_t btstack_get_version;
extern const hci_cmd_t btstack_get_system_bluetooth_enabled;
extern const hci_cmd_t btstack_set_system_bluetooth_enabled;
extern const hci_cmd_t btstack_set_discoverable;
extern const hci_cmd_t btstack_set_bluetooth_enabled;

extern const hci_cmd_t hci_accept_connection_request;
extern const hci_cmd_t hci_authentication_requested;
extern const hci_cmd_t hci_change_connection_link_key;
extern const hci_cmd_t hci_create_connection;
extern const hci_cmd_t hci_create_connection_cancel;
extern const hci_cmd_t hci_delete_stored_link_key;
extern const hci_cmd_t hci_disconnect;
extern const hci_cmd_t hci_host_buffer_size;
extern const hci_cmd_t hci_inquiry;
extern const hci_cmd_t hci_inquiry_cancel;
extern const hci_cmd_t hci_link_key_request_negative_reply;
extern const hci_cmd_t hci_link_key_request_reply;
extern const hci_cmd_t hci_pin_code_request_reply;
extern const hci_cmd_t hci_pin_code_request_negative_reply;
extern const hci_cmd_t hci_qos_setup;
extern const hci_cmd_t hci_read_bd_addr;
extern const hci_cmd_t hci_read_buffer_size;
extern const hci_cmd_t hci_read_le_host_supported;
extern const hci_cmd_t hci_read_link_policy_settings;
extern const hci_cmd_t hci_read_link_supervision_timeout;
extern const hci_cmd_t hci_read_local_supported_features;
extern const hci_cmd_t hci_read_num_broadcast_retransmissions;
extern const hci_cmd_t hci_reject_connection_request;
extern const hci_cmd_t hci_remote_name_request;
extern const hci_cmd_t hci_remote_name_request_cancel;
extern const hci_cmd_t hci_reset;
extern const hci_cmd_t hci_role_discovery;
extern const hci_cmd_t hci_set_event_mask;
extern const hci_cmd_t hci_set_connection_encryption;
extern const hci_cmd_t hci_sniff_mode;
extern const hci_cmd_t hci_switch_role_command;
extern const hci_cmd_t hci_write_authentication_enable;
extern const hci_cmd_t hci_write_class_of_device;
extern const hci_cmd_t hci_write_extended_inquiry_response;
extern const hci_cmd_t hci_write_inquiry_mode;
extern const hci_cmd_t hci_write_le_host_supported;
extern const hci_cmd_t hci_write_link_policy_settings;
extern const hci_cmd_t hci_write_link_supervision_timeout;
extern const hci_cmd_t hci_write_local_name;
extern const hci_cmd_t hci_write_num_broadcast_retransmissions;
extern const hci_cmd_t hci_write_page_timeout;
extern const hci_cmd_t hci_write_scan_enable;
extern const hci_cmd_t hci_write_simple_pairing_mode;

extern const hci_cmd_t hci_le_add_device_to_whitelist;
extern const hci_cmd_t hci_le_clear_white_list;
extern const hci_cmd_t hci_le_connection_update;
extern const hci_cmd_t hci_le_create_connection;
extern const hci_cmd_t hci_le_create_connection_cancel;
extern const hci_cmd_t hci_le_encrypt;
extern const hci_cmd_t hci_le_long_term_key_negative_reply;
extern const hci_cmd_t hci_le_long_term_key_request_reply;
extern const hci_cmd_t hci_le_rand;
extern const hci_cmd_t hci_le_read_advertising_channel_tx_power;
extern const hci_cmd_t hci_le_read_buffer_size ;
extern const hci_cmd_t hci_le_read_channel_map;
extern const hci_cmd_t hci_le_read_remote_used_features;
extern const hci_cmd_t hci_le_read_supported_features;
extern const hci_cmd_t hci_le_read_supported_states;
extern const hci_cmd_t hci_le_read_white_list_size;
extern const hci_cmd_t hci_le_receiver_test;
extern const hci_cmd_t hci_le_remove_device_from_whitelist;
extern const hci_cmd_t hci_le_set_advertise_enable;
extern const hci_cmd_t hci_le_set_advertising_data;
extern const hci_cmd_t hci_le_set_advertising_parameters;
extern const hci_cmd_t hci_le_set_event_mask;
extern const hci_cmd_t hci_le_set_host_channel_classification;
extern const hci_cmd_t hci_le_set_random_address;
extern const hci_cmd_t hci_le_set_scan_enable;
extern const hci_cmd_t hci_le_set_scan_parameters;
extern const hci_cmd_t hci_le_set_scan_response_data;
extern const hci_cmd_t hci_le_start_encryption;
extern const hci_cmd_t hci_le_test_end;
extern const hci_cmd_t hci_le_transmitter_test;

extern const hci_cmd_t l2cap_accept_connection;
extern const hci_cmd_t l2cap_create_channel;
extern const hci_cmd_t l2cap_create_channel_mtu;
extern const hci_cmd_t l2cap_decline_connection;
extern const hci_cmd_t l2cap_disconnect;
extern const hci_cmd_t l2cap_register_service;
extern const hci_cmd_t l2cap_unregister_service;

extern const hci_cmd_t sdp_register_service_record;
extern const hci_cmd_t sdp_unregister_service_record;


extern const hci_cmd_t rfcomm_accept_connection;

extern const hci_cmd_t rfcomm_create_channel;

extern const hci_cmd_t rfcomm_create_channel_with_initial_credits;

extern const hci_cmd_t rfcomm_decline_connection;

extern const hci_cmd_t rfcomm_disconnect;

extern const hci_cmd_t rfcomm_register_service;

extern const hci_cmd_t rfcomm_register_service_with_initial_credits;

extern const hci_cmd_t rfcomm_unregister_service;

extern const hci_cmd_t rfcomm_persistent_channel_for_service;
# 49 "src/hci.h" 2
# 1 "./include/btstack/utils.h" 1
# 40 "./include/btstack/utils.h"
       






# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 1 3 4
# 201 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 3 4
typedef long long int intmax_t;
typedef unsigned long long int uintmax_t;
# 48 "./include/btstack/utils.h" 2




typedef uint16_t hci_con_handle_t;





typedef uint8_t bd_addr_t[6];





typedef uint8_t link_key_t[16];





typedef uint8_t device_name_t[248 +1];
# 102 "./include/btstack/utils.h"
void bt_store_16(uint8_t *buffer, uint16_t pos, uint16_t value);
void bt_store_32(uint8_t *buffer, uint16_t pos, uint32_t value);
void bt_flip_addr(bd_addr_t dest, bd_addr_t src);

void net_store_16(uint8_t *buffer, uint16_t pos, uint16_t value);
void net_store_32(uint8_t *buffer, uint16_t pos, uint32_t value);

void hexdump(void *data, int size);
void printUUID(uint8_t *uuid);


void print_bd_addr( bd_addr_t addr);
char * bd_addr_to_str(bd_addr_t addr);

int sscan_bd_addr(uint8_t * addr_string, bd_addr_t addr);

uint8_t crc8_check(uint8_t *data, uint16_t len, uint8_t check_sum);
uint8_t crc8_calc(uint8_t *data, uint16_t len);
# 50 "src/hci.h" 2
# 1 "src/hci_transport.h" 1
# 45 "src/hci_transport.h"
       

# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 1 3 4
# 201 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 3 4
typedef long long int intmax_t;
typedef unsigned long long int uintmax_t;
# 48 "src/hci_transport.h" 2
# 1 "./include/btstack/run_loop.h" 1
# 38 "./include/btstack/run_loop.h"
       

# 1 "./config.h" 1
# 41 "./include/btstack/run_loop.h" 2

# 1 "./include/btstack/linked_list.h" 1
# 38 "./include/btstack/linked_list.h"
       





typedef struct linked_item {
    struct linked_item *next;
    void *user_data;
} linked_item_t;

typedef linked_item_t * linked_list_t;

void linked_item_set_user(linked_item_t *item, void *user_data);
void * linked_item_get_user(linked_item_t *item);
int linked_list_empty(linked_list_t * list);
void linked_list_add(linked_list_t * list, linked_item_t *item);
void linked_list_add_tail(linked_list_t * list, linked_item_t *item);
int linked_list_remove(linked_list_t * list, linked_item_t *item);
linked_item_t * linked_list_get_last_item(linked_list_t * list);

void test_linked_list(void);
# 43 "./include/btstack/run_loop.h" 2
# 112 "./include/btstack/run_loop.h"
typedef struct timer {
    linked_item_t item;
    void (*process)(struct timer *ts);
} timer_source_t;
# 49 "src/hci_transport.h" 2






typedef struct {
    int (*open)(void *transport_config);
    int (*close)(void *transport_config);
    int (*send_packet)(uint8_t packet_type, uint8_t *packet, int size);
    void (*register_packet_handler)(void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size));
    const char * (*get_transport_name)(void);

    int (*set_baudrate)(uint32_t baudrate);

    int (*can_send_packet_now)(uint8_t packet_type);
} hci_transport_t;

typedef struct {
    const char *device_name;
    uint32_t baudrate_init;
    uint32_t baudrate_main;
    int flowcontrol;
} hci_uart_config_t;



extern hci_transport_t * hci_transport_h4_instance(void);
extern hci_transport_t * hci_transport_h4_dma_instance(void);
extern hci_transport_t * hci_transport_h4_iphone_instance(void);
extern hci_transport_t * hci_transport_h5_instance(void);
extern hci_transport_t * hci_transport_usb_instance(void);
extern hci_transport_t * hci_transport_mchpusb_instance(void *buf, int size);


extern void hci_transport_h4_iphone_set_enforce_wake_device(char *path);
extern void hci_transport_mchpusb_tasks();
# 51 "src/hci.h" 2
# 1 "src/bt_control.h" 1
# 46 "src/bt_control.h"
       

# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 1 3 4
# 201 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 3 4
typedef long long int intmax_t;
typedef unsigned long long int uintmax_t;
# 49 "src/bt_control.h" 2

typedef enum {
    POWER_WILL_SLEEP = 1,
    POWER_WILL_WAKE_UP
} POWER_NOTIFICATION_t;

typedef struct {
    int (*on) (void *config);
    int (*off) (void *config);
    int (*sleep)(void *config);
    int (*wake) (void *config);
    int (*valid)(void *config);
    const char * (*name) (void *config);




    int (*baudrate_cmd)(void * config, uint32_t baudrate, uint8_t *hci_cmd_buffer);




    int (*next_cmd)(void *config, uint8_t * hci_cmd_buffer);

    void (*register_for_power_notifications)(void (*cb)(POWER_NOTIFICATION_t event));

    void (*hw_error)(void);
} bt_control_t;
# 52 "src/hci.h" 2
# 1 "src/remote_device_db.h" 1
# 41 "src/remote_device_db.h"
       



typedef struct {


    void (*open)(void);
    void (*close)(void);


    int (*get_link_key)(bd_addr_t *bd_addr, link_key_t *link_key);
    void (*put_link_key)(bd_addr_t *bd_addr, link_key_t *key);
    void (*delete_link_key)(bd_addr_t *bd_addr);


    int (*get_name)(bd_addr_t *bd_addr, device_name_t *device_name);
    void (*put_name)(bd_addr_t *bd_addr, device_name_t *device_name);
    void (*delete_name)(bd_addr_t *bd_addr);


    uint8_t (*persistent_rfcomm_channel)(char *servicename);

} remote_device_db_t;

extern remote_device_db_t remote_device_db_iphone;
extern const remote_device_db_t remote_device_db_memory;




typedef struct {

    linked_item_t item;

    bd_addr_t bd_addr;
} db_mem_device_t;

typedef struct {
    db_mem_device_t device;
    link_key_t link_key;
} db_mem_device_link_key_t;

typedef struct {
    db_mem_device_t device;
    char device_name[32];
} db_mem_device_name_t;

typedef struct {

    linked_item_t item;

    char service_name[32];
    uint8_t channel;
} db_mem_service_t;
# 53 "src/hci.h" 2

# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 1 3 4
# 201 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 3 4
typedef long long int intmax_t;
typedef unsigned long long int uintmax_t;
# 55 "src/hci.h" 2

# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdarg.h" 1 3 4





typedef void *va_list;
# 57 "src/hci.h" 2
# 193 "src/hci.h"
typedef enum {
    AUTH_FLAGS_NONE = 0x00,
    RECV_LINK_KEY_REQUEST = 0x01,
    HANDLE_LINK_KEY_REQUEST = 0x02,
    SENT_LINK_KEY_REPLY = 0x04,
    SENT_LINK_KEY_NEGATIVE_REQUEST = 0x08,
    RECV_LINK_KEY_NOTIFICATION = 0x10,
    RECV_PIN_CODE_REQUEST = 0x20,
    SENT_PIN_CODE_REPLY = 0x40,
    SENT_PIN_CODE_NEGATIVE_REPLY = 0x80
} hci_authentication_flags_t;

typedef enum {
    SENT_CREATE_CONNECTION = 1,
    RECEIVED_CONNECTION_REQUEST,
    ACCEPTED_CONNECTION_REQUEST,
    REJECTED_CONNECTION_REQUEST,
    OPEN,
    SENT_DISCONNECT
} CONNECTION_STATE;

typedef enum {
    BLUETOOTH_OFF = 1,
    BLUETOOTH_ON,
    BLUETOOTH_ACTIVE
} BLUETOOTH_STATE;

typedef struct {

    linked_item_t item;


    bd_addr_t address;


    hci_con_handle_t con_handle;


    CONNECTION_STATE state;


    hci_authentication_flags_t authentication_flags;

    timer_source_t timeout;
# 247 "src/hci.h"
    uint8_t acl_recombination_buffer[4 + (4 + 252)];
    uint16_t acl_recombination_pos;
    uint16_t acl_recombination_length;


    uint8_t num_acl_packets_sent;

} hci_connection_t;




typedef struct {

    hci_transport_t * hci_transport;
    void * config;


    bt_control_t * control;


    linked_list_t connections;


    uint8_t hci_packet_buffer[(3 + 255)];


    uint8_t num_cmd_packets;

    uint8_t total_num_acl_packets;
    uint16_t acl_data_packet_length;


    uint16_t packet_types;


    void (*packet_handler)(uint8_t packet_type, uint8_t *packet, uint16_t size);


    remote_device_db_t const*remote_device_db;


    HCI_STATE state;
    uint8_t substate;
    uint8_t cmds_ready;

    uint8_t discoverable;
    uint8_t connectable;


    uint8_t new_scan_enable_value;


    uint8_t decline_reason;
    bd_addr_t decline_addr;

} hci_stack_t;


uint16_t hci_create_cmd(uint8_t *hci_cmd_buffer, hci_cmd_t *cmd, ...);
uint16_t hci_create_cmd_internal(uint8_t *hci_cmd_buffer, const hci_cmd_t *cmd, va_list argptr);


void hci_init(hci_transport_t *transport, void *config, bt_control_t *control, remote_device_db_t const* remote_device_db);
void hci_register_packet_handler(void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size));
void hci_close(void);


int hci_power_control(HCI_POWER_MODE mode);
void hci_discoverable_control(uint8_t enable);
void hci_connectable_control(uint8_t enable);




void hci_run(void);


int hci_send_cmd(const hci_cmd_t *cmd, ...);


int hci_send_cmd_packet(uint8_t *packet, int size);


int hci_send_acl_packet(uint8_t *packet, int size);


int hci_can_send_packet_now(uint8_t packet_type);

hci_connection_t * connection_for_handle(hci_con_handle_t con_handle);
uint8_t hci_number_outgoing_packets(hci_con_handle_t handle);
uint8_t hci_number_free_acl_slots(void);
int hci_authentication_active_for_handle(hci_con_handle_t handle);
void hci_drop_link_key_for_bd_addr(bd_addr_t *addr);
uint16_t hci_max_acl_data_packet_length(void);
uint16_t hci_usable_acl_packet_types(void);
uint8_t* hci_get_outgoing_acl_packet_buffer(void);


void hci_emit_state(void);
void hci_emit_connection_complete(hci_connection_t *conn, uint8_t status);
void hci_emit_l2cap_check_timeout(hci_connection_t *conn);
void hci_emit_disconnection_complete(uint16_t handle, uint8_t reason);
void hci_emit_nr_connections_changed(void);
void hci_emit_hci_open_failed(void);
void hci_emit_btstack_version(void);
void hci_emit_system_bluetooth_enabled(uint8_t enabled);
void hci_emit_remote_name_cached(bd_addr_t *addr, device_name_t *name);
void hci_emit_discoverable_enabled(uint8_t enabled);
# 53 "src/btstack_memory.c" 2
# 1 "src/l2cap.h" 1
# 45 "src/l2cap.h"
       


# 1 "src/l2cap_signaling.h" 1
# 43 "src/l2cap_signaling.h"
       

# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 1 3 4
# 201 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 3 4
typedef long long int intmax_t;
typedef unsigned long long int uintmax_t;
# 46 "src/l2cap_signaling.h" 2




typedef enum {
    COMMAND_REJECT = 1,
    CONNECTION_REQUEST,
    CONNECTION_RESPONSE,
    CONFIGURE_REQUEST,
    CONFIGURE_RESPONSE,
    DISCONNECTION_REQUEST,
    DISCONNECTION_RESPONSE,
    ECHO_REQUEST,
    ECHO_RESPONSE,
    INFORMATION_REQUEST,
    INFORMATION_RESPONSE
} L2CAP_SIGNALING_COMMANDS;

uint16_t l2cap_create_signaling_internal(uint8_t * acl_buffer,hci_con_handle_t handle, L2CAP_SIGNALING_COMMANDS cmd, uint8_t identifier, va_list argptr);
uint8_t l2cap_next_sig_id(void);
uint16_t l2cap_next_local_cid(void);
# 49 "src/l2cap.h" 2

# 1 "./include/btstack/btstack.h" 1
# 41 "./include/btstack/btstack.h"
       





# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 1 3 4
# 201 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 3 4
typedef long long int intmax_t;
typedef unsigned long long int uintmax_t;
# 48 "./include/btstack/btstack.h" 2
# 60 "./include/btstack/btstack.h"
typedef void (*btstack_packet_handler_t) (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);



void bt_use_tcp(const char * address, uint16_t port);


int bt_open(void);


int bt_close(void);


int bt_send_cmd(const hci_cmd_t *cmd, ...);



btstack_packet_handler_t bt_register_packet_handler(btstack_packet_handler_t handler);

void bt_send_acl(uint8_t * data, uint16_t len);

void bt_send_l2cap(uint16_t local_cid, uint8_t *data, uint16_t len);
void bt_send_rfcomm(uint16_t rfcom_cid, uint8_t *data, uint16_t len);
# 51 "src/l2cap.h" 2
# 79 "src/l2cap.h"
void l2cap_init(void);
void l2cap_register_packet_handler(void (*handler)(void * connection, uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size));
void l2cap_create_channel_internal(void * connection, btstack_packet_handler_t packet_handler, bd_addr_t address, uint16_t psm, uint16_t mtu);
void l2cap_disconnect_internal(uint16_t local_cid, uint8_t reason);
uint16_t l2cap_get_remote_mtu_for_local_cid(uint16_t local_cid);
uint16_t l2cap_max_mtu(void);

void l2cap_block_new_credits(uint8_t blocked);
int l2cap_can_send_packet_now(uint16_t local_cid);


uint8_t *l2cap_get_outgoing_buffer(void);

int l2cap_send_prepared(uint16_t local_cid, uint16_t len);
int l2cap_send_internal(uint16_t local_cid, uint8_t *data, uint16_t len);

int l2cap_send_prepared_connectionless(uint16_t handle, uint16_t cid, uint16_t len);
int l2cap_send_connectionless(uint16_t handle, uint16_t cid, uint8_t *data, uint16_t len);

void l2cap_close_connection(void *connection);

void l2cap_register_service_internal(void *connection, btstack_packet_handler_t packet_handler, uint16_t psm, uint16_t mtu);
void l2cap_unregister_service_internal(void *connection, uint16_t psm);

void l2cap_accept_connection_internal(uint16_t local_cid);
void l2cap_decline_connection_internal(uint16_t local_cid, uint8_t reason);


void l2cap_register_fixed_channel(btstack_packet_handler_t packet_handler, uint16_t channel_id);



typedef enum {
    L2CAP_STATE_CLOSED = 1,
    L2CAP_STATE_WILL_SEND_CREATE_CONNECTION,
    L2CAP_STATE_WAIT_CONNECTION_COMPLETE,
    L2CAP_STATE_WAIT_CLIENT_ACCEPT_OR_REJECT,
    L2CAP_STATE_WAIT_CONNECT_RSP,
    L2CAP_STATE_CONFIG,
    L2CAP_STATE_OPEN,
    L2CAP_STATE_WAIT_DISCONNECT,
    L2CAP_STATE_WILL_SEND_CONNECTION_REQUEST,
    L2CAP_STATE_WILL_SEND_CONNECTION_RESPONSE_DECLINE,
    L2CAP_STATE_WILL_SEND_CONNECTION_RESPONSE_ACCEPT,
    L2CAP_STATE_WILL_SEND_DISCONNECT_REQUEST,
    L2CAP_STATE_WILL_SEND_DISCONNECT_RESPONSE,
} L2CAP_STATE;

typedef enum {
    L2CAP_CHANNEL_STATE_VAR_NONE = 0,
    L2CAP_CHANNEL_STATE_VAR_RCVD_CONF_REQ = 1 << 0,
    L2CAP_CHANNEL_STATE_VAR_RCVD_CONF_RSP = 1 << 1,
    L2CAP_CHANNEL_STATE_VAR_SEND_CONF_REQ = 1 << 2,
    L2CAP_CHANNEL_STATE_VAR_SEND_CONF_RSP = 1 << 3,
    L2CAP_CHANNEL_STATE_VAR_SENT_CONF_REQ = 1 << 4,
    L2CAP_CHANNEL_STATE_VAR_SENT_CONF_RSP = 1 << 5,
} L2CAP_CHANNEL_STATE_VAR;


typedef struct {

    linked_item_t item;

    L2CAP_STATE state;
    L2CAP_CHANNEL_STATE_VAR state_var;

    bd_addr_t address;
    hci_con_handle_t handle;

    uint8_t remote_sig_id;
    uint8_t local_sig_id;

    uint16_t local_cid;
    uint16_t remote_cid;

    uint16_t local_mtu;
    uint16_t remote_mtu;

    uint16_t psm;

    uint8_t packets_granted;

    uint8_t reason;


    void * connection;


    btstack_packet_handler_t packet_handler;

} l2cap_channel_t;


typedef struct {

    linked_item_t item;


    uint16_t psm;


    uint16_t mtu;


    void *connection;


    btstack_packet_handler_t packet_handler;

} l2cap_service_t;


typedef struct l2cap_signaling_response {
    hci_con_handle_t handle;
    uint8_t sig_id;
    uint8_t code;
    uint16_t data;
} l2cap_signaling_response_t;
# 54 "src/btstack_memory.c" 2
# 1 "src/rfcomm.h" 1
# 44 "src/rfcomm.h"
# 1 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 1 3 4
# 201 "/opt/microchip/xc16/v1.10/bin/bin/../../include/stdint.h" 3 4
typedef long long int intmax_t;
typedef unsigned long long int uintmax_t;
# 45 "src/rfcomm.h" 2





void rfcomm_init(void);


void rfcomm_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
void rfcomm_register_packet_handler(void (*handler)(void * connection, uint8_t packet_type,
             uint16_t channel, uint8_t *packet, uint16_t size));


void rfcomm_create_channel_internal(void * connection, bd_addr_t *addr, uint8_t channel);
void rfcomm_create_channel_with_initial_credits_internal(void * connection, bd_addr_t *addr, uint8_t server_channel, uint8_t initial_credits);
void rfcomm_disconnect_internal(uint16_t rfcomm_cid);
void rfcomm_register_service_internal(void * connection, uint8_t channel, uint16_t max_frame_size);
void rfcomm_register_service_with_initial_credits_internal(void * connection, uint8_t channel, uint16_t max_frame_size, uint8_t initial_credits);
void rfcomm_unregister_service_internal(uint8_t service_channel);
void rfcomm_accept_connection_internal(uint16_t rfcomm_cid);
void rfcomm_decline_connection_internal(uint16_t rfcomm_cid);
void rfcomm_grant_credits(uint16_t rfcomm_cid, uint8_t credits);
int rfcomm_send_internal(uint16_t rfcomm_cid, uint8_t *data, uint16_t len);
int rfcomm_can_send(uint8_t rfcomm_cid);
void rfcomm_close_connection(void *connection);




typedef enum {
 RFCOMM_MULTIPLEXER_CLOSED = 1,
 RFCOMM_MULTIPLEXER_W4_CONNECT,
 RFCOMM_MULTIPLEXER_SEND_SABM_0,
 RFCOMM_MULTIPLEXER_W4_UA_0,
 RFCOMM_MULTIPLEXER_W4_SABM_0,
    RFCOMM_MULTIPLEXER_SEND_UA_0,
 RFCOMM_MULTIPLEXER_OPEN,
    RFCOMM_MULTIPLEXER_SEND_UA_0_AND_DISC
} RFCOMM_MULTIPLEXER_STATE;

typedef enum {
    MULT_EV_READY_TO_SEND = 1,

} RFCOMM_MULTIPLEXER_EVENT;

typedef enum {
 RFCOMM_CHANNEL_CLOSED = 1,
 RFCOMM_CHANNEL_W4_MULTIPLEXER,
 RFCOMM_CHANNEL_SEND_UIH_PN,
    RFCOMM_CHANNEL_W4_PN_RSP,
 RFCOMM_CHANNEL_SEND_SABM_W4_UA,
 RFCOMM_CHANNEL_W4_UA,
    RFCOMM_CHANNEL_INCOMING_SETUP,
    RFCOMM_CHANNEL_DLC_SETUP,
 RFCOMM_CHANNEL_OPEN,
    RFCOMM_CHANNEL_SEND_UA_AFTER_DISC,
    RFCOMM_CHANNEL_SEND_DISC,
    RFCOMM_CHANNEL_SEND_DM,

} RFCOMM_CHANNEL_STATE;

typedef enum {
    RFCOMM_CHANNEL_STATE_VAR_NONE = 0,
    RFCOMM_CHANNEL_STATE_VAR_CLIENT_ACCEPTED = 1 << 0,
    RFCOMM_CHANNEL_STATE_VAR_RCVD_PN = 1 << 1,
    RFCOMM_CHANNEL_STATE_VAR_RCVD_RPN = 1 << 2,
    RFCOMM_CHANNEL_STATE_VAR_RCVD_SABM = 1 << 3,

    RFCOMM_CHANNEL_STATE_VAR_RCVD_MSC_CMD = 1 << 4,
    RFCOMM_CHANNEL_STATE_VAR_RCVD_MSC_RSP = 1 << 5,
    RFCOMM_CHANNEL_STATE_VAR_SEND_PN_RSP = 1 << 6,
    RFCOMM_CHANNEL_STATE_VAR_SEND_RPN_INFO = 1 << 7,

    RFCOMM_CHANNEL_STATE_VAR_SEND_RPN_RSP = 1 << 8,
    RFCOMM_CHANNEL_STATE_VAR_SEND_UA = 1 << 9,
    RFCOMM_CHANNEL_STATE_VAR_SEND_MSC_CMD = 1 << 10,
    RFCOMM_CHANNEL_STATE_VAR_SEND_MSC_RSP = 1 << 11,

    RFCOMM_CHANNEL_STATE_VAR_SEND_CREDITS = 1 << 12,
    RFCOMM_CHANNEL_STATE_VAR_SENT_MSC_CMD = 1 << 13,
    RFCOMM_CHANNEL_STATE_VAR_SENT_MSC_RSP = 1 << 14,
    RFCOMM_CHANNEL_STATE_VAR_SENT_CREDITS = 1 << 15,
} RFCOMM_CHANNEL_STATE_VAR;

typedef enum {
    CH_EVT_RCVD_SABM = 1,
    CH_EVT_RCVD_UA,
    CH_EVT_RCVD_PN,
    CH_EVT_RCVD_PN_RSP,
    CH_EVT_RCVD_DISC,
    CH_EVT_RCVD_DM,
    CH_EVT_RCVD_MSC_CMD,
    CH_EVT_RCVD_MSC_RSP,
    CH_EVT_RCVD_RPN_CMD,
    CH_EVT_RCVD_RPN_REQ,
    CH_EVT_RCVD_CREDITS,
    CH_EVT_MULTIPLEXER_READY,
    CH_EVT_READY_TO_SEND,
} RFCOMM_CHANNEL_EVENT;

typedef struct rfcomm_channel_event {
    RFCOMM_CHANNEL_EVENT type;
} rfcomm_channel_event_t;

typedef struct rfcomm_channel_event_pn {
    rfcomm_channel_event_t super;
    uint16_t max_frame_size;
    uint8_t priority;
    uint8_t credits_outgoing;
} rfcomm_channel_event_pn_t;

typedef struct rfcomm_rpn_data {
    uint8_t baud_rate;
    uint8_t flags;
    uint8_t flow_control;
    uint8_t xon;
    uint8_t xoff;
    uint8_t parameter_mask_0;
    uint8_t parameter_mask_1;
} rfcomm_rpn_data_t;

typedef struct rfcomm_channel_event_rpn {
    rfcomm_channel_event_t super;
    rfcomm_rpn_data_t data;
} rfcomm_channel_event_rpn_t;


typedef struct {

    linked_item_t item;


    uint8_t server_channel;


    uint16_t max_frame_size;


    uint8_t incoming_flow_control;


    uint8_t incoming_initial_credits;


    void *connection;


    btstack_packet_handler_t packet_handler;

} rfcomm_service_t;



typedef struct {

    linked_item_t item;

    timer_source_t timer;
    int timer_active;

 RFCOMM_MULTIPLEXER_STATE state;

    uint16_t l2cap_cid;
    uint8_t l2cap_credits;

 bd_addr_t remote_addr;
    hci_con_handle_t con_handle;

 uint8_t outgoing;


    uint8_t at_least_one_connection;

    uint16_t max_frame_size;


    uint8_t send_dm_for_dlci;

} rfcomm_multiplexer_t;


typedef struct {

    linked_item_t item;

 rfcomm_multiplexer_t *multiplexer;
 uint16_t rfcomm_cid;
    uint8_t outgoing;
    uint8_t dlci;


    uint8_t packets_granted;


    uint8_t credits_outgoing;


    uint8_t new_credits_incoming;


    uint8_t credits_incoming;


    uint8_t incoming_flow_control;


    RFCOMM_CHANNEL_STATE state;


    RFCOMM_CHANNEL_STATE_VAR state_var;


    uint8_t pn_priority;


    uint16_t max_frame_size;


    rfcomm_rpn_data_t rpn_data;


 rfcomm_service_t * service;


    btstack_packet_handler_t packet_handler;


    void * connection;

} rfcomm_channel_t;
# 55 "src/btstack_memory.c" 2




static hci_connection_t hci_connection_storage[1];
static memory_pool_t hci_connection_pool;
void * btstack_memory_hci_connection_get(void){
    return memory_pool_get(&hci_connection_pool);
}
void btstack_memory_hci_connection_free(void *hci_connection){
    memory_pool_free(&hci_connection_pool, hci_connection);
}
# 87 "src/btstack_memory.c"
static l2cap_service_t l2cap_service_storage[2];
static memory_pool_t l2cap_service_pool;
void * btstack_memory_l2cap_service_get(void){
    return memory_pool_get(&l2cap_service_pool);
}
void btstack_memory_l2cap_service_free(void *l2cap_service){
    memory_pool_free(&l2cap_service_pool, l2cap_service);
}
# 115 "src/btstack_memory.c"
static l2cap_channel_t l2cap_channel_storage[(1+1)];
static memory_pool_t l2cap_channel_pool;
void * btstack_memory_l2cap_channel_get(void){
    return memory_pool_get(&l2cap_channel_pool);
}
void btstack_memory_l2cap_channel_free(void *l2cap_channel){
    memory_pool_free(&l2cap_channel_pool, l2cap_channel);
}
# 143 "src/btstack_memory.c"
static rfcomm_multiplexer_t rfcomm_multiplexer_storage[1];
static memory_pool_t rfcomm_multiplexer_pool;
void * btstack_memory_rfcomm_multiplexer_get(void){
    return memory_pool_get(&rfcomm_multiplexer_pool);
}
void btstack_memory_rfcomm_multiplexer_free(void *rfcomm_multiplexer){
    memory_pool_free(&rfcomm_multiplexer_pool, rfcomm_multiplexer);
}
# 171 "src/btstack_memory.c"
static rfcomm_service_t rfcomm_service_storage[1];
static memory_pool_t rfcomm_service_pool;
void * btstack_memory_rfcomm_service_get(void){
    return memory_pool_get(&rfcomm_service_pool);
}
void btstack_memory_rfcomm_service_free(void *rfcomm_service){
    memory_pool_free(&rfcomm_service_pool, rfcomm_service);
}
# 199 "src/btstack_memory.c"
static rfcomm_channel_t rfcomm_channel_storage[1];
static memory_pool_t rfcomm_channel_pool;
void * btstack_memory_rfcomm_channel_get(void){
    return memory_pool_get(&rfcomm_channel_pool);
}
void btstack_memory_rfcomm_channel_free(void *rfcomm_channel){
    memory_pool_free(&rfcomm_channel_pool, rfcomm_channel);
}
# 236 "src/btstack_memory.c"
void * btstack_memory_db_mem_device_name_get(void){
    return (0);
}
void btstack_memory_db_mem_device_name_free(void *db_mem_device_name){
};
# 255 "src/btstack_memory.c"
static db_mem_device_link_key_t db_mem_device_link_key_storage[2];
static memory_pool_t db_mem_device_link_key_pool;
void * btstack_memory_db_mem_device_link_key_get(void){
    return memory_pool_get(&db_mem_device_link_key_pool);
}
void btstack_memory_db_mem_device_link_key_free(void *db_mem_device_link_key){
    memory_pool_free(&db_mem_device_link_key_pool, db_mem_device_link_key);
//src/btstack_memory.c: In function 'btstack_memory_db_mem_device_link_key_free':
//src/btstack_memory.c:262:1: internal compiler error: Segmentation fault
}
# 292 "src/btstack_memory.c"
void * btstack_memory_db_mem_service_get(void){
    return (0);
}
void btstack_memory_db_mem_service_free(void *db_mem_service){
};
# 308 "src/btstack_memory.c"
void btstack_memory_init(void){

    memory_pool_create(&hci_connection_pool, hci_connection_storage, 1, sizeof(hci_connection_t));


    memory_pool_create(&l2cap_service_pool, l2cap_service_storage, 2, sizeof(l2cap_service_t));


    memory_pool_create(&l2cap_channel_pool, l2cap_channel_storage, (1+1), sizeof(l2cap_channel_t));


    memory_pool_create(&rfcomm_multiplexer_pool, rfcomm_multiplexer_storage, 1, sizeof(rfcomm_multiplexer_t));


    memory_pool_create(&rfcomm_service_pool, rfcomm_service_storage, 1, sizeof(rfcomm_service_t));


    memory_pool_create(&rfcomm_channel_pool, rfcomm_channel_storage, 1, sizeof(rfcomm_channel_t));





    memory_pool_create(&db_mem_device_link_key_pool, db_mem_device_link_key_storage, 2, sizeof(db_mem_device_link_key_t));




}
