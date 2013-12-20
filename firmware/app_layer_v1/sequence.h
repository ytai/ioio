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

#ifndef SEQUENCE_H
#define SEQUENCE_H


#include <assert.h>
#include <stdint.h>

#include "platform.h"
#include "queue.h"

#define SEQUENCE_LENGTH 32

typedef struct {
  uint16_t rs;
  uint16_t r;
  uint16_t con1;
  uint16_t inc;
} OCCue;

typedef struct {
  uint16_t and;
  uint16_t or;
} PortCue;

typedef struct {
  OCCue   oc[NUM_PWM_MODULES];
  PortCue port[NUM_PORTS];
} Cue;

typedef struct {
  Cue      cue;
  uint16_t time;
} TimedCue;

typedef QUEUE(TimedCue, SEQUENCE_LENGTH) Sequence;

extern inline void SequenceClear(Sequence *seq) {
  QUEUE_CLEAR(seq);
}

extern inline unsigned SequenceSize(Sequence const *seq) {
  return QUEUE_SIZE(seq);
}

extern inline bool SequenceFull(Sequence const *seq) {
  return QUEUE_SIZE(seq) == QUEUE_CAPACITY(seq);
}

extern inline TimedCue * SequencePoke(Sequence *seq) {
  assert(QUEUE_SIZE(seq) < QUEUE_CAPACITY(seq));
  return QUEUE_POKE(seq);
}

extern inline TimedCue const * SequencePeek(Sequence const *seq) {
  if (QUEUE_SIZE(seq) == 0) return NULL;
  return QUEUE_PEEK(seq);
}

extern inline void SequencePull(Sequence *seq) {
  QUEUE_PULL(seq);
}

extern inline void SequencePush(Sequence *seq) {
  QUEUE_PUSH(seq);
}

#endif	// SEQUENCE_H

