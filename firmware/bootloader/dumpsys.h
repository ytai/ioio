#ifndef __DUMPSYS_H__
#define __DUMPSYS_H__

static const char* DUMPSYS_ERROR = (const char *) -1;
static const char* DUMPSYS_BUSY = (const char *) 0;

void DumpsysInit();
const char* DumpsysProcess(const char* data, int size);


#endif  // __DUMPSYS_H__
