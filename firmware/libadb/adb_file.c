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

#include <assert.h>
#include <string.h>
#include "logging.h"
#include "adb_file.h"

#define CHANGE_STATE(var,state)              \
do {                                         \
  log_printf("%s set to %s", #var, #state);  \
  var = state;                               \
} while (0)

#define MKID(a,b,c,d) (((UINT32) (a)) | (((UINT32) (b)) << 8) | (((UINT32) (c)) << 16) | (((UINT32) (d)) << 24))

#define ID_STAT MKID('S','T','A','T')
#define ID_LIST MKID('L','I','S','T')
#define ID_ULNK MKID('U','L','N','K')
#define ID_SEND MKID('S','E','N','D')
#define ID_RECV MKID('R','E','C','V')
#define ID_DENT MKID('D','E','N','T')
#define ID_DONE MKID('D','O','N','E')
#define ID_DATA MKID('D','A','T','A')
#define ID_OKAY MKID('O','K','A','Y')
#define ID_FAIL MKID('F','A','I','L')
#define ID_QUIT MKID('Q','U','I','T')

typedef union {
    UINT32 id;
    struct {
        UINT32 id;
        UINT32 namelen;
    } req;
    struct {
        UINT32 id;
        UINT32 mode;
        UINT32 size;
        UINT32 time;
    } stat;
    struct {
        UINT32 id;
        UINT32 mode;
        UINT32 size;
        UINT32 time;
        UINT32 namelen;
    } dent;
    struct {
        UINT32 id;
        UINT32 size;
    } data;
    struct {
        UINT32 id;
        UINT32 msglen;
    } status;    
} adb_syncmsg;

typedef struct {
  UINT32 id;
  UINT32 namelen;
} adb_req;

typedef enum {
  ADB_FILE_STATE_FREE = 0,
  ADB_FILE_STATE_WAIT_OPEN,
  ADB_FILE_STATE_WAIT_HEADER,
  ADB_FILE_STATE_WAIT_DATA,
  ADB_FILE_STATE_WAIT_FAIL_DATA
} ADB_FILE_STATE;

typedef struct {
  ADB_FILE_STATE state;
  ADBFileRecvFunc func;
  ADB_CHANNEL_HANDLE handle;
  UINT32 read_remain;
  adb_syncmsg msg;
  adb_req req;
  char path[ADB_FILE_MAX_PATH_LENGTH];
} ADB_FILE;

static ADB_FILE adb_files[ADB_FILE_MAX_FILES];
static int adb_file_buffer_refcount;

static void ADBFileCallback(ADB_CHANNEL_HANDLE h, const void* data, UINT32 data_len) {
  int i;
  ADB_FILE* f;
  for (i = 0; i < ADB_FILE_MAX_FILES && adb_files[i].handle != h; ++i);
  assert(i < ADB_FILE_MAX_FILES);
  f = &adb_files[i];

  // handle unexpected channel closure
  if (!data) goto error;

  // consume data
  while (1) {
    switch (f->state) {
     case ADB_FILE_STATE_FREE:
     case ADB_FILE_STATE_WAIT_OPEN:
      goto close_and_error;
  
     case ADB_FILE_STATE_WAIT_HEADER:
      if (data_len >= f->read_remain) {
        memcpy(((BYTE*) &f->msg.data) + sizeof f->msg.data - f->read_remain, data, f->read_remain);
        data = ((const BYTE*) data) + f->read_remain;
        data_len -= f->read_remain;
        f->read_remain = f->msg.data.size;
        if (f->msg.data.id == ID_DATA || f->msg.data.id == ID_DONE) {
          CHANGE_STATE(f->state, ADB_FILE_STATE_WAIT_DATA);
        } else if (f->msg.data.id == ID_FAIL) {
          CHANGE_STATE(f->state, ADB_FILE_STATE_WAIT_FAIL_DATA);
        } else {
          goto close_and_error;
        }
      } else {
        memcpy(((BYTE*) &f->msg.data) + sizeof f->msg.data - f->read_remain, data, data_len);
        f->read_remain -= data_len; 
        return;
      }
      break;
  
     case ADB_FILE_STATE_WAIT_DATA:
      if (data_len >= f->read_remain) {
        if (data_len > 0) {
          f->func(i, data, f->read_remain);
        }
        if (f->msg.id == ID_DONE) {
          if (data_len != f->read_remain) goto close_and_error;
          // success - EOF
          f->func(i, NULL, 0);
          ADBClose(f->handle);
          memset(f, 0, sizeof(ADB_FILE));
          return;
        } else {
          data = ((const BYTE*) data) + f->read_remain;
          data_len -= f->read_remain;
          f->read_remain = sizeof f->msg.data;
          CHANGE_STATE(f->state, ADB_FILE_STATE_WAIT_HEADER);
        }
      } else {
        if (data_len > 0) {
          f->func(i, data, data_len);
        }
        f->read_remain -= data_len;
        return;
      }
      break;

     case ADB_FILE_STATE_WAIT_FAIL_DATA:
      if (data_len >= f->read_remain) {
        goto close_and_error;
      } else {
        f->read_remain -= data_len;
        return;
      }
      break;
    }
  }
  return;

close_and_error:
  ADBClose(f->handle);
error:
  log_printf("Failed to open or read file %s", f->path);
  f->func(i, NULL, 1);
  memset(f, 0, sizeof(ADB_FILE));
}

ADB_FILE_HANDLE ADBFileRead(const char* path, ADBChannelRecvFunc recv_func) {
  int i;
  assert(path != NULL);
  assert(strlen(path) < ADB_FILE_MAX_PATH_LENGTH);
  for (i = 0; i < ADB_FILE_MAX_FILES && adb_files[i].state != ADB_FILE_STATE_FREE; ++i);
  if (i == ADB_FILE_MAX_FILES) {
    log_printf("Exceeded maximum number of open files: %d", ADB_FILE_MAX_FILES);
    return ADB_FILE_INVALID_HANDLE;
  }
  if ((adb_files[i].handle = ADBOpen("sync:", &ADBFileCallback)) == ADB_INVALID_CHANNEL_HANDLE) {
    log_printf("Failed to open ADB channel to sync:");
    return ADB_FILE_INVALID_HANDLE;
  }
  adb_files[i].func = recv_func;
  strncpy(adb_files[i].path, path, ADB_FILE_MAX_PATH_LENGTH);
  CHANGE_STATE(adb_files[i].state, ADB_FILE_STATE_WAIT_OPEN);
  log_printf("Success opening file %s. Handle is %d", path, i);
  return i;
}

void ADBFileClose(ADB_FILE_HANDLE h) {
  log_printf("Closing file %d", h);
  assert(h >= 0 && h < ADB_FILE_MAX_FILES);
  ADB_FILE* f = &adb_files[h];
  if (f->state != ADB_FILE_STATE_FREE) {
    ADBClose(f->handle);
    memset(f, 0, sizeof(ADB_FILE));
  }
}

void ADBFileInit() {
  memset(adb_files, 0, sizeof adb_files);
  adb_file_buffer_refcount = 0;
}

void ADBFileTasks() {
  int i;
  for (i = 0; i < ADB_FILE_MAX_FILES; ++i) {
    ADB_FILE* f = &adb_files[i];
    if (f->state == ADB_FILE_STATE_WAIT_OPEN && ADBChannelReady(f->handle)) {
      f->req.id = ID_RECV;
      f->req.namelen = strlen(f->path);
      ADBWrite(f->handle, &f->req, sizeof f->req + f->req.namelen);
      f->read_remain = sizeof f->msg.data;
      CHANGE_STATE(f->state, ADB_FILE_STATE_WAIT_HEADER);
    }
  }
}
