#include <assert.h>
#include "Compiler.h"
#include "flash.h"

void FlashErasePage(DWORD address) {
  assert((address & 0x3FF) == 0);
	DWORD_VAL a = { address };

	NVMCON = 0x4042;  // Erase page

	TBLPAG = a.byte.UB;
	__builtin_tblwtl(a.word.LW, 0xFFFF);

	asm("DISI #5");
	__builtin_write_NVM();
  while(NVMCONbits.WR);
	NVMCONbits.WREN = 0;
}	

void FlashWriteDWORD(DWORD address, DWORD value)	{
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
}

void FlashWriteBlock(DWORD address, WORD block[128]) {
  assert((address & 0x7F) == 0);
  unsigned int i = 0;
	DWORD_VAL a = { address };

	NVMCON = 0x4001;  // Block write

	TBLPAG = a.word.HW;
  while (i < 128) {
  	__builtin_tblwtl(a.word.LW, block[i++]);		//Write the low word to the latch
	  __builtin_tblwth(a.word.LW, block[i++]);  	//Write the high word to the latch (8 bits of data + 8 bits of "phantom data")
    a.word.LW += 2;
  }

	asm("DISI #5");
	__builtin_write_NVM();
  while (NVMCONbits.WR);
	NVMCONbits.WREN = 0;
}

DWORD FlashReadDWORD(DWORD address) {
  DWORD_VAL a = { address };
  DWORD_VAL res;
  TBLPAG = a.word.HW;
  res.word.HW = __builtin_tblrdh(a.word.LW);
  res.word.LW = __builtin_tblrdl(a.word.LW);
  return res.Val;
}

