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

#include "sequencer.h"

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <p24Fxxxx.h>
#include <libpic30.h>

#include "logging.h"
#include "platform.h"
#include "sequence.h"
#include "pins.h"
#include "pp_util.h"
#include "sync.h"

#define INT_PRIORITY 7

////////////////////////////////////////////////////////////////////////////////

static QUEUE(uint8_t, 16) event_queue;

void PushEvent(uint8_t e) {
  PRIORITY(INT_PRIORITY) {
    *QUEUE_POKE(&event_queue) = e;
    QUEUE_PUSH(&event_queue);
  }
}

////////////////////////////////////////////////////////////////////////////////

typedef struct {
  uint16_t oc_enabled;
  uint16_t oc_discrete;
  uint16_t oc_idle_change;
  Cue idle_cue;
} ChannelConfig;

uint16_t sequencer_running __attribute__((near));
uint16_t sequencer_should_run __attribute__((near));
Sequence the_sequence __attribute__((far));
ChannelConfig the_config __attribute__((near));
static bool sequencer_open = false;
static size_t expected_cue_size;

////////////////////////////////////////////////////////////////////////////////
// Debug utilities.

#if 0
static void PrintCue(Cue const *cue, uint16_t oc_enabled) {
  size_t i;
  for (i = 0; i < ARRAY_LEN(cue->oc); ++i) {
    if (oc_enabled & (1 << i)) {
      printf("OC%d { %u %u 0x%x %u } ", i, cue->oc[i].r, cue->oc[i].rs, cue->oc[i].con1, cue->oc[i].inc);
    }
  }
  for (i = 0; i < ARRAY_LEN(cue->port); ++i) {
      printf("P%d { &0x%x |0x%x } ", i, cue->port[i].and, cue->port[i].or);
  }
  printf("\n");
}

static void PrintConfig(ChannelConfig const *cfg) {
  printf("ChannelConfig\n"
         "-------------\n");
#define PRINT(field) printf(#field " = 0x%04x\n", cfg->field)
  PRINT(oc_enabled);
  PRINT(oc_discrete);
  PRINT(oc_idle_change);
  PrintCue(&cfg->idle_cue);
#undef PRINT
}

static void PrintSequence(Sequence const *seq) {
  printf("Sequence\n"
         "--------\n");
  size_t i;
  for (i = 0; i < SequenceSize(seq); ++i) {
    TimedCue const * const timed_cue =
        &seq->buf[(seq->read_count + i) % SEQUENCE_LENGTH];
    printf("Duration = %d\n", timed_cue->time);
    PrintCue(&timed_cue->cue);
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////
// Low-level stuff.
// These are the functions that actually write to the LAT and OC registers.
// They are implemented in assembly for determinitic timing and performance.

extern void Freeze(Cue const *idle_cue,
                   uint16_t oc_enabled,
                   uint16_t oc_discrete,
                   uint16_t oc_idle_change);

extern void ProcessCue(Cue const *cue,
                       uint16_t oc_enabled,
                       uint16_t oc_discrete);

extern void ProcessStart(Cue const *cue,
                         uint16_t oc_enabled,
                         uint16_t oc_discrete);

////////////////////////////////////////////////////////////////////////////////
// High-level stuff.
// These are the functions that manage operation at the sequence level.
//

#define MAX_CHANNELS 32

typedef struct {
  uint8_t oc_num;
  uint16_t rs;
  uint16_t con1;
} InternalChannelSettingsPwm;

typedef struct {
  uint8_t oc_num;
  uint16_t r;
  uint16_t con1;
} InternalChannelSettingsFm;

typedef struct {
  uint8_t oc_num;
} InternalChannelSettingsSteps;

typedef struct {
  PORT port;
  uint8_t nbit;
} InternalChannelSettingsBinary;

typedef struct {
  ChannelType type;
  union {
    InternalChannelSettingsPwm pwm;
    InternalChannelSettingsFm fm;
    InternalChannelSettingsSteps steps;
    InternalChannelSettingsBinary binary;
  };
} InternalChannelSettings;

static InternalChannelSettings channel_settings[MAX_CHANNELS];
static size_t num_active_channels;

static bool ConvertClock(ClockSource clk, uint16_t *result) {
  assert(result);
  switch (clk) {
    case CLK_16M:  *result = 0b111; return true;
    case CLK_2M :  *result = 0b001; return true;
    case CLK_250K: *result = 0b010; return true;
    case CLK_62K5: *result = 0b011; return true;
    default:                        return false;
  }
}

static void ClearCue(Cue *cue) {
  size_t i;
  memset(cue, 0, sizeof(Cue));
  for (i = 0; i < ARRAY_LEN(cue->port); ++i) {
    cue->port[i].and = ~0;
  }
}

static void ClearConfig(ChannelConfig *config) {
  memset(config, 0, sizeof(ChannelConfig));
  ClearCue(&config->idle_cue);
  expected_cue_size = 0;
}

static int TranslateSettings(uint8_t const * data,
                             size_t max_size,
                             InternalChannelSettings * result,
                             ChannelConfig * config,
                             size_t * cue_size) {
  assert(data);
  assert(max_size);
  assert(result);
  assert(config);
  assert(cue_size);
  
  ChannelSettings const *settings = (ChannelSettings const *) data;
  uint16_t cclk = 0;
  result->type = settings->type;

  // Set the channel settings.
  log_printf("TranslateSettings, type=%d", settings->type);
  switch (settings->type) {
    case CHANNEL_TYPE_PWM_POSITION:
    case CHANNEL_TYPE_PWM_SPEED:
      if (max_size < 1 + sizeof(ChannelSettingsPwm)) return -1;
      if (settings->pwm.oc_num >= NUM_PWM_MODULES) return -1;
      if (settings->pwm.period < 2) return -1;
      if (!ConvertClock(settings->pwm.clk, &cclk)) return -1;
      result->pwm.oc_num = settings->pwm.oc_num;
      result->pwm.rs = settings->pwm.period - 1;
      result->pwm.con1 = (cclk << 10) | 0b110;
      config->oc_enabled |= 1 << settings->pwm.oc_num;
      if (settings->type == CHANNEL_TYPE_PWM_SPEED) {
        config->oc_idle_change |= 1 << settings->pwm.oc_num;
      }
      // In position mode, the idle queue is just used for initialization.
      // In speed-mode the idle cue is used for freezing too.
      config->idle_cue.oc[settings->pwm.oc_num].r =
          settings->pwm.init_pw ? settings->pwm.init_pw - 1 : 0;
      config->idle_cue.oc[settings->pwm.oc_num].rs = settings->pwm.period - 1;
      config->idle_cue.oc[settings->pwm.oc_num].con1 = result->pwm.con1;

      *cue_size = sizeof(ChannelCuePwm);
      return 1 + sizeof(ChannelSettingsPwm);

    case CHANNEL_TYPE_FM_SPEED:
      if (max_size < 1 + sizeof(ChannelSettingsFm)) return -1;
      if (settings->fm.oc_num >= NUM_PWM_MODULES) return -1;
      if (settings->fm.pw < 2) return -1;
      if (!ConvertClock(settings->fm.clk, &cclk)) return -1;
      result->fm.oc_num = settings->fm.oc_num;
      result->fm.r = settings->fm.pw - 1;
      result->fm.con1 = (cclk << 10) | 0b110;
      config->oc_enabled |= 1 << settings->fm.oc_num;
      config->oc_idle_change |= 1 << settings->fm.oc_num;

      config->idle_cue.oc[settings->fm.oc_num].r = 0;
      config->idle_cue.oc[settings->fm.oc_num].rs = 1;
      config->idle_cue.oc[settings->fm.oc_num].con1 = result->fm.con1;

      *cue_size = sizeof(ChannelCueFm);
      return 1 + sizeof(ChannelSettingsFm);

    case CHANNEL_TYPE_STEPS:
      if (max_size < 1 + sizeof(ChannelSettingsSteps)) return -1;
      if (settings->pwm.oc_num >= NUM_PWM_MODULES) return -1;
      result->pwm.oc_num = settings->pwm.oc_num;
      config->oc_enabled |= 1 << settings->pwm.oc_num;
      config->oc_discrete |= 1 << settings->pwm.oc_num;

      *cue_size = sizeof(ChannelCueSteps);
      return 1 + sizeof(ChannelSettingsSteps);

    case CHANNEL_TYPE_BINARY:
      if (max_size < 1 + sizeof(ChannelSettingsBinary)) return -1;
      if (settings->binary.pin > NUM_PINS) return -1;
      int nbit;
      PORT port;
      PinToPortBit(settings->binary.pin, &port, &nbit);
      result->binary.port = port;
      result->binary.nbit = nbit;
      if (settings->binary.init_when_idle) {
        if (settings->binary.init) {
          config->idle_cue.port[port].or |= 1 << nbit;
        } else {
          config->idle_cue.port[port].and &= ~(1 << nbit);
        }
      }

      *cue_size = sizeof(ChannelCueBinary);
      return 1 + sizeof(ChannelSettingsBinary);

    default:
      return -1;
  }
}

static bool Configure(uint8_t const *settings, size_t size) {

  ClearConfig(&the_config);
  num_active_channels = 0;

  while (size) {
    if (num_active_channels == MAX_CHANNELS) return false;
    size_t cue_size;

    int const result = TranslateSettings(settings,
                                         size,
                                         &channel_settings[num_active_channels],
                                         &the_config,
                                         &cue_size);
    if (result <= 0) return false;

    ++num_active_channels;
    expected_cue_size += cue_size;
    size -= result;
    settings += result;
  }
  return true;
}

bool TranslateCue(uint8_t const *data, Cue *result) {
  assert(result);

  ClearCue(result);

  size_t i;
  for (i = 0; i < num_active_channels; ++i) {
    ChannelCue const *cue = (ChannelCue const *) data;

    switch (channel_settings[i].type) {
      case CHANNEL_TYPE_PWM_POSITION:
      case CHANNEL_TYPE_PWM_SPEED:
        {
          OCCue * const oc = &result->oc[channel_settings[i].pwm.oc_num];
          oc->con1 = channel_settings[i].pwm.con1;
          oc->rs   = channel_settings[i].pwm.rs;
          oc->r    = cue->pwm.pw ? cue->pwm.pw - 1 : 0;
          oc->inc  = 0;
          data += sizeof(ChannelCuePwm);
        }
        break;

      case CHANNEL_TYPE_FM_SPEED:
        {
          OCCue * const oc = &result->oc[channel_settings[i].fm.oc_num];
          oc->con1 = channel_settings[i].fm.con1;
          oc->r    = cue->fm.period > 1 ? channel_settings[i].fm.r : 0;
          oc->rs   = cue->fm.period > 1 ? cue->fm.period - 1 : 1;
          oc->inc  = 0;
          data += sizeof(ChannelCueFm);
        }
        break;

      case CHANNEL_TYPE_STEPS:
        {
          uint16_t cclk = 0;
          if (!ConvertClock(cue->steps.clk, &cclk)) return false;
          OCCue * const oc = &result->oc[channel_settings[i].steps.oc_num];
          if (cue->steps.pw) {
            oc->rs   = (cue->steps.period + cue->steps.pw - 1) / 2;
            oc->r    = oc->rs - cue->steps.pw;
            oc->con1 = (cclk << 10) | 0b111;
            oc->inc  = (cue->steps.period - cue->steps.pw) / 2;
          } else {
            oc->con1 = 0;
          }
          data += sizeof(ChannelCueSteps);
        }
        break;

      case CHANNEL_TYPE_BINARY:
        {
          PortCue * const port_cue =
              &result->port[channel_settings[i].binary.port];
          if (cue->binary.value) {
            port_cue->or |= (1 << channel_settings[i].binary.nbit);
          } else {
            port_cue->and &= ~(1 << channel_settings[i].binary.nbit);
          }
          data += sizeof(ChannelCueBinary);
        }
        break;

      default:
        return false;
    }
  }
  return true;
}

bool SequencerPush(uint8_t const *cue, uint16_t time) {
  log_printf("SequencerPush(0x%p, %u)", cue, time);
  log_print_buf(cue, SequencerExpectedCueSize());
  assert(cue);

  if (!sequencer_open) return false;
  if (SequenceFull(&the_sequence)) return false;
  
  TimedCue *new_cue = SequencePoke(&the_sequence);
  new_cue->time = time;
  if (!TranslateCue(cue, &new_cue->cue)) return false;
  // PrintCue(&new_cue->cue, the_config.oc_enabled);

  SequencePush(&the_sequence);
  if (!sequencer_running && sequencer_should_run) {
    SequencerStart();
  }
  return true;
}


bool SequencerOpen(uint8_t const *settings, size_t size) {
  log_printf("SequencerOpen(0x%p, %u)", settings, size);
  log_print_buf(settings, size);
  assert(!size || settings);
  
  if (sequencer_open) return false;
  if (!Configure(settings, size)) return false;

  // PrintConfig(&the_config);

  ProcessStart(&the_config.idle_cue,
               the_config.oc_enabled,
               the_config.oc_discrete);

  sequencer_running = false;
  sequencer_should_run = false;
  sequencer_open = true;
  PushEvent(SEQ_EVENT_OPENED);
  return true;
}

bool SequencerStart() {
  log_printf("SequencerStart()");
  if (!sequencer_open) return false;
  if (sequencer_running) return false;

  assert(PR2 == 1);

  sequencer_running = true;
  sequencer_should_run = true;

  // It is important that we clear the IF. We want the IRQ to happen in sync
  // with the timer clock.
  PRIORITY(INT_PRIORITY) {
    _T2IF = 0;
    _T2IE = 1;
  }
  return true;
}

bool SequencerPause() {
  log_printf("SequencerPause()");
  if (!sequencer_open) return false;
  // Will be picked up upon next interrupt.
  sequencer_should_run = false;
  return true;
}

bool SequencerStop() {
  log_printf("SequencerStop()");
  if (!sequencer_open) return false;

  _T2IE = 0;
  if (sequencer_running) {
    Freeze(&the_config.idle_cue,
           the_config.oc_enabled,
           the_config.oc_discrete,
           the_config.oc_idle_change);
    PR2 = 1;
    sequencer_running = false;
  }
  sequencer_should_run = false;
  // Remember how many elements we're removing. We need to report this.
  uint8_t queue_size = SequenceSize(&the_sequence);
  // We only have 5 bits in the event queue for size.
  assert(queue_size < 0x1F);
  SequenceClear(&the_sequence);
  PushEvent(SEQ_EVENT_STOPPED | (queue_size << 3));
  return true;
}

static int oc_waiting_close = 0;

bool SequencerClose() {
  log_printf("SequencerClose()");
  if (!sequencer_open) return false;

  SequencerStop();
  // At this point, we're stopped, and only non-discrete OC channels are still
  // possibly running.
  // We don't want to stop them immediately, as this may cause the last pulse to
  // have an undesired width.
  // So first, count how many such channels there are.
  // Then for each one, set the pulse width to 0 and the period to 1. Clear the
  // interrupt flag, and enable interrupt.
  // On each OC interrupt, close the OC and decrement the counter. If it's the
  // last one, report closed.
  int i;
  for (i = 0; i < NUM_PWM_MODULES; ++i) {
    if (the_config.oc_enabled & ~the_config.oc_discrete & (1 << i)) {
      ++oc_waiting_close;
    }
  }

  if (oc_waiting_close) {
    #define OC_STOP(num, ununed)                            \
      if (the_config.oc_enabled & (1 << (num - 1))) {       \
        if (!(the_config.oc_discrete & (1 << (num - 1)))) { \
          OC##num##R   = 0;                                 \
          OC##num##RS  = 1;                                 \
          _OC##num##IP = 1;                                 \
          _OC##num##IF = 0;                                 \
          _OC##num##IE = 1;                                 \
        }                                                   \
      }

    REPEAT_1B(OC_STOP, NUM_PWM_MODULES, 0)
    #undef OC_STOP
  } else {
    PushEvent(SEQ_EVENT_CLOSED);
  }

  sequencer_open = false;
  return true;
}

#define OC_INTERRUPT(num, unused)                                         \
  void __attribute__((__interrupt__,no_auto_psv)) _OC##num##Interrupt() { \
    OC##num##CON1 = 0;                                                    \
    if (--oc_waiting_close == 0) PushEvent(SEQ_EVENT_CLOSED);             \
    _OC##num##IF = 0;                                                     \
    _OC##num##IE = 0;                                                     \
  }

REPEAT_1B(OC_INTERRUPT, NUM_PWM_MODULES, 0)
#undef OC_INTERRUPT

void SequencerKill() {
  log_printf("SequencerKill()");
  PRIORITY(INT_PRIORITY) {
    _T2IE = 0;
    _T2IF = 0;
    sequencer_running = false;
    sequencer_should_run = false;

    #define OC_KILL(num, ununed)                      \
      if (the_config.oc_enabled & (1 << (num - 1))) { \
        OC##num##CON1 = 0;                            \
      }

    REPEAT_1B(OC_KILL, NUM_PWM_MODULES, 0)
    #undef OC_KILL

    SequenceClear(&the_sequence);
    ClearConfig(&the_config);
    num_active_channels = 0;
    QUEUE_CLEAR(&event_queue);

    sequencer_open = false;
  }
}

bool SequencerManualCue(uint8_t const *cue) {
  log_printf("SequencerManualCue(0x%p)", cue);
  log_print_buf(cue, SequencerExpectedCueSize());
  assert(cue);

  if (!sequencer_open) return false;
  if (sequencer_running) return false;

  Cue raw_cue;
  if (!TranslateCue(cue, &raw_cue)) return false;
  // PrintCue(&raw_cue);
  ProcessCue(&raw_cue, the_config.oc_enabled, the_config.oc_discrete);
  return true;
}

bool SequencerManualStop() {
  log_printf("SequencerManualStop()");
  if (!sequencer_open) return false;
  if (sequencer_running) return false;

  Freeze(&the_config.idle_cue,
         the_config.oc_enabled,
         the_config.oc_discrete,
         the_config.oc_idle_change);
  return true;
}

uint8_t SequencerGetEvent() {
  if (!QUEUE_SIZE(&event_queue)) return SEQ_EVENT_NONE;
  uint8_t result = *QUEUE_PEEK(&event_queue);
  QUEUE_PULL(&event_queue);
  return result;
}

void SequencerInit() {
  log_printf("SequencerInit()");

  // Just in case we were open. Reset everything to a known state.
  SequencerKill();

  _T2IP = INT_PRIORITY;
  PR2 = 1;
}

size_t SequencerQueueLength() {
  return SEQUENCE_LENGTH;
}

size_t SequencerExpectedCueSize() {
  return expected_cue_size;
}

