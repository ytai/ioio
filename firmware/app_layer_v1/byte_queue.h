/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */

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
  static BYTE name##_buf[size] __attribute__((far));      \
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
void ByteQueuePullToBuffer(BYTE_QUEUE* q, void* buffer, int size);

void ByteQueuePushByte(BYTE_QUEUE* q, BYTE b);
BYTE ByteQueuePullByte(BYTE_QUEUE* q);

static inline int ByteQueueSize(BYTE_QUEUE* q) { return q->size; }
static inline int ByteQueueRemaining(BYTE_QUEUE* q) { return q->capacity - q->size; }

#endif  // __BYTEQUEUE_H__
