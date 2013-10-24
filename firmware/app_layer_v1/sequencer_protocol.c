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

#include "sequencer_protocol.h"
#include "protocol.h"

void SequencerTasks() {
  // Flush the event queue.
  uint8_t e;
  while ((e = SequencerGetEvent()) != SEQ_EVENT_NONE) {
    SequencerEvent const ev = e & 0x07;
    OUTGOING_MESSAGE msg;
    msg.type = SEQUENCER_EVENT;
    msg.args.sequencer_event.event = ev;
    if (ev == SEQ_EVENT_OPENED) {
      uint8_t size = (uint8_t) SequencerQueueLength();
      AppProtocolSendMessageWithVarArg(&msg, &size, 1);
    } else if (ev == SEQ_EVENT_STOPPED) {
      uint8_t size = e >> 3;
      AppProtocolSendMessageWithVarArg(&msg, &size, 1);
    } else {
      AppProtocolSendMessage(&msg);
    }
  }
}

bool SequencerCommand(SEQ_CMD cmd, uint8_t const * extra) {
  switch (cmd) {
    case SEQ_CMD_STOP:         return SequencerStop();
    case SEQ_CMD_START:        return SequencerStart();
    case SEQ_CMD_PAUSE:        return SequencerPause();
    case SEQ_CMD_MANUAL_START: return SequencerManualCue(extra);
    case SEQ_CMD_MANUAL_STOP:  return SequencerManualStop();
    default:                   return false;
  }
}
