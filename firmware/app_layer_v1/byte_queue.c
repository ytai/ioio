#include "byte_queue.h"

#include <assert.h>
#include <string.h>
#include "logging.h"

void ByteQueueOverflow() {
  // TODO: do something
  log_printf("Buffer overflow!");
}

void ByteQueuePushByte(BYTE_QUEUE* q, BYTE b) {
  if (q->size == q->capacity) {
    ByteQueueOverflow();
    return;
  }
  q->buf[q->write_cursor++] = b;
  if (q->write_cursor == q->capacity) {
    q->write_cursor = 0;
  }
  ++q->size;
}

BYTE ByteQueuePullByte(BYTE_QUEUE* q) {
  BYTE ret;
  assert(q->size);
  ret = q->buf[q->read_cursor++];
  if (q->read_cursor == q->capacity) {
    q->read_cursor = 0;
  }
  --q->size;
  return ret;
}

void ByteQueuePushBuffer(BYTE_QUEUE* q, const void* buf, int len) {
  if (q->size + len > q->capacity) {
    ByteQueueOverflow();
    return;
  }
  if (q->write_cursor + len <= q->capacity) {
    memcpy(q->buf + q->write_cursor, buf, len);
    q->write_cursor += len;
    if (q->write_cursor == q->capacity) {
      q->write_cursor -= q->capacity;
    }
  } else {
    int size_first = q->capacity - q->write_cursor;
    memcpy(q->buf + q->write_cursor, buf, size_first);
    memcpy(q->buf, ((const BYTE*) buf) + size_first, len - size_first);
    q->write_cursor += len - q->capacity;
  }
  q->size += len;
}

void ByteQueuePeek(BYTE_QUEUE* q, const BYTE** data, int* size) {
  *data = q->buf + q->read_cursor;
  if (q->write_cursor < q->read_cursor) {
    *size = q->capacity - q->read_cursor;
  } else {
    *size = q->write_cursor - q->read_cursor;
  }
}

void ByteQueuePull(BYTE_QUEUE* q, int size) {
  assert(size <= q->size);
  q->read_cursor += size;
  q->size -= size;
  if (q->read_cursor >= q->capacity) {
    q->read_cursor -= q->capacity;
  }
}
