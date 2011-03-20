#include "incap.h"

#include "Compiler.h"
#include "board.h"
#include "logging.h"
#include "pp_util.h"

DEFINE_REG_SETTERS_1B(NUM_INCAP_MODULES, _IC, IF)
DEFINE_REG_SETTERS_1B(NUM_INCAP_MODULES, _IC, IE)
DEFINE_REG_SETTERS_1B(NUM_INCAP_MODULES, _IC, IP)

typedef struct {
  unsigned int con1;
  unsigned int con2;
  unsigned int buf;
  unsigned int tmr;
} INCAP_REG;

volatile INCAP_REG* incap_regs = (volatile INCAP_REG *) &IC1CON1;

void InCapInit() {
  log_printf("InCapInit()");
  int i;
  for (i = 0; i < NUM_INCAP_MODULES; ++i) {
    InCapConfig(i, 0, 0, 0, 0);
  }
}

void InCapConfig(int incap_num, int mode, int continouos, int clock_scale,
                 int input_scale) {
  log_printf("InCapConfig(%d, %d, %d, %d, %d)", incap_num, mode, continouos,
             clock_scale, input_scale);
  Set_ICIF[2](0);
}
