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

#ifndef SEQUENCER_H
#define	SEQUENCER_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Settings

typedef enum {
  CHANNEL_TYPE_PWM_POSITION = 0,
  CHANNEL_TYPE_PWM_SPEED    = 1,
  CHANNEL_TYPE_FM_SPEED     = 2,
  CHANNEL_TYPE_STEPS        = 3,
  CHANNEL_TYPE_BINARY       = 4
} ChannelType;

typedef enum {
  CLK_16M  = 0,
  CLK_2M   = 1,
  CLK_250K = 2,
  CLK_62K5 = 3
} ClockSource;

typedef struct __attribute__((packed)) {
  uint8_t oc_num  : 4;
  ClockSource clk : 2;
  uint8_t         : 2;
  uint16_t period;
  uint16_t init_pw;
} ChannelSettingsPwm;

typedef struct __attribute__((packed)) {
  uint8_t oc_num  : 4;
  ClockSource clk : 2;
  uint8_t         : 2;
  uint16_t pw;
} ChannelSettingsFm;

typedef struct __attribute__((packed)) {
  uint8_t oc_num : 4;
  uint8_t        : 4;
} ChannelSettingsSteps;

typedef struct __attribute__((packed)) {
  uint8_t pin         : 6;
  bool init           : 1;
  bool init_when_idle : 1;
} ChannelSettingsBinary;

typedef struct __attribute__((packed)) {
  ChannelType type : 4;
  uint8_t          : 4;
  union {
    ChannelSettingsPwm pwm;
    ChannelSettingsFm fm;
    ChannelSettingsSteps steps;
    ChannelSettingsBinary binary;
  };
} ChannelSettings;

////////////////////////////////////////////////////////////////////////////////
// Cues

typedef struct __attribute__((packed)) {
  uint16_t pw;
} ChannelCuePwm;

typedef struct __attribute__((packed)) {
  uint16_t period;
} ChannelCueFm;

typedef struct __attribute__((packed)) {
  ClockSource clk : 2;
  uint8_t         : 6;
  uint16_t pw;
  uint16_t period;
} ChannelCueSteps;

typedef struct __attribute__((packed)) {
  bool value : 1;
  uint8_t    : 7;
} ChannelCueBinary;

typedef union {
  ChannelCuePwm pwm;
  ChannelCueFm fm;
  ChannelCueSteps steps;
  ChannelCueBinary binary;
} ChannelCue;

////////////////////////////////////////////////////////////////////////////////
// Events

typedef enum {
  // Never change the first two. The assembly code relies on these special
  // values!
  SEQ_EVENT_PAUSED   = 0,
  SEQ_EVENT_STALLED  = 1,
  SEQ_EVENT_OPENED   = 2,
  SEQ_EVENT_NEXT_CUE = 3,
  SEQ_EVENT_STOPPED  = 4,
  SEQ_EVENT_CLOSED   = 5,
  SEQ_EVENT_NONE     = 6
} SequencerEvent;

void SequencerInit();

// An array of ChannelSettings, without the padding bytes introduced by the
// union.
bool SequencerOpen(uint8_t const *settings, size_t size);

// An array of ChannelCue, without the padding bytes introduced by the
// union.
bool SequencerPush(uint8_t const *cue, uint16_t time);

bool SequencerClose();

bool SequencerStart();

bool SequencerPause();

bool SequencerStop();

// An array of ChannelCue, without the padding bytes introduced by the
// union.
bool SequencerManualCue(uint8_t const *cue);

size_t SequencerExpectedCueSize();

bool SequencerManualStop();

uint8_t SequencerGetEvent();

size_t SequencerQueueLength();

#endif	// SEQUENCER_H

