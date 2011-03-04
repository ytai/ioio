#include "i2c.h"

#include "Compiler.h"
#include "board.h"

typedef struct {
  unsigned int rcv;
  unsigned int trn;
  unsigned int brg;
  unsigned int con;
  unsigned int stat;
  unsigned int add;
  unsigned int mask;
} I2CREG;

#if NUM_I2C_MODULES != 3
  #error The code below assumes 3 I2C modules. Please fix.
#endif
volatile I2CREG* i2c_reg[NUM_I2C_MODULES] = {
  (volatile I2CREG*) &I2C1RCV,
  (volatile I2CREG*) &I2C2RCV,
  (volatile I2CREG*) &I2C3RCV
};

// The macro magic below generates for each type of flag from
// {IE, IF, IP}
// an array of function pointers for setting this flag, where each element in
// the array corresponds to a single I2C module, in master mode.
//
// For example, to set IE of I2C2 as master to 1, call:
// SetMIE[1](1);
#define MI2C_FLAG_FUNC(i2c, flag) static void SetM##flag##i2c(int val) { _MI2C##i2c##flag = val; }

typedef void (*I2CFlagFunc)(int val);

#if NUM_I2C_MODULES != 3
  #error Currently only devices with 3 I2C modules are supported. Please fix below.
#endif

#define ALL_I2C_FLAG_FUNC(flag)                                 \
  MI2C_FLAG_FUNC(1, flag)                                       \
  MI2C_FLAG_FUNC(2, flag)                                       \
  MI2C_FLAG_FUNC(3, flag)                                       \
  I2CFlagFunc SetM##flag[NUM_I2C_MODULES] = { &SetM##flag##1,   \
                                              &SetM##flag##2,   \
                                              &SetM##flag##3 };

ALL_I2C_FLAG_FUNC(IE)
ALL_I2C_FLAG_FUNC(IF)
ALL_I2C_FLAG_FUNC(IP)


void I2CInit() {
  int i;
  for (i = 0; i < NUM_I2C_MODULES; ++i) {
    SetMIP[i](4);  // interrupt priority 4
    I2CConfigMaster(i, 0, 0);
  }
}

void I2CConfigMaster(int i2c_num, int rate, int smbus_levels) {
  volatile I2CREG* regs = i2c_reg[i2c_num];
  static const unsigned int brg_values[] = { 0x9D, 0x25, 0x0D };
  
  SetMIE[i2c_num](0);  // disable interrupt
  regs->con = 0x0000;  // disable module
  SetMIF[i2c_num](0);  // clear interrupt
  if (rate) {
    regs->brg = brg_values[rate - 1];
    regs->con = (1 << 15)               // enable
                | ((rate != 2) << 9)    // disable slew rate unless 400KHz mode
                | (smbus_levels << 8);  // use SMBus levels
   SetMIE[i2c_num](1);  // enable interrupt
  }
}

void I2CWriteRead(int i2c_num, int addr, const void* data, int data_bytes,
                  int read_bytes, int ack_last_read) {
}

static inline void MI2CInterrupt(int i2c_num) {
}

#define DEFINE_INTERRUPT_HANDLERS(i2c_num)                                     \
  void __attribute__((__interrupt__, auto_psv)) _MI2C##i2c_num##Interrupt() {  \
    MI2CInterrupt(i2c_num - 1);                                                \
  }

#if NUM_I2C_MODULES > 3
  #error Currently only devices with 3 or less I2Cs are supported. Please fix below.
#endif

#if NUM_I2C_MODULES >= 1
  DEFINE_INTERRUPT_HANDLERS(1)
#endif

#if NUM_I2C_MODULES >= 2
  DEFINE_INTERRUPT_HANDLERS(2)
#endif

#if NUM_I2C_MODULES >= 3
  DEFINE_INTERRUPT_HANDLERS(3)
#endif
