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

#include <assert.h>
#include "Compiler.h"
#include "flash.h"

BOOL FlashErasePage(DWORD address) {
  assert((address & 0x3FF) == 0);
  DWORD_VAL a = { address };

  NVMCON = 0x4042;  // Erase page

  TBLPAG = a.byte.UB;
  __builtin_tblwtl(a.word.LW, 0xFFFF);

  asm("DISI #5");
  __builtin_write_NVM();
  while(NVMCONbits.WR);
  NVMCONbits.WREN = 0;
  return NVMCONbits.WRERR == 0;
}	

BOOL FlashWriteDWORD(DWORD address, DWORD value)	{
  assert((address & 0x1) == 0);
  DWORD_VAL a = { address };
  DWORD_VAL v = { value };

  NVMCON = 0x4003;  // Word write

  TBLPAG = a.word.HW;
  __builtin_tblwtl(a.word.LW, v.word.LW);		//Write the low word to the latch
  __builtin_tblwth(a.word.LW, v.word.HW); 	//Write the high word to the latch (8 bits of data + 8 bits of "phantom data")

  asm("DISI #5");
  __builtin_write_NVM();
  while (NVMCONbits.WR);
  NVMCONbits.WREN = 0;
  return NVMCONbits.WRERR == 0;
}

BOOL FlashWriteBlock(DWORD address, const BYTE block[192]) {
  assert((address & 0x7F) == 0);
  unsigned int i = 0;
  DWORD_VAL a = { address };
  DWORD_VAL v;

  NVMCON = 0x4001;  // Block write

  TBLPAG = a.word.HW;
  while (i < 192) {
    v.byte.LB = block[i++];
    v.byte.HB = block[i++];
    v.byte.UB = block[i++];
    __builtin_tblwtl(a.word.LW, v.word.LW);  // Write the low word to the latch
    __builtin_tblwth(a.word.LW, v.word.HW);  // Write the high word to the latch (8 bits of data + 8 bits of "phantom data")
    a.word.LW += 2;
  }

  asm("DISI #5");
  __builtin_write_NVM();
  while (NVMCONbits.WR);
  NVMCONbits.WREN = 0;
  return NVMCONbits.WRERR == 0;
}

DWORD FlashReadDWORD(DWORD address) {
  DWORD_VAL a = { address };
  DWORD_VAL res;
  TBLPAG = a.word.HW;
  res.word.HW = __builtin_tblrdh(a.word.LW);
  res.word.LW = __builtin_tblrdl(a.word.LW);
  return res.Val;
}

