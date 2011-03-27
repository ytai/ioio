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
