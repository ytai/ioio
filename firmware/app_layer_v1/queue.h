/*
 * Copyright 2013 Ytai Ben-Tsvi. All rights reserved.
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

#ifndef QUEUE_H
#define	QUEUE_H


#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

#define QUEUE(T, SIZE) struct { \
  volatile int read_count;      \
  volatile int write_count;     \
  T buf[SIZE];                  \
}

#define QUEUE_CLEAR(q) (q)->read_count = (q)->write_count = 0
#define QUEUE_SIZE(q) ((unsigned) ((q)->write_count - (q)->read_count))
#define QUEUE_CAPACITY(q) ARRAY_LEN((q)->buf)

#define QUEUE_PEEK(q) (&(q)->buf[(q)->read_count % QUEUE_CAPACITY(q)])
#define QUEUE_POKE(q) (&(q)->buf[(q)->write_count % QUEUE_CAPACITY(q)])
#define QUEUE_PULL(q) (++(q)->read_count)
#define QUEUE_PUSH(q) (++(q)->write_count)


#endif	// QUEUE_H

