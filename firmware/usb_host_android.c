#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "GenericTypeDefs.h"
#include "USB\usb.h"
#include "usb_host_android.h"
#include "logging.h"

ANDROID_DEVICE gc_DevData;

BOOL USBHostAndroidInit(BYTE address, DWORD flags, BYTE clientDriverID) {
  USB_DEVICE_DESCRIPTOR *pDev;
  
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
  // TODO(ytai): those are currently hard-coded because the USB host doesn't allow easy access
  // to the parsed descriptors for easy retrieval of the endpoints of the current interface.
  gc_DevData.inEndpoint = 0x84;
  gc_DevData.outEndpoint = 0x03;
  
#ifdef DEBUG_MODE
  UART2PrintString("ANDR: Android Client Initalized: flags=0x");
  UART2PutHex(flags);
  UART2PrintString(" address=");
  UART2PutDec(address);
  UART2PrintString(" VID=0x");
  UART2PutHex(gc_DevData.ID.vid >> 8);
  UART2PutHex(gc_DevData.ID.vid & 0xFF);
  UART2PrintString(" PID=0x");
  UART2PutHex(gc_DevData.ID.pid >> 8);
  UART2PutHex(gc_DevData.ID.pid & 0xFF);
  UART2PrintString(" IN_EP=0x");
  UART2PutHex(gc_DevData.inEndpoint);
  UART2PrintString(" OUT_EP=0x");
  UART2PutHex(gc_DevData.outEndpoint);
  UART2PrintString("\r\n");
#endif
  
  // Android Driver Init Complete.
  gc_DevData.flags.initialized = 1;
  
  return TRUE;
} //  USBHostAndroidInit
  

BOOL USBHostAndroidEventHandler(BYTE address, USB_EVENT event, void *data, DWORD size) {
  // Make sure it was for our device
  if (address != gc_DevData.ID.deviceAddress) {
    return FALSE;
  }

  // Handle specific events.
  switch (event) {
   case EVENT_DETACH:
    gc_DevData.flags.val        = 0;
    gc_DevData.ID.deviceAddress = 0;

#ifdef DEBUG_MODE
    UART2PrintString("ANDR: Android Client Device Detached: address=");
    UART2PutDec(address);
    UART2PrintString("\r\n");
#endif
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
        print0("Received message: ");
        print_message(((HOST_TRANSFER_DATA *)data)->pUserData, ((HOST_TRANSFER_DATA *)data)->dataCount);
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
    break;
  }

  return USB_HOST_APP_EVENT_HANDLER(address, event, data, size);
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

  print1("Requested read of %u bytes", (unsigned) length);

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
          print0("Received message with %d bytes", byteCount);
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

  print1("Sending message with %u bytes: ", (unsigned) length);
  print_message(buffer, length);

  // Set the busy flag and start a new OUT transfer.
  gc_DevData.flags.txBusy = 1;
  RetVal = USBHostWrite(gc_DevData.ID.deviceAddress, gc_DevData.outEndpoint, (BYTE *)buffer, length);
  if (RetVal != USB_SUCCESS) {
    gc_DevData.flags.txBusy = 0;    // Clear flag to allow re-try
  }
  return RetVal;
}  // USBHostAndroidWrite
