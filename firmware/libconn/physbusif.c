#include "arch/lwbtopts.h"
#include "lwbt/phybusif.h"
#include "lwbt/hci.h"
#include "lwip/debug.h"
#include "lwip/mem.h"

#include "usb_host_bluetooth.h"
#include "logging.h"
#include "usb_host.h"
#include "HardwareProfile.h"
#include "timer.h"

void phybusif_output(struct pbuf *p, u16_t len) {
  static unsigned char buf[256];

  //	static unsigned char *ptr;
  //	unsigned char c;
  u16_t total_len;

  /* Send pbuf */
  total_len = pbuf_copy_partial(p, buf, sizeof buf, 1);

  if (*(u8_t*) p->payload == HCI_COMMAND_DATA_PACKET) {
    LWIP_DEBUGF(PHYBUSIF_DEBUG, ("sending cmd len %02x\n", total_len));
    USBHostBluetoothWriteControl(buf, total_len);
    DelayMs(10);  // TEMPTEMPTEMP
  } else if (*(u8_t*) p->payload == HCI_ACL_DATA_PACKET) {
    LWIP_DEBUGF(PHYBUSIF_DEBUG, ("sending acl len %02x\n", total_len));
    USBHostBluetoothWriteBulk(buf, total_len);
    DelayMs(10);  // TEMPTEMPTEMP
  }
}


void bt_init(void) {
  //sys_init();
  mem_init();
  memp_init();
  pbuf_init();
  log_printf("mem mgmt initialized\r\n");
  lwbt_memp_init();

  if (hci_init() != ERR_OK) {
    log_printf("HCI initialization failed!\r\n");
    return;
  }
  l2cap_init();
  sdp_init();
  rfcomm_init();
  log_printf("Bluetooth initialized.\r\n");

  bt_spp_start();
  log_printf("Applications started.\r\n");
}
