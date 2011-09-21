#include "arch/lwbtopts.h"
#include "lwbt/phybusif.h"
#include "lwbt/hci.h"
#include "lwip/debug.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"

#include "usb_host_bluetooth.h"
#include "logging.h"
#include "usb_host.h"
#include "HardwareProfile.h"
#include "timer.h"

static struct pbuf *cmd_first = NULL, *acl_first = NULL;

void phybusif_init() {
  if (cmd_first) pbuf_free(cmd_first);
  if (acl_first) pbuf_free(acl_first);
  cmd_first = acl_first = NULL;
}

void phybusif_output(struct pbuf *p, u16_t len) {
  if (*(u8_t*) p->payload == HCI_COMMAND_DATA_PACKET) {
    pbuf_header(p, -1);
    if (cmd_first) {
      pbuf_chain(cmd_first, p);
    } else {
      pbuf_ref(p);
      cmd_first = p;
    }
  } else if (*(u8_t*) p->payload == HCI_ACL_DATA_PACKET) {
    pbuf_header(p, -1);
    if (acl_first) {
      pbuf_chain(acl_first, p);
    } else {
      pbuf_ref(p);
      acl_first = p;
    }
  }
}

struct pbuf *phybusif_next_command() {
  if (cmd_first) {
    struct pbuf *tmp = cmd_first;
    pbuf_ref(cmd_first->next);
    cmd_first = pbuf_dechain(cmd_first);
    return tmp;
  }
  return NULL;
}

struct pbuf *phybusif_next_acl() {
  if (acl_first) {
    struct pbuf *tmp = acl_first;
    pbuf_ref(acl_first->next);
    acl_first = pbuf_dechain(acl_first);
    return tmp;
  }
  return NULL;
}

void bt_init(void) {
  //sys_init();
  mem_init();
  memp_init();
  pbuf_init();
  log_printf("mem mgmt initialized\r\n");
  lwbt_memp_init();
  phybusif_init();

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
