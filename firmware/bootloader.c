#include "adb.h"
#include "adb_file.h"
#include "bootloader.h"

BOOL BootloaderTasks() {
  BOOL connected = ADBTasks();
  if (!connected) return FALSE;
  ADBFileTasks();
  return TRUE;
}
