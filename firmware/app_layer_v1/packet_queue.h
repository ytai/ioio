#ifndef __PACKETQUEUE_H__
#define __PACKETQUEUE_H__

#include <assert.h>
#include "byte_queue.h"

typedef struct {
  BYTE_QUEUE byte_queue;
  BYTE packet_read_bytes_remaining;
  BYTE dest;
  BYTE num_complete_packets;
} PACKET_QUEUE;

static inline void PacketQueueInit(PACKET_QUEUE* q, BYTE* buf, int capacity) {
  ByteQueueInit(&q->byte_queue, buf, capacity);
  q->packet_read_bytes_remaining = 0;
  q->num_complete_packets = 0;
}

static inline void PacketQueueClear(PACKET_QUEUE* q) {
  ByteQueueClear(&q->byte_queue);
  q->packet_read_bytes_remaining = 0;
  q->num_complete_packets = 0;
}

static inline void PacketQueuePush(PACKET_QUEUE* q, BYTE dest, BYTE size,
                                   const void* data) {
  assert(size);
  ByteQueuePushByte(&q->byte_queue, dest);
  ByteQueuePushByte(&q->byte_queue, size);
  ByteQueuePushBuffer(&q->byte_queue, data, size);
  ++q->num_complete_packets;
}

static inline void PacketQueuePushByte(PACKET_QUEUE* q, BYTE b) {
  ByteQueuePushByte(&q->byte_queue, b);
}

static inline void PacketQueuePacketWriteDone(PACKET_QUEUE* q) {
  ++q->num_complete_packets;
}

static inline void PacketQueueStartRead(PACKET_QUEUE* q) {
  assert(q->packet_read_bytes_remaining == 0 && q->num_complete_packets != 0);
  q->dest = ByteQueuePullByte(&q->byte_queue);
  q->packet_read_bytes_remaining = ByteQueuePullByte(&q->byte_queue);
}

static inline int PacketQueueHasData(PACKET_QUEUE* q) {
  return q->num_complete_packets != 0;
}

static inline BYTE PacketQueueRead(PACKET_QUEUE* q) {
  assert(q->packet_read_bytes_remaining > 0);
  BYTE data = ByteQueuePullByte(&q->byte_queue);
  if (--q->packet_read_bytes_remaining == 0) {
    --q->num_complete_packets;
  }
  return data;
}

static inline void PacketQueuePull(PACKET_QUEUE* q, int size) {
  assert(q->packet_read_bytes_remaining >= size);
  ByteQueuePull(&q->byte_queue, size);
  q->packet_read_bytes_remaining -= size;
  if (q->packet_read_bytes_remaining == 0) {
    --q->num_complete_packets;
  }
}

static inline void PacketQueuePeek(PACKET_QUEUE* q, const BYTE** data1,
                                   int* size1, const BYTE** data2,
                                   int* size2) {
  ByteQueuePeekAll(&q->byte_queue, data1, size1, data2, size2);
  if (*size1 > q->packet_read_bytes_remaining) {
    *size1 = q->packet_read_bytes_remaining;
    *size2 = 0;
  } else if (*size1 + *size2 > q->packet_read_bytes_remaining) {
    *size2 = q->packet_read_bytes_remaining - *size1;
  }
}

static inline BYTE PacketQueueCurrentDest(PACKET_QUEUE* q) {
  return q->dest;
}

static inline int PacketQueueReadRemaining(PACKET_QUEUE* q) {
  return q->packet_read_bytes_remaining;
}

static inline int PacketQueueRemaining(PACKET_QUEUE* q) {
  return ByteQueueRemaining(&q->byte_queue);
}

#endif  // __PACKETQUEUE_H__
