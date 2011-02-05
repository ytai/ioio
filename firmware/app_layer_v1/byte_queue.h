#ifndef __BYTEQUEUE_H__
#define __BYTEQUEUE_H__

#include "GenericTypeDefs.h"
#include "p24Fxxxx.h"


typedef struct {
  BYTE* buf;
  int capacity;
  int read_cursor;
  int write_cursor;
  int size;
} ByteQueue;

#define DEFINE_STATIC_BYTE_QUEUE(name, size)              \
  static BYTE name##_buf[size];                           \
  static ByteQueue name = { name##_buf, size, 0, 0, 0 }

#define ByteQueueLock(q) BYTE q##ipl_save = SRbits.IPL; SRbits.IPL = 1;
#define ByteQueueUnlock(q) SRbits.IPL = q##ipl_save; 

void ByteQueuePushBuffer(ByteQueue* q, const void* buf, int len);
void ByteQueuePeek(ByteQueue* q, BYTE** data, int* size);
void ByteQueuePull(ByteQueue* q, int size);

#endif  // __BYTEQUEUE_H__
