#include "pins.h"
#include <p24fxxxx.h>

#define SFR volatile unsigned int

typedef struct {
  SFR* tris;
  SFR* port;
  SFR* lat;
  SFR* odc;
  unsigned int set_mask;
  unsigned int clr_mask;
} PORT_INFO;

#define MAKE_PORT_INFO(port, num) { &TRIS##port, &PORT##port, &LAT##port, &ODC##port, (1 << num), ~(1 << num) }

typedef struct {
  SFR* cnen;
  SFR* cnpu;
  SFR* cnpd;
  unsigned int set_mask;
  unsigned int clr_mask;
} CN_INFO;

#define MAKE_CN_INFO(num, bit) { &CNEN##num, &CNPU##num, &CNPD##num, (1 << bit), ~(1 << bit) }

#if defined(IOIO_V10) || defined(IOIO_V11)
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
#endif  // defined(IOIO_V10) || defined(IOIO_V11)

void PinSetTris(int pin, int val) {
  const PORT_INFO* info = &port_info[pin];
  if (val) {
    *info->tris |= info->set_mask;
  } else {
    *info->tris &= info->clr_mask;
  }
}

void PinSetLat(int pin, int val) {
  const PORT_INFO* info = &port_info[pin];
  if (val) {
    *info->lat |= info->set_mask;
  } else {
    *info->lat &= info->clr_mask;
  }
}

int PinGetPort(int pin) {
  const PORT_INFO* info = &port_info[pin];
  return (*info->port & info->set_mask) != 0;
}

void PinSetOdc(int pin, int val) {
  const PORT_INFO* info = &port_info[pin];
  if (val) {
    *info->odc |= info->set_mask;
  } else {
    *info->odc &= info->clr_mask;
  }
}

void PinSetCnen(int pin, int cnen) {
  const CN_INFO* info = &cn_info[pin];
  if (cnen) {
    *info->cnen |= info->set_mask;
  } else {
    *info->cnen &= info->clr_mask;
  }
}

void PinSetCnpu(int pin, int cnpu) {
  const CN_INFO* info = &cn_info[pin];
  if (cnpu) {
    *info->cnpu |= info->set_mask;
  } else {
    *info->cnpu &= info->clr_mask;
  }
}

void PinSetCnpd(int pin, int cnpd) {
  const CN_INFO* info = &cn_info[pin];
  if (cnpd) {
    *info->cnpd |= info->set_mask;
  } else {
    *info->cnpd &= info->clr_mask;
  }
}
