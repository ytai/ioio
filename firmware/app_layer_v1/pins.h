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

#ifndef __PINS_H__
#define __PINS_H__

#include "platform.h"

extern unsigned int CNENB;
extern unsigned int CNENC;
extern unsigned int CNEND;
extern unsigned int CNENE;
extern unsigned int CNENF;
extern unsigned int CNENG;

extern unsigned int CNBACKUPB;
extern unsigned int CNBACKUPC;
extern unsigned int CNBACKUPD;
extern unsigned int CNBACKUPE;
extern unsigned int CNBACKUPF;
extern unsigned int CNBACKUPG;

extern unsigned int CNFORCEB;
extern unsigned int CNFORCEC;
extern unsigned int CNFORCED;
extern unsigned int CNFORCEE;
extern unsigned int CNFORCEF;
extern unsigned int CNFORCEG;

// TODO: pin capabilities

void PinSetTris(int pin, int val);
void PinSetAnsel(int pin, int val);
void PinSetLat(int pin, int val);
int PinGetPort(int pin);
void PinSetOdc(int pin, int val);
void PinSetCnen(int pin, int cnen);
void PinSetCnforce(int pin);
void PinSetCnpu(int pin, int cnpu);
void PinSetCnpd(int pin, int cnpd);
void PinSetRpor(int pin, int per);
int PinToRpin(int pin);

int PinFromPortB(int bit);
int PinFromPortC(int bit);
int PinFromPortD(int bit);
int PinFromPortE(int bit);
int PinFromPortF(int bit);
int PinFromPortG(int bit);

int PinToAnalogChannel(int pin);
int PinFromAnalogChannel(int ch);

#endif  // __PINS_H__
