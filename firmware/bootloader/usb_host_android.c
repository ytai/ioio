#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "GenericTypeDefs.h"
#include "USB/usb.h"
#include "usb_host_android.h"

#include "logging.h"

ANDROID_DEVICE gc_DevData;

BOOL USBHostAndroidInit(BYTE address, DWORD flags, BYTE clientDriverID) {
  USB_DEVICE_DESCRIPTOR *pDev;
  USB_DEVICE_INFO *pDevInfo = USBHostGetDeviceInfo();
  USB_ENDPOINT_INFO *pFirstEpInfo;
  USB_ENDPOINT_INFO *pSecondEpInfo;

  // Initialize state
  gc_DevData.rxLength = 0;
  gc_DevData.flags.val = 0;
  
  // Save device the address, VID, & PID
  gc_DevData.ID.deviceAddress = address;
  pDev = (USB_DEVICE_DESCRIPTOR *) USBHostGetDeviceDescriptor(address);
  gc_DevData.ID.vid  =  pDev->idVendor;
  gc_DevData.ID.pid  =  pDev->idProduct;
  
  // Save the Client Driver ID
  gc_DevData.clientDriverID = clientDriverID;

  // Save the endpoint addresses
  pFirstEpInfo = pDevInfo->pInterfaceList->pCurrentSetting->pEndpointList;
  if (!pFirstEpInfo) return FALSE;
  pSecondEpInfo = pFirstEpInfo->next;
  if (!pSecondEpInfo) return FALSE;
  if (pSecondEpInfo->next) return FALSE;  // we expect exactly 2 endpoints.

  if (pFirstEpInfo->bEndpointAddress & 0x80) {
      if (pSecondEpInfo->bEndpointAddress & 0x80) return FALSE;
      gc_DevData.inEndpoint = pFirstEpInfo->bEndpointAddress;
      gc_DevData.outEndpoint = pSecondEpInfo->bEndpointAddress;
  } else {
      if (!(pSecondEpInfo->bEndpointAddress & 0x80)) return FALSE;
      gc_DevData.inEndpoint = pSecondEpInfo->bEndpointAddress;
      gc_DevData.outEndpoint = pFirstEpInfo->bEndpointAddress;
  }
  
  log_printf("Android Client Initalized: flags=0x%lx address=%d VID=0x%x PID=0x%x IN_EP=0x%x OUT_EP=0x%x",
              flags, address, gc_DevData.ID.vid, gc_DevData.ID.pid, gc_DevData.inEndpoint, gc_DevData.outEndpoint);
  
  // Android Driver Init Complete.
  gc_DevData.flags.initialized = 1;
  
  return TRUE;
} //  USBHostAndroidInit
  

BOOL USBHostAndroidEventHandler(BYTE address, USB_EVENT event, void *data, DWORD size) {
  // Make sure it was for our device
  if (address != gc_DevData.ID.deviceAddress) return FALSE;

  // Handle specific events.
  switch (event) {
   case EVENT_DETACH:
    gc_DevData.flags.val        = 0;
    gc_DevData.ID.deviceAddress = 0;

    log_printf("Android Client Device Detached: address=0x%x", address);
    break;

#ifdef USB_ENABLE_TRANSFER_EVENT
   case EVENT_BUS_ERROR:
   case EVENT_TRANSFER:
    if ((data != NULL) && (size == sizeof(HOST_TRANSFER_DATA))) {
      DWORD dataCount = ((HOST_TRANSFER_DATA *)data)->dataCount;
  
      if (((HOST_TRANSFER_DATA *)data)->bEndpointAddress == gc_DevData.inEndpoint) {
        gc_DevData.flags.rxBusy = 0;
        gc_DevData.rxLength = dataCount;
        gc_DevData.rxErrorCode = ((HOST_TRANSFER_DATA *)data)->bErrorCode;
        log_printf("Received message with %ld bytes: ", ((HOST_TRANSFER_DATA *)data)->dataCount);
        log_print_buf(((HOST_TRANSFER_DATA *)data)->pUserData, ((HOST_TRANSFER_DATA *)data)->dataCount);
      } else if (((HOST_TRANSFER_DATA *)data)->bEndpointAddress == gc_DevData.outEndpoint) {
        gc_DevData.flags.txBusy = 0;
        gc_DevData.txErrorCode = ((HOST_TRANSFER_DATA *)data)->bErrorCode;
      } else {
        return FALSE;
      }
      return TRUE;
    } else {
      return FALSE;
    }
#endif

   case EVENT_SUSPEND:
   case EVENT_RESUME:
   default:
    return TRUE;
  }
  return FALSE;
}  // USBHostAndroidEventHandler


void USBHostAndroidGetDeviceId(ANDROID_DEVICE_ID *pDevID) {
  assert(gc_DevData.flags.initialized);
  assert(pDevID != NULL);
  *pDevID = gc_DevData.ID;
}  // USBHostAndroidGetDeviceAddress


void USBHostAndroidReset() {
  assert(USBHostAndroidIsDeviceAttached());
  USBHostResetDevice(gc_DevData.ID.deviceAddress);
  gc_DevData.flags.val        = 0;
  gc_DevData.ID.deviceAddress = 0;
}

BYTE USBHostAndroidRead(void *buffer, DWORD length) {
  BYTE RetVal;

  // Validate the call
  assert(USBHostAndroidIsDeviceAttached());
  if (gc_DevData.flags.rxBusy) return USB_BUSY;

  log_printf("Requested read of %u bytes", (unsigned) length);

  // Set the busy flag, clear the count and start a new IN transfer.
  gc_DevData.flags.rxBusy = 1;
  gc_DevData.rxLength = 0;
  RetVal = USBHostRead(gc_DevData.ID.deviceAddress, gc_DevData.inEndpoint, (BYTE *)buffer, length);
  if (RetVal != USB_SUCCESS) {
    gc_DevData.flags.rxBusy = 0;    // Clear flag to allow re-try
  }
  return RetVal;
}  // USBHostAndroidRead

BOOL USBHostAndroidRxIsComplete(BYTE *errorCode, DWORD *byteCount) {
  if (gc_DevData.flags.rxBusy) {
    return FALSE;
  } else {
    *byteCount = gc_DevData.rxLength;
    *errorCode = gc_DevData.rxErrorCode;
    return TRUE;
  }
}  // USBHostAndroidRxIsComplete

#ifndef USB_ENABLE_TRANSFER_EVENT
void USBHostAndroidTasks(void) {
  DWORD   byteCount;
  BYTE    errorCode;

  if (gc_DevData.ID.deviceAddress && gc_DevData.flags.initialized) {
    if (gc_DevData.flags.rxBusy) {
      if (USBHostTransferIsComplete(gc_DevData.ID.deviceAddress, gc_DevData.inEndpoint, &errorCode, &byteCount)) {
          gc_DevData.flags.rxBusy = 0;
          gc_DevData.rxLength     = byteCount;
          gc_DevData.rxErrorCode  = errorCode;
          log_print_1("Received message with %ld bytes", byteCount);
      }
    }

    if (gc_DevData.flags.txBusy) {
      if (USBHostTransferIsComplete(gc_DevData.ID.deviceAddress, gc_DevData.outEndpoint, &errorCode, &byteCount)) {
        gc_DevData.flags.txBusy = 0;
        gc_DevData.txErrorCode  = errorCode;
      }
    }
  }
}  // USBHostAndroidTasks
#endif  // USB_ENABLE_TRANSFER_EVENT

BOOL USBHostAndroidTxIsComplete(BYTE *errorCode) {
  if (gc_DevData.flags.txBusy) {
    return FALSE;
  } else {
    *errorCode = gc_DevData.txErrorCode;
    return TRUE;
  }
}  // USBHostAndroidTxIsComplete


BYTE USBHostAndroidWrite(const void *buffer, DWORD length) {
  BYTE RetVal;

  // Validate the call
  assert(USBHostAndroidIsDeviceAttached());
  if (gc_DevData.flags.txBusy) return USB_BUSY;

  log_printf("Sending message with %u bytes: ", (unsigned) length);
  log_print_buf(buffer, length);

  // Set the busy flag and start a new OUT transfer.
  gc_DevData.flags.txBusy = 1;
  RetVal = USBHostWrite(gc_DevData.ID.deviceAddress, gc_DevData.outEndpoint, (BYTE *)buffer, length);
  if (RetVal != USB_SUCCESS) {
    gc_DevData.flags.txBusy = 0;    // Clear flag to allow re-try
  }
  return RetVal;
}  // USBHostAndroidWrite
