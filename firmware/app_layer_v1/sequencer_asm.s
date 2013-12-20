;
; Copyright 2013 Ytai Ben-Tsvi. All rights reserved.
;
;
; Redistribution and use in source and binary forms, with or without modification, are
; permitted provided that the following conditions are met:
;
;    1. Redistributions of source code must retain the above copyright notice, this list of
;       conditions and the following disclaimer.
;
;    2. Redistributions in binary form must reproduce the above copyright notice, this list
;       of conditions and the following disclaimer in the documentation and/or other materials
;       provided with the distribution.
;
; THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
; WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
; FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
; CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
; SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
; ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
; NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
; ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
; The views and conclusions contained in the software and documentation are those of the
; authors and should not be interpreted as representing official policies, either expressed
; or implied.
;

  .set NUM_PWM_MODULES, 9
  .set NUM_PORTS, 6
  .set sizeof_OCCue, 8
  .set sizeof_OCSFR, 10
  .set sizeof_PortCue, 4
  .set sizeof_Cue, NUM_PWM_MODULES * sizeof_OCCue + NUM_PORTS * sizeof_PortCue
  .set sizeof_TimedCue, sizeof_Cue + 2
  .set SEQUENCE_LENGTH, 32
  .set sizeof_Sequence_buf, SEQUENCE_LENGTH * sizeof_TimedCue
  .set offsetof_Sequence_read_count, 0
  .set offsetof_Sequence_write_count, 2
  .set offsetof_Sequence_buf, 4
  .set offsetof_ChannelConfig_oc_enabled, 0
  .set offsetof_ChannelConfig_oc_discrete, 2
  .set offsetof_ChannelConfig_oc_idle_change, 4
  .set offsetof_ChannelConfig_idle_cue, 6
  .set offsetof_TimedCue_time, sizeof_Cue

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MACRO sequence_peek: Equivalent to SequencePeek
;   inout: register. IN: pointer to sequence. OUT: pointer to a TimedCue, or
;          NULL if empty.
;   tmp1: register (even!). IN: don't care. OUT: undefined.
;   tmp2: register (tmp1+1). IN: don't care. OUT: undefined.
;   Z flag: set iff NULL.
;
; CYCLES: 12
  .macro sequence_peek inout, tmp1, tmp2
    mov.w   [\inout + offsetof_Sequence_read_count], \tmp1
    mov.w   [\inout + offsetof_Sequence_write_count], \tmp2
    cp      \tmp1, \tmp2
    bra     eq, empty\@
    and.w   #(SEQUENCE_LENGTH - 1), \tmp1
    mov.w   #(sizeof_TimedCue), \tmp2
    mul.uu  \tmp1, \tmp2, \tmp1
    add.w   #offsetof_Sequence_buf, \inout
    add.w   \inout, \tmp1, \inout
    bra     end\@
  empty\@:
    and.w   #0, \inout
    repeat  #4
    nop
  end\@:
  .endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MACRO sequence_pull: Equivalent to SequencePull
;   seq: register. IN: pointer to sequence. OUT: unchanged.
  .macro sequence_pull seq
    inc.w [\seq], [\seq]
  .endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MACRO oc_pre: Setting of OC registers (pre)
;   pcue: register. IN: pointer to an OCCue array. OUT: unchanged.
;   oc_en: register. IN: the oc_enable mask. OUT: unchanged.
;   oc_dis: register IN: the oc_discrete mask. OUT: unchanged.
;   num: Expression, index in the OCCue array.
;   tmp: register. IN: don't care. OUT: undefined.
;
; CYCLES: enabled ? 8 : 3
  .macro oc_pre pcue, oc_en, oc_dis, num, tmp
    .set      con1\@, _OC1CON1 + \num * sizeof_OCSFR + 0
    .set      rs\@,   con1\@ + 4
    .set      r\@,    con1\@ + 6
    .set      ocrs\@, \num * sizeof_OCCue  + 0
    .set      ocr\@,  ocrs\@ + 2
    btst.z    \oc_en, #\num
    bra       Z, end\@
    btsc      \oc_dis, #\num
    clr.w     con1\@
    mov.w     [\pcue + ocr\@], \tmp
    mov.w     \tmp, r\@
    mov.w     [\pcue + ocrs\@], \tmp
    mov.w     \tmp, rs\@
  end\@:
  .endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MACRO port: Seting of a LAT register.
;   pport: register. IN: pointer to a PortCue struct. OUT: pointer to the next.
;   lat:  symbol. LAT register.
;   zero: register. IN: must be zero. OUT: unchanged.
;   w0: register. IN: don't care. OUT: undefined.
;
; CYCLES: 4

  .macro port pport, lat, zero
    ior.w     \zero, [\pport++], w0
    and.w     \lat
    ior.w     \zero, [\pport++], w0
    ior.w     \lat
  .endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MACRO post: Setting of OC registers (pre)
;   pcue: register. IN: pointer to an OCCue array. OUT: unchanged.
;   oc_en: register. IN: the oc_enable mask. OUT: unchanged.
;   oc_dis: register IN: the oc_discrete mask. OUT: unchanged.
;   num: Expression, index in the OCCue array.
;   zero: register. IN: must be zero. OUT: unchanged.
;   w0: register. IN: don't care. OUT: undefined.
;
; CYCLES: enabled ? 8 : 3
  .macro oc_post pcue, oc_en, oc_dis, num, zero
    .set      con1\@, _OC1CON1 + \num * sizeof_OCSFR + 0
    .set      rs\@,   con1\@ + 4
    .set      r\@,    con1\@ + 6
    .set      occon1\@, \num * sizeof_OCCue  + 4
    .set      ocinc\@,  occon1\@ + 2

    btst.z    \oc_en, #\num
    bra       Z, end\@
    mov.w     [\pcue+occon1\@], w0
    btsc      \oc_dis, #\num
    mov.w     w0, con1\@
    mov.w     [\pcue+ocinc\@], w0
    add.w     rs\@
    add.w     r\@
  end\@:
  .endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MACRO oc_freeze_dis: Freeze discrete channels
;   pcue: register. IN: pointer to an OCCue array. OUT: unchanged.
;   oc_en: register. IN: the oc_enable mask. OUT: unchanged.
;   oc_dis: register IN: the oc_discrete mask. OUT: unchanged.
;   num: Expression, index in the OCCue array.
;
; CYCLES: enabled ? 8 : 3
  .macro oc_freeze_dis pcue, oc_en, oc_dis, num
    .set      con1\@, _OC1CON1 + \num * sizeof_OCSFR + 0
    btst.z    \oc_en, #\num
    bra       Z, end\@
    btsc      \oc_dis, #\num
    clr.w     con1\@
    repeat    #2
    nop
  end\@:
  .endm


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MACRO oc_freeze_con: Freeze continuous channels.
;   pcue: register. IN: pointer to an OCCue array. OUT: unchanged.
;   oc_en: register. IN: the oc_enable mask. OUT: unchanged.
;   oc_dis: register IN: the oc_discrete mask. OUT: unchanged.
;   oc_idle: register IN: the oc_idle_change mask. OUT: unchanged.
;   num: Expression, index in the OCCue array.
;   tmp: register. IN: don't care. OUT: undefined.
;
; CYCLES: enabled ? 8 : 3
  .macro oc_freeze_con pcue, oc_en, oc_dis, oc_idle, num, tmp
    .set      con1\@, _OC1CON1 + \num * sizeof_OCSFR + 0
    .set      rs\@,   con1\@ + 4
    .set      r\@,    con1\@ + 6
    .set      ocrs\@, \num * sizeof_OCCue  + 0
    .set      ocr\@,  ocrs\@ + 2
    btst.z    \oc_en, #\num
    bra       Z, end\@
    btst.z    \oc_idle, #\num
    bra       Z, end\@
    mov.w     [\pcue + ocr\@], \tmp
    mov.w     \tmp, r\@
    mov.w     [\pcue + ocrs\@], \tmp
    mov.w     \tmp, rs\@
  end\@:
  .endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MACRO oc_start: Start OC channels.
;   pcue: register. IN: pointer to an OCCue array. OUT: unchanged.
;   oc_en: register. IN: the oc_enable mask. OUT: unchanged.
;   oc_dis: register IN: the oc_discrete mask. OUT: unchanged.
;   num: Expression, index in the OCCue array.
;   tmp: register. IN: don't care. OUT: undefined.
;
; CYCLES: enabled ? (discrete ? 7 : 12) : 3
  .macro oc_start pcue, oc_en, oc_dis, num, tmp
    .set      con1\@,   _OC1CON1 + \num * sizeof_OCSFR + 0
    .set      con2\@,   con1\@ + 2
    .set      rs\@,     con1\@ + 4
    .set      r\@,      con1\@ + 6
    .set      ocrs\@,   \num * sizeof_OCCue  + 0
    .set      ocr\@,    ocrs\@ + 2
    .set      occon1\@, ocrs\@ + 4
    btst.z    \oc_en, #\num
    bra       Z, end\@
    mov.w     #0x1F, \tmp
    mov.w     \tmp, con2\@
    btst.z    \oc_dis, #\num
    bra       NZ, end\@
    mov.w     [\pcue + ocrs\@], \tmp
    mov.w     \tmp, rs\@
    mov.w     [\pcue + ocr\@], \tmp
    mov.w     \tmp, r\@
    mov.w     [\pcue + occon1\@], \tmp
    mov.w     \tmp, con1\@
  end\@:
  .endm


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; FUNCTION _ProcessCue
;
; Timing info:
; Delay until first pre: 4
; Max delay between pre and post: 9*8 + 4*6 = 96
; Max total cycles: 4+9*(8+8)+6*4+3 = 175
  .global _ProcessCue
_ProcessCue:
  ; w0 = pcue
  ; w1 = oc_en
  ; w2 = oc_dis
  ; w3 = pcue
  mov w0, w3
  ; w4 = pport
  mov w0, w4
  add #(9 * 8), w4
  ; w5 = tmp
  ; w6 = zero
  clr.w w6

  ; OC (pre)
  .irp num, 0, 1, 2, 3, 4, 5, 6, 7, 8
    oc_pre w3, w1, w2, \num, w5
  .endr

  ; PORT
  .irp p, B, C, D, E, F, G
    port w4, _LAT\p, w6
  .endr

  ; OC (post)
  .irp num, 0, 1, 2, 3, 4, 5, 6, 7, 8
    oc_post w3, w1, w2, \num, w6
  .endr

  return
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; FUNCTION _Freeze
;
; Timing info:
; Delay until first OC: 4
; Delay between discrete OC: enabled ? 8 : 3
; Max total cycles: 4+9*(8+8)+6*4+3 = 175
  .global _Freeze
_Freeze:
  ; w0 = pcue
  ; w1 = oc_en
  ; w2 = oc_dis
  ; w3 = oc_idle
  ; w4 = pcue
  mov w0, w4
  ; w5 = pport
  mov w0, w5
  add #(9 * 8), w5
  ; w6 = tmp
  ; w7 = zero
  clr.w w7

  ; OC (discrete)
  .irp num, 0, 1, 2, 3, 4, 5, 6, 7, 8
    oc_freeze_dis w4, w1, w2, \num
  .endr

  ; OC (continuous)
  .irp num, 0, 1, 2, 3, 4, 5, 6, 7, 8
    oc_freeze_con w4, w1, w2, w3, \num, w6
  .endr

  ; PORT
  .irp p, B, C, D, E, F, G
    port w5, _LAT\p, w7
  .endr

  return

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; FUNCTION _ProcessStart
;
; Timing info:
; Max total cycles: 4+9*12+6*4+3 = 139
  .global _ProcessStart
_ProcessStart:
  ; w0 = pcue
  ; w1 = oc_en
  ; w2 = oc_dis
  ; w3 = pcue
  mov w0, w3
  ; w4 = pport
  mov w0, w4
  add #(9 * 8), w4
  ; w5 = tmp
  ; w6 = zero
  clr.w w6

  ; OC
  .irp num, 0, 1, 2, 3, 4, 5, 6, 7, 8
    oc_start w3, w1, w2, \num, w5
  .endr

  ; PORT
  .irp p, B, C, D, E, F, G
    port w4, _LAT\p, w6
  .endr

  return

  ; T2 interrupt handler.
  ;
  ; Cycles:
  ; From timer match to first instruction (with bootloader):
  ;     8
  ; From first instruction to first instruction of either Freeze or ProcessCue:
  ;    30
  ; From first instruction of functions to first setting of OC SFR:
  ;     X
  ; From first instruction of functions to one-after-last setting of OC SFR:
  ;   172
  .global __T2Interrupt
__T2Interrupt:
  ; Backup registers.
  ; CYCLES: 8
  push.s
  push    _RCOUNT
  push.d  w4
  push.d  w6
  push.d  w8

  ; w0 = sequencer_should_run ? SequencePeek(seq) : NULL
  ; w1 = oc_en
  ; w2 = oc_dis
  ; CYCLES: 17
  mov.w   #_the_sequence, w0
  sequence_peek w0, w2, w3
  btss    _sequencer_should_run, #0
  ; Clear w0 while setting the Z flag.
  and.w   #0, w0
  mov.w   (_the_config + offsetof_ChannelConfig_oc_enabled), w1
  mov.w   (_the_config + offsetof_ChannelConfig_oc_discrete), w2

  ; branch if w0 is not NULL.
  ; CYCLES: taken ? 2 : 1
  ; This is balanced by the fact that freeze has one extra cycle before getting
  ; called.
  ; So from this point, cycles until either rcall are 3,
  ; and CYCLES TO START OF EITHER FUNCTION: 5.
  bra     NZ, process

freeze:
  ; w0 = &the_config.idle_cue
  mov.w   #(_the_config + offsetof_ChannelConfig_idle_cue), w0
  ; w3 = the_config.oc_idle
  mov.w   (_the_config + offsetof_ChannelConfig_oc_idle_change), w3
  rcall   _Freeze
  ; No longer counting cycles!

  ; Clear T2IE
  bclr    _IEC0, #7

  ; PR2 = 1
  mov.w   #1, w0
  mov.w   w0, _PR2

  ; sequencer_running = false;
  clr.w   _sequencer_running

  ; If _sequencer_should_run is 1, this is a stall event, otherwise, a pause
  ; event. The event numbers have been chosen so that copying
  ; _sequencer_should_run does the right thing:
  ; PushEvent(SEQ_EVENT_PAUSED or SEQ_EVENT_STALLED);
  mov.w   _sequencer_should_run, w0

  bra     done

process:
  ; Backup w0 to w8
  mov.w   w0, w8
  rcall   _ProcessCue
  ; No longer counting cycles!

  ; SequencePull(the_sequence)
  mov.w   #_the_sequence, w0
  sequence_pull w0

  ; PR2 = w8->time
  mov.w   [w8+offsetof_TimedCue_time], w0
  mov.w   w0, _PR2

  ; PushEvent(SEQ_EVENT_NEXT_CUE)
  mov.w   #3, w0

done:
  rcall   _PushEvent
  ; Clear T2IF.
  bclr    _IFS0, #7

  ; Restore registers.
  pop.d   w8
  pop.d   w6
  pop.d   w4
  pop     _RCOUNT
  pop.s
  retfie


  .end
