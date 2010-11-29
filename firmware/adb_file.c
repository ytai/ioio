#include <assert.h>
#include <string.h>
#include "logging.h"
#include "adb_file.h"

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

typedef enum {
  ADB_FILE_STATE_FREE = 0,
  ADB_FILE_STATE_WAIT_OPEN,
  ADB_FILE_STATE_WAIT_HEADER,
  ADB_FILE_STATE_WAIT_DATA
} ADB_FILE_STATE;

typedef struct {
  ADB_FILE_STATE state;
  ADBChannelRecvFunc func;
  ADB_CHANNEL_HANDLE handle;
  UINT32 read_remain;
  adb_syncmsg msg;
  char path[ADB_FILE_MAX_PATH_LENGTH];  // must follow msg immediately - sent together.
} ADB_FILE;

static ADB_FILE adb_files[ADB_FILE_MAX_FILES];

static void ADBFileRecvFunc(ADB_CHANNEL_HANDLE h, const void* data, UINT32 data_len) {
}

ADB_FILE_HANDLE ADBFileRead(const char* path, ADBChannelRecvFunc recv_func) {
  int i;
  assert(path != NULL);
  assert(strlen(path) < ADB_FILE_MAX_PATH_LENGTH);
  for (i = 0; i < ADB_FILE_MAX_FILES && adb_files[i].state == ADB_FILE_STATE_FREE; ++i);
  if (i == ADB_FILE_MAX_FILES) {
    print1("Exceeded maximum number of open files: %d", ADB_FILE_MAX_FILES);
    return ADB_FILE_INVALID_HANDLE;
  }
  if ((adb_files[i].handle = ADBOpen("sync:", &ADBFileRecvFunc)) == ADB_INVALID_CHANNEL_HANDLE) {
    print0("Failed to open ADB channel to sync:");
    return ADB_FILE_INVALID_HANDLE;
  }
  adb_files[i].func = recv_func;
  strncpy(adb_files[i].path, path, ADB_FILE_MAX_PATH_LENGTH);
  ADB_CHANGE_STATE(adb_files[i].state, ADB_FILE_STATE_WAIT_OPEN);
  return i;
}

void ADBFileInit() {
  memset(adb_files, 0, sizeof adb_files);
}

void ADBFileTasks() {
  static int current_file = 0;
  int i;
  for (i = 0; i < ADB_FILE_MAX_FILES; ++i) {
    ADB_FILE* f;
    if (++current_file == ADB_FILE_MAX_FILES) current_file = 0;
    f = &adb_files[current_file];
    switch (f->state) {
     case ADB_FILE_STATE_FREE:
      continue;

     case ADB_FILE_STATE_WAIT_OPEN:
      if (ADBChannelReady(f->handle)) {
        f->msg.req.id = ID_RECV;
        f->msg.req.namelen = strlen(f->path);
        ADBWrite(f->handle, &f->msg.req, sizeof f->msg.req + f->msg.req.namelen);
        f->read_remain = sizeof f->msg.data;
        ADB_CHANGE_STATE(adb_files[current_file].state, ADB_FILE_STATE_WAIT_HEADER);
      }
      break;

     case ADB_FILE_STATE_WAIT_HEADER:
      break;

     case ADB_FILE_STATE_WAIT_DATA:
      break;
    }
    break;
  }
}
