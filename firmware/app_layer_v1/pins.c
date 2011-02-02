#include "pins.h"
#include <p24fxxxx.h>

#define SFR volatile unsigned int

unsigned int CNENB = 0x0000;
unsigned int CNENC = 0x0000;
unsigned int CNEND = 0x0000;
unsigned int CNENE = 0x0000;
unsigned int CNENF = 0x0000;
unsigned int CNENG = 0x0000;

unsigned int CNBACKUPB = 0x0000;
unsigned int CNBACKUPC = 0x0000;
unsigned int CNBACKUPD = 0x0000;
unsigned int CNBACKUPE = 0x0000;
unsigned int CNBACKUPF = 0x0000;
unsigned int CNBACKUPG = 0x0000;

typedef struct {
  SFR* tris;
  SFR* ansel;
  SFR* port;
  SFR* lat;
  SFR* odc;
  unsigned int* fake_cnen;
  unsigned int* cn_backup;
  unsigned int pos_mask;
  unsigned int neg_mask;
} PORT_INFO;

#if defined(IOIO_V10) || defined(IOIO_V11) || defined(IOIO_V12)
#define ANSE (*((SFR*) 0))  // hack: there is no ANSE register on 64-pin devices
#endif

#define MAKE_PORT_INFO(port, num) { &TRIS##port, &ANS##port, &PORT##port, &LAT##port, &ODC##port, &CNEN##port, &CNBACKUP##port, (1 << num), ~(1 << num) }

typedef struct {
  SFR* cnen;
  SFR* cnpu;
  SFR* cnpd;
  unsigned int pos_mask;
  unsigned int neg_mask;
} CN_INFO;

#define MAKE_CN_INFO(num, bit) { &CNEN##num, &CNPU##num, &CNPD##num, (1 << bit), ~(1 << bit) }
#define MAKE_RPOR(num) (((unsigned char*) &RPOR0) + num)

#if defined(IOIO_V10) || defined(IOIO_V11) || defined(IOIO_V12)
  const PORT_INFO port_info[NUM_PINS] = {
    MAKE_PORT_INFO(F, 3),   // LED
    MAKE_PORT_INFO(C, 12),  // 1
    MAKE_PORT_INFO(C, 15),  // 2
    MAKE_PORT_INFO(D, 8),   // 3
    MAKE_PORT_INFO(D, 9),   // 4
    MAKE_PORT_INFO(D, 10),  // 5
    MAKE_PORT_INFO(D, 11),  // 6
    MAKE_PORT_INFO(D, 0),   // 7
    MAKE_PORT_INFO(C, 13),  // 8
    MAKE_PORT_INFO(C, 14),  // 9
    MAKE_PORT_INFO(D, 1),   // 10
    MAKE_PORT_INFO(D, 2),   // 11
    MAKE_PORT_INFO(D, 3),   // 12
    MAKE_PORT_INFO(D, 4),   // 13
    MAKE_PORT_INFO(D, 5),   // 14
    MAKE_PORT_INFO(D, 6),   // 15
    MAKE_PORT_INFO(D, 7),   // 16
    MAKE_PORT_INFO(F, 0),   // 17
    MAKE_PORT_INFO(F, 1),   // 18
    MAKE_PORT_INFO(E, 0),   // 19
    MAKE_PORT_INFO(E, 1),   // 20
    MAKE_PORT_INFO(E, 2),   // 21
    MAKE_PORT_INFO(E, 3),   // 22
    MAKE_PORT_INFO(E, 4),   // 23
    MAKE_PORT_INFO(E, 5),   // 24
    MAKE_PORT_INFO(E, 6),   // 25
    MAKE_PORT_INFO(E, 7),   // 26
    MAKE_PORT_INFO(G, 6),   // 27
    MAKE_PORT_INFO(G, 7),   // 28
    MAKE_PORT_INFO(G, 8),   // 29
  #ifdef IOIO_V10
    { 0, 0, 0, 0, 0, 0}, // MCLR (30)
  #endif  // IOIO_V10
    MAKE_PORT_INFO(G, 9),   // 30 (31)
    MAKE_PORT_INFO(B, 5),   // 31 (32)
    MAKE_PORT_INFO(B, 4),   // 32 (33)
    MAKE_PORT_INFO(B, 3),   // 33 (34)
    MAKE_PORT_INFO(B, 2),   // 34 (35)
    MAKE_PORT_INFO(B, 1),   // 35 (36)
    MAKE_PORT_INFO(B, 0),   // 36 (37)
    MAKE_PORT_INFO(B, 6),   // 37 (38)
    MAKE_PORT_INFO(B, 7),   // 38 (39)
    MAKE_PORT_INFO(B, 8),   // 39 (40)
    MAKE_PORT_INFO(B, 9),   // 40 (41)
    MAKE_PORT_INFO(B, 10),  // 41 (42)
    MAKE_PORT_INFO(B, 11),  // 42 (43)
    MAKE_PORT_INFO(B, 12),  // 43 (44)
    MAKE_PORT_INFO(B, 13),  // 44 (45)
    MAKE_PORT_INFO(B, 14),  // 45 (46)
    MAKE_PORT_INFO(B, 15),  // 46 (47)
    MAKE_PORT_INFO(F, 4),   // 47 (48)
    MAKE_PORT_INFO(F, 5),   // 48 (49)
  };

  const CN_INFO cn_info[NUM_PINS] = {
    MAKE_CN_INFO(5, 7),  // LED
    MAKE_CN_INFO(2, 7),  // 1
    MAKE_CN_INFO(2, 6),  // 2
    MAKE_CN_INFO(4, 5),  // 3
    MAKE_CN_INFO(4, 6),  // 4
    MAKE_CN_INFO(4, 7),  // 5
    MAKE_CN_INFO(4, 8),  // 6
    MAKE_CN_INFO(4, 1),  // 7
    MAKE_CN_INFO(1, 1),  // 8
    MAKE_CN_INFO(1, 0),  // 9
    MAKE_CN_INFO(4, 2),  // 10
    MAKE_CN_INFO(4, 3),  // 11
    MAKE_CN_INFO(4, 4),  // 12
    MAKE_CN_INFO(1, 13),  // 13
    MAKE_CN_INFO(1, 14),  // 14
    MAKE_CN_INFO(1, 15),  // 15
    MAKE_CN_INFO(2, 0),  // 16
    MAKE_CN_INFO(5, 4),  // 17
    MAKE_CN_INFO(5, 5),  // 18
    MAKE_CN_INFO(4, 10),  // 19
    MAKE_CN_INFO(4, 11),  // 20
    MAKE_CN_INFO(4, 12),  // 21
    MAKE_CN_INFO(4, 13),  // 22
    MAKE_CN_INFO(4, 14),  // 23
    MAKE_CN_INFO(4, 15),  // 24
    MAKE_CN_INFO(5, 0),  // 25
    MAKE_CN_INFO(5, 1),  // 26
    MAKE_CN_INFO(1, 8),  // 27
    MAKE_CN_INFO(1, 9),  // 28
    MAKE_CN_INFO(1, 10),  // 29
  #ifdef IOIO_V10
    { 0, 0, 0, 0, 0}, // MCLR (30)
  #endif  // IOIO_V10
    MAKE_CN_INFO(1, 11),  // 30 (31)
    MAKE_CN_INFO(1, 7),  // 31 (32)
    MAKE_CN_INFO(1, 6),  // 32 (33)
    MAKE_CN_INFO(1, 5),  // 33 (34)
    MAKE_CN_INFO(1, 4),  // 34 (35)
    MAKE_CN_INFO(1, 3),  // 35 (36)
    MAKE_CN_INFO(1, 2),  // 36 (37)
    MAKE_CN_INFO(2, 8),  // 37 (38)
    MAKE_CN_INFO(2, 9),  // 38 (39)
    MAKE_CN_INFO(2, 10),  // 39 (40)
    MAKE_CN_INFO(2, 11),  // 40 (41)
    MAKE_CN_INFO(2, 12),  // 41 (42)
    MAKE_CN_INFO(2, 13),  // 42 (43)
    MAKE_CN_INFO(2, 14),  // 43 (44)
    MAKE_CN_INFO(2, 15),  // 44 (45)
    MAKE_CN_INFO(3, 0),  // 45 (46)
    MAKE_CN_INFO(1, 12),  // 46 (47)
    MAKE_CN_INFO(2, 1),  // 47 (48)
    MAKE_CN_INFO(2, 2),  // 48 (49)
  };

  #if defined(IOIO_V10)
    static const signed char port_to_pin[7][16] = {
//        0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
/* B */ {37, 36, 35, 34, 33, 32, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47},
/* C */ {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  1,  8,  9,  2},
/* D */ { 7, 10, 11, 12, 13, 14, 15, 16,  3,  4,  5,  6, -1, -1, -1, -1},
/* E */ {19, 20, 21, 22, 23, 24, 25, 26, -1, -1, -1, -1, -1, -1, -1, -1},
/* F */ {17, 18, -1,  0, 48, 49, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
/* G */ {-1, -1, -1, -1, -1, -1, 27, 28, 29, 31, -1, -1, -1, -1, -1, -1}
    };
  #elif defined(IOIO_V11) || defined(IOIO_V12)
    static const signed char port_to_pin[7][16] = {
    static const signed char port_to_pin[7][16] = {
//        0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
/* B */ {36, 35, 34, 33, 32, 31, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46},
/* C */ {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  1,  8,  9,  2},
/* D */ { 7, 10, 11, 12, 13, 14, 15, 16,  3,  4,  5,  6, -1, -1, -1, -1},
/* E */ {19, 20, 21, 22, 23, 24, 25, 26, -1, -1, -1, -1, -1, -1, -1, -1},
/* F */ {17, 18, -1,  0, 47, 48, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
/* G */ {-1, -1, -1, -1, -1, -1, 27, 28, 29, 30, -1, -1, -1, -1, -1, -1}
    };
  #endif

  volatile unsigned char* pin_to_rpor[NUM_PINS] = {
    MAKE_RPOR(16),
    0,
    0,
    MAKE_RPOR(2),
    MAKE_RPOR(4),
    MAKE_RPOR(3),
    MAKE_RPOR(12),
    MAKE_RPOR(11),
    0,
    0,
    MAKE_RPOR(24),
    MAKE_RPOR(23),
    MAKE_RPOR(22),
    MAKE_RPOR(25),
    MAKE_RPOR(20),
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    MAKE_RPOR(21),
    MAKE_RPOR(26),
    MAKE_RPOR(19),
  #ifdef IOIO_V10
    0, // MCLR (30)
  #endif  // IOIO_V10
    MAKE_RPOR(27),
    MAKE_RPOR(18),
    MAKE_RPOR(28),
    0,
    MAKE_RPOR(13),
    MAKE_RPOR(1),
    MAKE_RPOR(0),
    MAKE_RPOR(6),
    MAKE_RPOR(7),
    MAKE_RPOR(8),
    MAKE_RPOR(9),
    0,
    0,
    0,
    0,
    MAKE_RPOR(14),
    MAKE_RPOR(29),
    MAKE_RPOR(10),
    MAKE_RPOR(17),
  };
#endif  // defined(IOIO_V10) || defined(IOIO_V11) || defined(IOIO_V12)

void PinSetTris(int pin, int val) {
  const PORT_INFO* info = &port_info[pin];
  if (val) {
    *info->tris |= info->pos_mask;
  } else {
    *info->tris &= info->neg_mask;
  }
}

void PinSetAnsel(int pin, int val) {
  const PORT_INFO* info = &port_info[pin];
  if (val) {
    *info->ansel |= info->pos_mask;
  } else {
    *info->ansel &= info->neg_mask;
  }
}

void PinSetLat(int pin, int val) {
  const PORT_INFO* info = &port_info[pin];
  if (val) {
    *info->lat |= info->pos_mask;
  } else {
    *info->lat &= info->neg_mask;
  }
}

int PinGetPort(int pin) {
  const PORT_INFO* info = &port_info[pin];
  return (*info->port & info->pos_mask) != 0;
}

void PinSetOdc(int pin, int val) {
  const PORT_INFO* info = &port_info[pin];
  if (val) {
    *info->odc |= info->pos_mask;
  } else {
    *info->odc &= info->neg_mask;
  }
}

void PinSetCnen(int pin, int cnen) {
  const CN_INFO* cinfo = &cn_info[pin];
  const PORT_INFO* pinfo = &port_info[pin];
  _CNIE = 0;  // disable CN interrupts
  if (cnen) {
    *cinfo->cnen |= cinfo->pos_mask;
    *pinfo->fake_cnen |= pinfo->pos_mask;
    // copy the pin bit from the port register to the backup register
    *pinfo->cn_backup ^= ((*pinfo->cn_backup ^ *pinfo->port)
                              & pinfo->pos_mask);
  } else {
    *cinfo->cnen &= cinfo->neg_mask;
    *port_info->fake_cnen &= port_info->neg_mask;
  }
  _CNIE = 1;  // enable CN interrupts
}

void PinSetCnpu(int pin, int cnpu) {
  const CN_INFO* info = &cn_info[pin];
  if (cnpu) {
    *info->cnpu |= info->pos_mask;
  } else {
    *info->cnpu &= info->neg_mask;
  }
}

void PinSetCnpd(int pin, int cnpd) {
  const CN_INFO* info = &cn_info[pin];
  if (cnpd) {
    *info->cnpd |= info->pos_mask;
  } else {
    *info->cnpd &= info->neg_mask;
  }
}

void PinSetRpor(int pin, int per) {
  *pin_to_rpor[pin] = per;
}

int PinFromPortB(int bit) { return port_to_pin[0][bit]; };
int PinFromPortC(int bit) { return port_to_pin[1][bit]; };
int PinFromPortD(int bit) { return port_to_pin[2][bit]; };
int PinFromPortE(int bit) { return port_to_pin[3][bit]; };
int PinFromPortF(int bit) { return port_to_pin[4][bit]; };
int PinFromPortG(int bit) { return port_to_pin[5][bit]; };
