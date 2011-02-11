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

// TODO: get these out of here - they are not buffer-related
#define ByteQueueLock(lock, level) do { lock = SRbits.IPL; SRbits.IPL = level; } while (0)
#define ByteQueueUnlock(lock) do { SRbits.IPL = lock; } while (0)

#define ByteQueueInit(q, b, cap) do { (q)->buf = b; (q)->capacity = cap; (q)->size = 0; (q)->read_cursor = 0; (q)->write_cursor = 0; } while(0)

void ByteQueuePushBuffer(ByteQueue* q, const void* buf, int len);
void ByteQueuePeek(ByteQueue* q, const BYTE** data, int* size);
void ByteQueuePull(ByteQueue* q, int size);

void ByteQueuePushByte(ByteQueue* q, BYTE b);
BYTE ByteQueuePullByte(ByteQueue* q);

static inline int ByteQueueSize(ByteQueue* q) { return q->size; }
static inline int ByteQueueRemaining(ByteQueue* q) { return q->capacity - q->size; }

#endif  // __BYTEQUEUE_H__
