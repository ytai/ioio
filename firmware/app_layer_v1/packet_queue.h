#ifndef __PACKETQUEUE_H__
#define __PACKETQUEUE_H__

#include <assert.h>
#include "byte_queue.h"

typedef struct {
  BYTE_QUEUE byte_queue;
  BYTE bytes_remaining;
  BYTE dest;
} PACKET_QUEUE;

static inline void PacketQueueInit(PACKET_QUEUE* q, BYTE* buf, int capacity) {
  ByteQueueInit(&q->byte_queue, buf, capacity);
  q->bytes_remaining = 0;
}

static inline void PacketQueueClear(PACKET_QUEUE* q) {
  ByteQueueClear(&q->byte_queue);
  q->bytes_remaining = 0;
}

static inline void PacketQueuePush(PACKET_QUEUE* q, BYTE dest, BYTE size,
                                   const void* data) {
  assert(size);
  ByteQueuePushByte(&q->byte_queue, dest);
  ByteQueuePushByte(&q->byte_queue, size);
  ByteQueuePushBuffer(&q->byte_queue, data, size);
}

static inline int PacketQueueHasData(PACKET_QUEUE* q) {
  return ByteQueueSize(&q->byte_queue) != 0;
}

static inline int PacketQueueRead(PACKET_QUEUE* q, BYTE* data) {
  if (q->bytes_remaining == 0) {
    // new packet starting
    q->dest = ByteQueuePullByte(&q->byte_queue);
    q->bytes_remaining = ByteQueuePullByte(&q->byte_queue);
  }
  *data = ByteQueuePullByte(&q->byte_queue);
  return --q->bytes_remaining;
}

static inline BYTE PacketQueueCurrentDest(PACKET_QUEUE* q) {
  return q->dest;
}

#endif  // __PACKETQUEUE_H__
