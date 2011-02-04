#ifndef __PINS_H__
#define __PINS_H__

#include "board.h"

extern unsigned int CNENA;
extern unsigned int CNENB;
extern unsigned int CNENC;
extern unsigned int CNEND;
extern unsigned int CNENE;
extern unsigned int CNENF;
extern unsigned int CNENG;

extern unsigned int CNBACKUPA;
extern unsigned int CNBACKUPB;
extern unsigned int CNBACKUPC;
extern unsigned int CNBACKUPD;
extern unsigned int CNBACKUPE;
extern unsigned int CNBACKUPF;
extern unsigned int CNBACKUPG;

// TODO: pin capabilities

void PinSetTris(int pin, int val);
void PinSetAnsel(int pin, int val);
void PinSetLat(int pin, int val);
int PinGetPort(int pin);
void PinSetOdc(int pin, int val);
void PinSetCnen(int pin, int cnen);
void PinSetCnpu(int pin, int cnpu);
void PinSetCnpd(int pin, int cnpd);
void PinSetRpor(int pin, int per);

int PinFromPortB(int bit);
int PinFromPortC(int bit);
int PinFromPortD(int bit);
int PinFromPortE(int bit);
int PinFromPortF(int bit);
int PinFromPortG(int bit);

int PinToAnalogChannel(int pin);
int PinFromAnalogChannel(int ch);

#endif  // __PINS_H__
