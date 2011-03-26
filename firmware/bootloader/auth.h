#ifndef __AUTH_H__
#define __AUTH_H__


typedef enum {
  AUTH_DONE_PASS,
  AUTH_DONE_FAIL,
  AUTH_DONE_PARSE_ERROR,
  AUTH_BUSY
} AUTH_RESULT;

void AuthInit();
AUTH_RESULT AuthProcess(const char* data, int size);


#endif  // __AUTH_H__
