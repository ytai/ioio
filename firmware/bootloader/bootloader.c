#include "adb_private.h"
#include "adb_file_private.h"
#include "bootloader_private.h"

BOOL BootloaderTasks() {
  BOOL connected = ADBTasks();
  if (!connected) return FALSE;
  ADBFileTasks();
  return TRUE;
}

void BootloaderInit() {
  ADBInit();
  ADBFileInit();
}
