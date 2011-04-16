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

#ifndef __REGMACROS_H__
#define __REGMACROS_H__


// repetition macros - 1-based
#define _REPEAT_1B_1(macro, ...) macro(1, __VA_ARGS__)
#define _REPEAT_1B_2(macro, ...) _REPEAT_1B_1(macro, __VA_ARGS__) macro(2, __VA_ARGS__)
#define _REPEAT_1B_3(macro, ...) _REPEAT_1B_2(macro, __VA_ARGS__) macro(3, __VA_ARGS__)
#define _REPEAT_1B_4(macro, ...) _REPEAT_1B_3(macro, __VA_ARGS__) macro(4, __VA_ARGS__)
#define _REPEAT_1B_5(macro, ...) _REPEAT_1B_4(macro, __VA_ARGS__) macro(5, __VA_ARGS__)
#define _REPEAT_1B_6(macro, ...) _REPEAT_1B_5(macro, __VA_ARGS__) macro(6, __VA_ARGS__)
#define _REPEAT_1B_7(macro, ...) _REPEAT_1B_6(macro, __VA_ARGS__) macro(7, __VA_ARGS__)
#define _REPEAT_1B_8(macro, ...) _REPEAT_1B_7(macro, __VA_ARGS__) macro(8, __VA_ARGS__)
#define _REPEAT_1B_9(macro, ...) _REPEAT_1B_8(macro, __VA_ARGS__) macro(9, __VA_ARGS__)
#define _REPEAT_1B_10(macro, ...) _REPEAT_1B_9(macro, __VA_ARGS__) macro(10, __VA_ARGS__)
#define _REPEAT_1B_11(macro, ...) _REPEAT_1B_10(macro, __VA_ARGS__) macro(11, __VA_ARGS__)
#define _REPEAT_1B_12(macro, ...) _REPEAT_1B_11(macro, __VA_ARGS__) macro(12, __VA_ARGS__)
#define _REPEAT_1B_13(macro, ...) _REPEAT_1B_12(macro, __VA_ARGS__) macro(13, __VA_ARGS__)
#define _REPEAT_1B_14(macro, ...) _REPEAT_1B_13(macro, __VA_ARGS__) macro(14, __VA_ARGS__)
#define _REPEAT_1B_15(macro, ...) _REPEAT_1B_14(macro, __VA_ARGS__) macro(15, __VA_ARGS__)
#define _REPEAT_1B_16(macro, ...) _REPEAT_1B_15(macro, __VA_ARGS__) macro(16, __VA_ARGS__)

#define _REPEAT_1B(macro, times, ...) _REPEAT_1B_##times(macro, __VA_ARGS__)
#define REPEAT_1B(macro, times, ...) _REPEAT_1B(macro, times, __VA_ARGS__)

// repetition macros - 0-based
#define _REPEAT_0B_1(macro, ...) macro(0, __VA_ARGS__)
#define _REPEAT_0B_2(macro, ...) _REPEAT_1B_1(macro, __VA_ARGS__) macro(1, __VA_ARGS__)
#define _REPEAT_0B_3(macro, ...) _REPEAT_1B_2(macro, __VA_ARGS__) macro(2, __VA_ARGS__)
#define _REPEAT_0B_4(macro, ...) _REPEAT_1B_3(macro, __VA_ARGS__) macro(3, __VA_ARGS__)
#define _REPEAT_0B_5(macro, ...) _REPEAT_1B_4(macro, __VA_ARGS__) macro(4, __VA_ARGS__)
#define _REPEAT_0B_6(macro, ...) _REPEAT_1B_5(macro, __VA_ARGS__) macro(5, __VA_ARGS__)
#define _REPEAT_0B_7(macro, ...) _REPEAT_1B_6(macro, __VA_ARGS__) macro(6, __VA_ARGS__)
#define _REPEAT_0B_8(macro, ...) _REPEAT_1B_7(macro, __VA_ARGS__) macro(7, __VA_ARGS__)
#define _REPEAT_0B_9(macro, ...) _REPEAT_1B_8(macro, __VA_ARGS__) macro(8, __VA_ARGS__)
#define _REPEAT_0B_10(macro, ...) _REPEAT_1B_9(macro, __VA_ARGS__) macro(9, __VA_ARGS__)
#define _REPEAT_0B_11(macro, ...) _REPEAT_1B_10(macro, __VA_ARGS__) macro(10, __VA_ARGS__)
#define _REPEAT_0B_12(macro, ...) _REPEAT_1B_11(macro, __VA_ARGS__) macro(11, __VA_ARGS__)
#define _REPEAT_0B_13(macro, ...) _REPEAT_1B_12(macro, __VA_ARGS__) macro(12, __VA_ARGS__)
#define _REPEAT_0B_14(macro, ...) _REPEAT_1B_13(macro, __VA_ARGS__) macro(13, __VA_ARGS__)
#define _REPEAT_0B_15(macro, ...) _REPEAT_1B_14(macro, __VA_ARGS__) macro(14, __VA_ARGS__)
#define _REPEAT_0B_16(macro, ...) _REPEAT_1B_15(macro, __VA_ARGS__) macro(15, __VA_ARGS__)

#define _REPEAT_0B(macro, times, ...) _REPEAT_0B_##times##(macro, __VA_ARGS__)
#define REPEAT_0B(macro, times, ...) _REPEAT_0B(macro, times, __VA_ARGS__)

typedef void (*SetterFunc) (int value);

#define _SETTER_NAME(num, prefix, suffix) _Set##prefix##num##suffix

#define _DEFINE_SETTER(num, prefix, suffix)                   \
  static void _SETTER_NAME(num, prefix, suffix)(int value) {  \
    prefix##num##suffix = value;                              \
  }

#define _SETTER_REF_COMMA(num, prefix, suffix) &_SETTER_NAME(num, prefix, suffix),

#define _DEFINE_ALL_SETTERS_1B(count, prefix, suffix) _REPEAT_1B(_DEFINE_SETTER, count, prefix, suffix)

#define _DEFINE_SETTER_TABLE_1B(count, prefix, suffix)    \
  static SetterFunc Set##prefix##suffix[count] = {        \
    _REPEAT_1B(_SETTER_REF_COMMA, count, prefix, suffix)  \
  };

#define DEFINE_REG_SETTERS_1B(count, prefix, suffix)  \
  _DEFINE_ALL_SETTERS_1B(count, prefix, suffix)       \
  _DEFINE_SETTER_TABLE_1B(count, prefix, suffix)


#endif  // __REGMACROS_H__
