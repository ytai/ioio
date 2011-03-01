// A generic byte queue, used for data buffering.

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
} BYTE_QUEUE;

#define DEFINE_STATIC_BYTE_QUEUE(name, size)              \
  static BYTE name##_buf[size];                           \
  static BYTE_QUEUE name = { name##_buf, size, 0, 0, 0 }

static inline void ByteQueueClear(BYTE_QUEUE* q) {
  q->size = 0;
  q->read_cursor = 0;
  q->write_cursor = 0;
}

static inline void ByteQueueInit(BYTE_QUEUE* q, BYTE* buf, int capacity) {
  q->buf = buf;
  q->capacity = capacity;
  ByteQueueClear(q);
}

void ByteQueuePushBuffer(BYTE_QUEUE* q, const void* buf, int len);
void ByteQueuePeek(BYTE_QUEUE* q, const BYTE** data, int* size);
//void ByteQueuePeekAll(BYTE_QUEUE* q, const BYTE** data1, int* size1,
//                      const BYTE** data2, int* size2);
void ByteQueuePeekMax(BYTE_QUEUE* q, int max_size, const BYTE** data1,
                      int* size1, const BYTE** data2, int* size2);
void ByteQueuePull(BYTE_QUEUE* q, int size);

void ByteQueuePushByte(BYTE_QUEUE* q, BYTE b);
BYTE ByteQueuePullByte(BYTE_QUEUE* q);

static inline int ByteQueueSize(BYTE_QUEUE* q) { return q->size; }
static inline int ByteQueueRemaining(BYTE_QUEUE* q) { return q->capacity - q->size; }

#endif  // __BYTEQUEUE_H__
