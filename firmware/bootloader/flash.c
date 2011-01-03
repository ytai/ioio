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

BOOL FlashWriteBlock(DWORD address, BYTE block[192]) {
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
  	__builtin_tblwtl(a.word.LW, v.word.LW);		//Write the low word to the latch
	  __builtin_tblwth(a.word.LW, v.word.HW);  	//Write the high word to the latch (8 bits of data + 8 bits of "phantom data")
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

