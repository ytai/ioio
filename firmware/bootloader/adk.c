#include "blapi/adk.h"
#include "usb_host_android.h"


BYTE ADKRead(void *buffer, DWORD length) {
  return USBHostAndroidRead(buffer, length, ANDROID_INTERFACE_ADK);
}

BOOL ADKReadDone(BYTE *errorCode, DWORD *byteCount) {
  return USBHostAndroidRxIsComplete(errorCode, byteCount, ANDROID_INTERFACE_ADK);
}

BYTE ADKWrite(const void *buffer, DWORD length) {
  return USBHostAndroidWrite(buffer, length, ANDROID_INTERFACE_ADK);
}

BOOL ADKWriteDone(BYTE *errorCode) {
  return USBHostAndroidTxIsComplete(errorCode, ANDROID_INTERFACE_ADK);
}

BOOL ADKAttached() {
  return USBHostAndroidIsInterfaceAttached(ANDROID_INTERFACE_ADK);
}
