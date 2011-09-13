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
#include "usb_config.h"
#include "usb_host_bluetooth.h"
#include "logging.h"

BLUETOOTH_DEVICE gc_BluetoothDevData;

#define FOUND_BULK_IN   0x01
#define FOUND_BULK_OUT  0x02
#define FOUND_INTERRUPT 0x04
#define FOUND_ALL       0x07

BOOL USBHostBluetoothInit(BYTE address, DWORD flags, BYTE clientDriverID) {
  USB_DEVICE_INFO *pDevInfo = USBHostGetDeviceInfo();
  USB_INTERFACE_INFO *pIntInfo;
  USB_DEVICE_DESCRIPTOR *pDev;
  USB_ENDPOINT_INFO *pEndpoint;

  log_printf("USBHostBluetoothInit(0x%x, 0x%llx, 0x%x)", address, flags, clientDriverID);

  // Initialize state
  memset(&gc_BluetoothDevData, 0, sizeof gc_BluetoothDevData);
  
  // Save device the address, VID, & PID
  gc_BluetoothDevData.ID.deviceAddress = address;
  pDev = (USB_DEVICE_DESCRIPTOR *) USBHostGetDeviceDescriptor(address);
  gc_BluetoothDevData.ID.vid  =  pDev->idVendor;
  gc_BluetoothDevData.ID.pid  =  pDev->idProduct;

  // We're looking for an interface with one bulk in, one bulk out, and one
  // interrupt endpoints.
  // When found, save the endpoint addresses for the interfaces
  for (pIntInfo = pDevInfo->pInterfaceList; pIntInfo; pIntInfo = pIntInfo->next) {
    unsigned int found = 0;
    
    for (pEndpoint = pIntInfo->pCurrentSetting->pEndpointList;
         pEndpoint;
         pEndpoint = pEndpoint->next) {
      if (pEndpoint->bEndpointAddress & 0x80
          && pEndpoint->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_BULK
          && !(found & FOUND_BULK_IN)) {
        // found bulk input
        gc_BluetoothDevData.bulkinEndpoint = pEndpoint->bEndpointAddress;
        found |= FOUND_BULK_IN;
      } else if (!(pEndpoint->bEndpointAddress & 0x80)
          && pEndpoint->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_BULK
          && !(found & FOUND_BULK_OUT)) {
        // found bulk input
        gc_BluetoothDevData.bulkoutEndpoint = pEndpoint->bEndpointAddress;
        found |= FOUND_BULK_OUT;
      } else if (pEndpoint->bEndpointAddress & 0x80
          && pEndpoint->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_INTERRUPT
          && !(found & FOUND_INTERRUPT)) {
        // found bulk input
        gc_BluetoothDevData.intEndpoint = pEndpoint->bEndpointAddress;
        found |= FOUND_INTERRUPT;
      } else {
        break;
      }
    }
    if ((found & FOUND_ALL) == FOUND_ALL) {
      goto good;
    }
  }
  // if we got here, a matching interface was not found
  log_printf("Could not find a matching interface");
  return FALSE;

good:
  gc_BluetoothDevData.initialized = 1;
  log_printf("Bluetooth Client Initalized: flags=0x%lx address=%d VID=0x%x PID=0x%x",
              flags, address, gc_BluetoothDevData.ID.vid, gc_BluetoothDevData.ID.pid);
  
  return TRUE;
}
  

BOOL USBHostBluetoothEventHandler(BYTE address, USB_EVENT event, void *data, DWORD size) {
  // Make sure it was for our device
  if (address != gc_BluetoothDevData.ID.deviceAddress) return FALSE;

  // Handle specific events.
  switch (event) {
   case EVENT_DETACH:
    memset(&gc_BluetoothDevData, 0, sizeof gc_BluetoothDevData);

    log_printf("Bluetooth Client Device Detached: address=0x%x", address);
    break;

#ifdef USB_ENABLE_TRANSFER_EVENT
   case EVENT_BUS_ERROR:
   case EVENT_TRANSFER:
    if ((data != NULL) && (size == sizeof(HOST_TRANSFER_DATA))) {
      int i;
      DWORD dataCount = ((HOST_TRANSFER_DATA *)data)->dataCount;

      if (((HOST_TRANSFER_DATA *)data)->bEndpointAddress == gc_BluetoothDevData.intEndpoint) {
        gc_BluetoothDevData.rxLength = dataCount;
        gc_BluetoothDevData.rxErrorCode = ((HOST_TRANSFER_DATA *)data)->bErrorCode;
        log_printf("Received interrupt with error code 0x%x, %ld bytes: ", ((HOST_TRANSFER_DATA *)data)->bErrorCode, dataCount);
        log_print_buf(((HOST_TRANSFER_DATA *)data)->pUserData, dataCount);
#ifdef USB_BLUETOOTH_INTERRUPT_HANDLER
        USB_BLUETOOTH_INTERRUPT_HANDLER(((HOST_TRANSFER_DATA *)data)->bErrorCode, dataCount, ((HOST_TRANSFER_DATA *)data)->pUserData);
#endif
        return TRUE;
      }
      if (((HOST_TRANSFER_DATA *)data)->bEndpointAddress == gc_BluetoothDevData.bulkinEndpoint) {
        gc_BluetoothDevData.rxBusy = 0;
        gc_BluetoothDevData.rxLength = dataCount;
        gc_BluetoothDevData.rxErrorCode = ((HOST_TRANSFER_DATA *)data)->bErrorCode;
        log_printf("Received message with %ld bytes: ", dataCount);
        log_print_buf(((HOST_TRANSFER_DATA *)data)->pUserData, dataCount);
        return TRUE;
      }
      if (((HOST_TRANSFER_DATA *)data)->bEndpointAddress == gc_BluetoothDevData.bulkoutEndpoint) {
        gc_BluetoothDevData.txBusy = 0;
        gc_BluetoothDevData.txErrorCode = ((HOST_TRANSFER_DATA *)data)->bErrorCode;
        return TRUE;
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
}


void USBHostBluetoothGetDeviceId(USB_DEVICE_ID *pDevID) {
  assert(gc_BluetoothDevData.initialized);
  assert(pDevID != NULL);
  *pDevID = gc_BluetoothDevData.ID;
}


void USBHostBluetoothReset() {
  assert(USBHostBluetoothIsDeviceAttached());
  USBHostResetDevice(gc_BluetoothDevData.ID.deviceAddress);
  memset(&gc_BluetoothDevData, 0, sizeof gc_BluetoothDevData);
}

BYTE USBHostBluetoothRead(void *buffer, DWORD length) {
  BYTE RetVal;

  // Validate the call
  assert(gc_BluetoothDevData.initialized);
  if (gc_BluetoothDevData.rxBusy) return USB_BUSY;

  log_printf("Requested read of %u bytes", (unsigned) length);

  // Set the busy flag, clear the count and start a new IN transfer.
  gc_BluetoothDevData.rxBusy = 1;
  gc_BluetoothDevData.rxLength = 0;
  RetVal = USBHostRead(gc_BluetoothDevData.ID.deviceAddress,
                       gc_BluetoothDevData.bulkinEndpoint,
                       (BYTE *)buffer,
                       length);
  if (RetVal != USB_SUCCESS) {
   gc_BluetoothDevData.rxBusy = 0;    // Clear flag to allow re-try
  }
  return RetVal;
}

BOOL USBHostBluetoothRxIsComplete(BYTE *errorCode, DWORD *byteCount) {
  if (gc_BluetoothDevData.rxBusy) {
    return FALSE;
  } else {
    *byteCount = gc_BluetoothDevData.rxLength;
    *errorCode = gc_BluetoothDevData.rxErrorCode;
    return TRUE;
  }
}

#ifndef USB_ENABLE_TRANSFER_EVENT
void USBHostBluetoothTasks(void) {
  DWORD   byteCount;
  BYTE    errorCode;

  if (gc_BluetoothDevData.ID.deviceAddress && gc_BluetoothDevData.initialized) {
    if (gc_BluetoothDevData.rxBusy) {
      if (USBHostTransferIsComplete(gc_BluetoothDevData.ID.deviceAddress,
                                    gc_BluetoothDevData.bulkinEndpoint,
                                    &errorCode,
                                    &byteCount)) {
          gc_BluetoothDevData.rxBusy = 0;
          gc_BluetoothDevData.rxLength     = byteCount;
          gc_BluetoothDevData.rxErrorCode  = errorCode;
          log_printf("Received message with %ld bytes", byteCount);
      }
    }

    if (gc_BluetoothDevData.txBusy) {
      if (USBHostTransferIsComplete(gc_BluetoothDevData.ID.deviceAddress,
                                    gc_BluetoothDevData.bulkoutEndpoint,
                                    &errorCode,
                                    &byteCount)) {
        gc_BluetoothDevData.txBusy = 0;
        gc_BluetoothDevData.txErrorCode  = errorCode;
      }
    }
  }
}
#endif  // USB_ENABLE_TRANSFER_EVENT

BOOL USBHostBluetoothTxIsComplete(BYTE *errorCode) {
  if (gc_BluetoothDevData.txBusy) {
    return FALSE;
  } else {
    *errorCode = gc_BluetoothDevData.txErrorCode;
    return TRUE;
  }
}

BYTE USBHostBluetoothWrite(const void *buffer, DWORD length) {
  BYTE RetVal;

  // Validate the call
  assert(gc_BluetoothDevData.initialized);
  if (gc_BluetoothDevData.txBusy) return USB_BUSY;

  log_printf("Sending message with %u bytes to endpoint 0x%x: ",
             (unsigned) length,
             gc_BluetoothDevData.bulkoutEndpoint);
  log_print_buf(buffer, length);

  // Set the busy flag and start a new OUT transfer.
  gc_BluetoothDevData.txBusy = 1;
  RetVal = USBHostWrite(gc_BluetoothDevData.ID.deviceAddress,
                        gc_BluetoothDevData.bulkoutEndpoint,
                        (BYTE *)buffer,
                        length);
  if (RetVal != USB_SUCCESS) {
    gc_BluetoothDevData.txBusy = 0;    // Clear flag to allow re-try
  }
  return RetVal;
}
