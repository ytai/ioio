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

#define ByteQueueLock(q) __asm__("push SR"); SRbits.IPL = 1;
#define ByteQueueUnlock(q) __asm__("pop SR"); 


void ByteQueuePushByte(ByteQueue* q, BYTE b);
void ByteQueuePushBuffer(ByteQueue* q, const void* buf, int len);

static inline void ByteQueuePushWord(ByteQueue* q, WORD w) {
  ByteQueuePushBuffer(q, &w, 2);
}

static inline void ByteQueuePushInt24(ByteQueue* q, DWORD dw) {
  ByteQueuePushBuffer(q, &dw, 3);
}

static inline void ByteQueuePushDword(ByteQueue* q, DWORD dw) {
  ByteQueuePushBuffer(q, &dw, 4);
}

void ByteQueuePeek(ByteQueue* q, BYTE** data, int* size);

void ByteQueuePull(ByteQueue* q, int size);

#endif  // __BYTEQUEUE_H__
