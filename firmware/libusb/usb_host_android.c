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

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "GenericTypeDefs.h"
#include "USB/usb.h"
#include "usb_host_android.h"

#include "logging.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

ANDROID_DEVICE gc_DevData;

BOOL USBHostAndroidInitInterface(BYTE address,
                                 DWORD iid,
                                 BYTE clientDriverID,
                                 USB_DEVICE_INFO *pDevInfo,
                                 USB_INTERFACE_INFO *pIntInfo) {
  USB_DEVICE_DESCRIPTOR *pDev = (USB_DEVICE_DESCRIPTOR *) USBHostGetDeviceDescriptor(address);
  USB_ENDPOINT_INFO *pFirstEpInfo;
  USB_ENDPOINT_INFO *pSecondEpInfo;
  ANDROID_INTERFACE *pInterface;
  log_printf("USBHostAndroidInitInterface(0x%x, 0x%lx, 0x%x)", address, iid, clientDriverID);
  assert(iid < ANDROID_INTERFACE_MAX);

  pInterface = &gc_DevData.interfaces[iid];
  pFirstEpInfo = pIntInfo->pCurrentSetting->pEndpointList;
  if (!pFirstEpInfo) { log_printf("No end-points"); return FALSE; }
  pSecondEpInfo = pFirstEpInfo->next;
  if (!pSecondEpInfo) { log_printf("Only one end-point"); return FALSE; }
  if (pSecondEpInfo->next) { log_printf("Too many end-points"); return FALSE; }

  if (pFirstEpInfo->bEndpointAddress & 0x80) {
      if (pSecondEpInfo->bEndpointAddress & 0x80) {
        log_printf("Need one input and one output end-point");
        return FALSE;
      }
      pInterface->inEndpoint = pFirstEpInfo->bEndpointAddress;
      pInterface->outEndpoint = pSecondEpInfo->bEndpointAddress;
  } else {
      if (!(pSecondEpInfo->bEndpointAddress & 0x80)) {
        log_printf("Need one input and one output end-point");
        return FALSE;
      }
      pInterface->inEndpoint = pSecondEpInfo->bEndpointAddress;
      pInterface->outEndpoint = pFirstEpInfo->bEndpointAddress;
  }
  pInterface->flags.initialized = 1;
  log_printf("Successfully initialized Android inteface %ld. IN_EP=0x%x OUT_EP=0x%x",
             iid, pInterface->inEndpoint, pInterface->outEndpoint);

  // Save device the address, VID, & PID
  gc_DevData.ID.deviceAddress = address;
  gc_DevData.ID.vid  =  pDev->idVendor;
  gc_DevData.ID.pid  =  pDev->idProduct;

  return TRUE;
}

BOOL USBHostAndroidInitDevice(BYTE address,
                              DWORD iid,
                              BYTE clientDriverID,
                              USB_DEVICE_INFO *pDevInfo,
                              USB_INTERFACE_INFO *pIntInfo) {
  log_printf("USBHostAndroidInitDevice(0x%x, 0x%lx, 0x%x)", address, iid, clientDriverID);

  // Initialize state
  memset(&gc_DevData, 0, sizeof gc_DevData);
  
  for (pIntInfo = pDevInfo->pInterfaceList; pIntInfo; pIntInfo = pIntInfo->next) {
    log_printf("Encoutered interface %d, class 0x%x, subclass 0x%x, protocol, 0x%x",
        pIntInfo->interface, pIntInfo->type.cls, pIntInfo->type.subcls, pIntInfo->type.proto);
    // initialize according to the interface number
    USBHostAndroidInitInterface(address, pIntInfo->interface, 0, pDevInfo, pIntInfo);
  }

  log_printf("Android device initalized: address=%d VID=0x%x PID=0x%x",
             address, gc_DevData.ID.vid, gc_DevData.ID.pid);
  
  return TRUE;
} //  USBHostAndroidInit
  

BOOL USBHostAndroidEventHandler(BYTE address, USB_EVENT event, void *data, DWORD size) {
  // Make sure it was for our device
  if (address != gc_DevData.ID.deviceAddress) return FALSE;

  // Handle specific events.
  switch (event) {
   case EVENT_DETACH:
    memset(&gc_DevData, 0, sizeof gc_DevData);

    log_printf("Android Client Device Detached: address=0x%x", address);
    break;

#ifdef USB_ENABLE_TRANSFER_EVENT
   case EVENT_BUS_ERROR:
   case EVENT_TRANSFER:
    if ((data != NULL) && (size == sizeof(HOST_TRANSFER_DATA))) {
      int i;
      DWORD dataCount = ((HOST_TRANSFER_DATA *)data)->dataCount;

      for (i = 0; i < ANDROID_INTERFACE_MAX; ++i) {
        ANDROID_INTERFACE *pInterface = &gc_DevData.interfaces[i];
        if (((HOST_TRANSFER_DATA *)data)->bEndpointAddress == pInterface->inEndpoint) {
          pInterface->flags.rxBusy = 0;
          pInterface->rxLength = dataCount;
          pInterface->rxErrorCode = ((HOST_TRANSFER_DATA *)data)->bErrorCode;
          log_printf("Received message with %ld bytes: ", ((HOST_TRANSFER_DATA *)data)->dataCount);
          log_print_buf(((HOST_TRANSFER_DATA *)data)->pUserData, ((HOST_TRANSFER_DATA *)data)->dataCount);
          return TRUE;
        }
        if (((HOST_TRANSFER_DATA *)data)->bEndpointAddress == pInterface->outEndpoint) {
          pInterface->flags.txBusy = 0;
          pInterface->txErrorCode = ((HOST_TRANSFER_DATA *)data)->bErrorCode;
          return TRUE;
        }
      }
    }
    return FALSE;
#endif

   case EVENT_SUSPEND:
   case EVENT_RESUME:
   default:
    return TRUE;
  }
  return FALSE;
}  // USBHostAndroidEventHandler


//void USBHostAndroidGetDeviceId(USB_DEVICE_ID *pDevID) {
//  assert(pDevID != NULL);
//  *pDevID = gc_DevData.ID;
//}  // USBHostAndroidGetDeviceId


void USBHostAndroidReset() {
  assert(gc_DevData.ID.deviceAddress != 0);
  USBHostResetDevice(gc_DevData.ID.deviceAddress);
  memset(&gc_DevData, 0, sizeof gc_DevData);
}

BYTE USBHostAndroidRead(void *buffer, DWORD length, ANDROID_INTERFACE_ID iid) {
  BYTE RetVal;
  ANDROID_INTERFACE *pInterface = &gc_DevData.interfaces[iid];

  // Validate the call
  assert(pInterface->flags.initialized);
  if (pInterface->flags.rxBusy) return USB_BUSY;

  log_printf("Requested read of %u bytes", (unsigned) length);

  // Set the busy flag, clear the count and start a new IN transfer.
  pInterface->flags.rxBusy = 1;
  pInterface->rxLength = 0;
  RetVal = USBHostRead(gc_DevData.ID.deviceAddress, pInterface->inEndpoint, (BYTE *)buffer, length);
  if (RetVal != USB_SUCCESS) {
    pInterface->flags.rxBusy = 0;    // Clear flag to allow re-try
  }
  return RetVal;
}  // USBHostAndroidRead

BOOL USBHostAndroidRxIsComplete(BYTE *errorCode, DWORD *byteCount, ANDROID_INTERFACE_ID iid) {
  ANDROID_INTERFACE *pInterface = &gc_DevData.interfaces[iid];

  if (pInterface->flags.rxBusy) {
    return FALSE;
  } else {
    *byteCount = pInterface->rxLength;
    *errorCode = pInterface->rxErrorCode;
    return TRUE;
  }
}  // USBHostAndroidRxIsComplete

#ifndef USB_ENABLE_TRANSFER_EVENT
void USBHostAndroidTasks(void) {
  DWORD   byteCount;
  BYTE    errorCode;
  int iid;

  for (iid = 0; iid < ANDROID_INTERFACE_MAX; ++iid) {
    ANDROID_INTERFACE *pInterface = &gc_DevData.interfaces[iid];

    if (gc_DevData.ID.deviceAddress && pInterface->flags.initialized) {
      if (pInterface->flags.rxBusy) {
        if (USBHostTransferIsComplete(gc_DevData.ID.deviceAddress, pInterface->inEndpoint, &errorCode, &byteCount)) {
          pInterface->flags.rxBusy = 0;
          pInterface->rxLength     = byteCount;
          pInterface->rxErrorCode  = errorCode;
          log_printf("Received message with %ld bytes on endpoint 0x%x", byteCount, pInterface->inEndpoint);
        }
      }

      if (pInterface->flags.txBusy) {
        if (USBHostTransferIsComplete(gc_DevData.ID.deviceAddress, pInterface->outEndpoint, &errorCode, &byteCount)) {
          pInterface->flags.txBusy = 0;
          pInterface->txErrorCode  = errorCode;
        }
      }
    }
  }
}  // USBHostAndroidTasks
#endif  // USB_ENABLE_TRANSFER_EVENT

BOOL USBHostAndroidTxIsComplete(BYTE *errorCode, ANDROID_INTERFACE_ID iid) {
  ANDROID_INTERFACE *pInterface = &gc_DevData.interfaces[iid];

  if ( pInterface->flags.txBusy) {
    return FALSE;
  } else {
    *errorCode =  pInterface->txErrorCode;
    return TRUE;
  }
}  // USBHostAndroidTxIsComplete


BYTE USBHostAndroidWrite(const void *buffer, DWORD length, ANDROID_INTERFACE_ID iid) {
  BYTE RetVal;
  ANDROID_INTERFACE *pInterface = &gc_DevData.interfaces[iid];

  // Validate the call
  assert(pInterface->flags.initialized);
  if (pInterface->flags.txBusy) return USB_BUSY;

  log_printf("Sending message with %u bytes to endpoint 0x%x: ",
             (unsigned) length,
             pInterface->outEndpoint);
  log_print_buf(buffer, length);

  // Set the busy flag and start a new OUT transfer.
  pInterface->flags.txBusy = 1;
  RetVal = USBHostWrite(gc_DevData.ID.deviceAddress, pInterface->outEndpoint, (BYTE *)buffer, length);
  if (RetVal != USB_SUCCESS) {
    pInterface->flags.txBusy = 0;    // Clear flag to allow re-try
  }
  return RetVal;
}  // USBHostAndroidWrite
