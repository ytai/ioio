#ifndef __FLASH_H__
#define __FLASH_H__

#include "GenericTypeDefs.h"


void FlashErasePage(DWORD address);
void FlashWriteDWORD(DWORD address, DWORD value);
void FlashWriteBlock(DWORD address, BYTE block[192]);
DWORD FlashReadDWORD(DWORD address);


#endif  // __FLASH_H__
