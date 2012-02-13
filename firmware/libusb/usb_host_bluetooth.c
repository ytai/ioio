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

BOOL USBHostBluetoothInit(BYTE address,
                          DWORD flags,
                          BYTE clientDriverID,
                          USB_DEVICE_INFO *pDevInfo,
                          USB_INTERFACE_INFO *pIntInfo) {
  USB_DEVICE_DESCRIPTOR *pDev;
  USB_ENDPOINT_INFO *pEndpoint;

  log_printf("USBHostBluetoothInit(0x%x, 0x%lx, 0x%x)", address, flags, clientDriverID);

  // Initialize state
  memset(&gc_BluetoothDevData, 0, sizeof gc_BluetoothDevData);
  
  // Save device the address, VID, & PID
  gc_BluetoothDevData.ID.deviceAddress = address;
  pDev = (USB_DEVICE_DESCRIPTOR *) USBHostGetDeviceDescriptor(address);
  gc_BluetoothDevData.ID.vid  =  pDev->idVendor;
  gc_BluetoothDevData.ID.pid  =  pDev->idProduct;
  gc_BluetoothDevData.driverID = clientDriverID;

  // We're looking for an interface with:
  // - class 0xe0 subclass 0x01 protocol 0x01
  // - one bulk in, one bulk out, and one interrupt endpoints.
  // When found, save the endpoint addresses for the interfaces
  for (pIntInfo = pDevInfo->pInterfaceList; pIntInfo; pIntInfo = pIntInfo->next) {
    log_printf("Encoutered interface %d, class 0x%x, subclass 0x%x, protocol, 0x%x",
        pIntInfo->interface, pIntInfo->type.cls, pIntInfo->type.subcls, pIntInfo->type.proto);

    if (pIntInfo->type.cls != 0xE0
        || pIntInfo->type.subcls != 0x01
        || pIntInfo->type.proto != 0x01) continue;

    unsigned int found = 0;

    for (pEndpoint = pIntInfo->pCurrentSetting->pEndpointList;
         pEndpoint;
         pEndpoint = pEndpoint->next) {
      if (pEndpoint->bEndpointAddress & 0x80
          && pEndpoint->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_BULK
          && !(found & FOUND_BULK_IN)) {
        // found bulk input
        gc_BluetoothDevData.bulkIn.address = pEndpoint->bEndpointAddress;
        found |= FOUND_BULK_IN;
      } else if (!(pEndpoint->bEndpointAddress & 0x80)
          && pEndpoint->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_BULK
          && !(found & FOUND_BULK_OUT)) {
        // found bulk input
        gc_BluetoothDevData.bulkOut.address = pEndpoint->bEndpointAddress;
        found |= FOUND_BULK_OUT;
      } else if (pEndpoint->bEndpointAddress & 0x80
          && pEndpoint->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_INTERRUPT
          && !(found & FOUND_INTERRUPT)) {
        // found bulk input
        gc_BluetoothDevData.intIn.address = pEndpoint->bEndpointAddress;
        found |= FOUND_INTERRUPT;
      } else {
        break;
      }
    }
    if ((found & FOUND_ALL) == FOUND_ALL) {
      gc_BluetoothDevData.interface = pIntInfo->interface;
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

  USBHostBluetoothCallback(BLUETOOTH_EVENT_ATTACHED, USB_SUCCESS, NULL, 0);
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
    USBHostBluetoothCallback(BLUETOOTH_EVENT_DETACHED, USB_SUCCESS, NULL, 0);
    break;

#ifdef USB_ENABLE_TRANSFER_EVENT
   case EVENT_BUS_ERROR:
   case EVENT_TRANSFER:
    if ((data != NULL) && (size == sizeof(HOST_TRANSFER_DATA))) {
      DWORD dataCount = ((HOST_TRANSFER_DATA *)data)->dataCount;
      BYTE errorCode = ((HOST_TRANSFER_DATA *)data)->bErrorCode;
      BYTE endPoint = ((HOST_TRANSFER_DATA *)data)->bEndpointAddress;
      BYTE *userData = ((HOST_TRANSFER_DATA *)data)->pUserData;

      if (endPoint == gc_BluetoothDevData.intIn.address) {
        // if (dataCount) {
        //   log_printf("Received interrupt with error code 0x%x, %ld bytes: ", errorCode, dataCount);
        //   log_print_buf(userData, dataCount);
        // }
        USBHostBluetoothCallback(BLUETOOTH_EVENT_READ_INTERRUPT_DONE,
                                 errorCode,
                                 userData, dataCount);
        gc_BluetoothDevData.intIn.busy = 0;
        return TRUE;
      }
      if (endPoint == gc_BluetoothDevData.bulkIn.address) {
        // if (dataCount) {
        //   log_printf("Received message with %ld bytes: ", dataCount);
        //   log_print_buf(userData, dataCount);
        // }
        USBHostBluetoothCallback(BLUETOOTH_EVENT_READ_BULK_DONE,
                                 errorCode,
                                 userData, dataCount);
        gc_BluetoothDevData.bulkIn.busy = 0;
        return TRUE;
      }
      if (endPoint == gc_BluetoothDevData.bulkOut.address) {
        // log_printf("bulk out done: %d", ((HOST_TRANSFER_DATA *)data)->bErrorCode);
        USBHostBluetoothCallback(BLUETOOTH_EVENT_WRITE_BULK_DONE,
                                 errorCode,
                                 NULL, 0);
        gc_BluetoothDevData.bulkOut.busy = 0;
        return TRUE;
      }
      if (endPoint == gc_BluetoothDevData.ctrlOut.address) {
        // log_printf("ctrl out done: %d", ((HOST_TRANSFER_DATA *)data)->bErrorCode);
        USBHostBluetoothCallback(BLUETOOTH_EVENT_WRITE_CONTROL_DONE,
                                 errorCode,
                                 NULL, 0);
        gc_BluetoothDevData.ctrlOut.busy = 0;
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

static BYTE USBHostBluetoothRead(BLUETOOTH_ENDPOINT *ep, void *buffer, DWORD length) {
  BYTE RetVal;

  // Validate the call
  assert(gc_BluetoothDevData.initialized);
  if (ep->busy) return USB_BUSY;

  //log_printf("Requested read of %u bytes", (unsigned) length);

  // Set the busy flag, clear the count and start a new IN transfer.
  ep->busy = 1;
  ep->data = buffer;
  RetVal = USBHostRead(gc_BluetoothDevData.ID.deviceAddress,
                       ep->address,
                       (BYTE *)buffer,
                       length);
  if (RetVal != USB_SUCCESS) {
   ep->busy = 0;    // Clear flag to allow re-try
  }
  return RetVal;
}

BYTE USBHostBluetoothReadBulk(void *buffer, DWORD length) {
  return USBHostBluetoothRead(&gc_BluetoothDevData.bulkIn, buffer, length);
}

BYTE USBHostBluetoothReadInt(void *buffer, DWORD length) {
  return USBHostBluetoothRead(&gc_BluetoothDevData.intIn, buffer, length);
}

#ifndef USB_ENABLE_TRANSFER_EVENT
void USBHostBluetoothTasks(void) {
  DWORD   byteCount;
  BYTE    errorCode;

  if (gc_BluetoothDevData.ID.deviceAddress && gc_BluetoothDevData.initialized) {
    if (gc_BluetoothDevData.intIn.busy) {
      if (USBHostTransferIsComplete(gc_BluetoothDevData.ID.deviceAddress,
                                    gc_BluetoothDevData.intIn.address,
                                    &errorCode,
                                    &byteCount)) {
//        if (byteCount) {
//          log_printf("Received interrupt with error code 0x%x, %ld bytes: ",
//                     errorCode, byteCount);
//          log_print_buf(gc_BluetoothDevData.intIn.data, byteCount);
//        }
        gc_BluetoothDevData.intIn.busy = 0;
        USBHostBluetoothCallback(BLUETOOTH_EVENT_READ_INTERRUPT_DONE,
                                 errorCode,
                                 gc_BluetoothDevData.intIn.data, byteCount);
      }
    }

    if (gc_BluetoothDevData.bulkIn.busy) {
      if (USBHostTransferIsComplete(gc_BluetoothDevData.ID.deviceAddress,
                                    gc_BluetoothDevData.bulkIn.address,
                                    &errorCode,
                                    &byteCount)) {
//        if (byteCount) {
//          log_printf("Received bulk with error code 0x%x, %ld bytes: ",
//                     errorCode, byteCount);
//          log_print_buf(gc_BluetoothDevData.bulkIn.data, byteCount);
//        }
        gc_BluetoothDevData.bulkIn.busy = 0;
        USBHostBluetoothCallback(BLUETOOTH_EVENT_READ_BULK_DONE,
                                 errorCode,
                                 gc_BluetoothDevData.bulkIn.data, byteCount);
      }
    }

    if (gc_BluetoothDevData.bulkOut.busy) {
      if (USBHostTransferIsComplete(gc_BluetoothDevData.ID.deviceAddress,
                                    gc_BluetoothDevData.bulkOut.address,
                                    &errorCode,
                                    &byteCount)) {
        gc_BluetoothDevData.bulkOut.busy = 0;
        USBHostBluetoothCallback(BLUETOOTH_EVENT_WRITE_BULK_DONE,
                                 errorCode,
                                 NULL, 0);
      }
    }

    if (gc_BluetoothDevData.ctrlOut.busy) {
      if (USBHostTransferIsComplete(gc_BluetoothDevData.ID.deviceAddress,
                                    gc_BluetoothDevData.ctrlOut.address,
                                    &errorCode,
                                    &byteCount)) {
        gc_BluetoothDevData.ctrlOut.busy = 0;
        USBHostBluetoothCallback(BLUETOOTH_EVENT_WRITE_CONTROL_DONE,
                                 errorCode,
                                 NULL, 0);
      }
    }
}
}
#endif  // USB_ENABLE_TRANSFER_EVENT

static BYTE USBHostBluetoothWrite(BLUETOOTH_ENDPOINT *ep, const void *buffer, DWORD length) {
  BYTE RetVal;

  // Validate the call
  assert(gc_BluetoothDevData.initialized);
  if (ep->busy) return USB_BUSY;

//  log_printf("Sending message with %u bytes to endpoint 0x%x: ",
//             (unsigned) length,
//             ep->address);
//  log_print_buf(buffer, length);

  // Set the busy flag and start a new OUT transfer.
  ep->busy = 1;

  if (ep->address == 0) {
    RetVal = USBHostIssueDeviceRequest(gc_BluetoothDevData.ID.deviceAddress,
                                       0x20, 0, 0,
                                       0,//gc_BluetoothDevData.interface,
                                       length, (BYTE *) buffer,
                                       USB_DEVICE_REQUEST_SET,
                                       gc_BluetoothDevData.driverID);
  } else {
    RetVal = USBHostWrite(gc_BluetoothDevData.ID.deviceAddress,
                          ep->address,
                          (BYTE *)buffer,
                          length);
  }

  if (RetVal != USB_SUCCESS) {
    log_printf("Write failed");
    ep->busy = 0;    // Clear flag to allow re-try
  }
  return RetVal;
}

BYTE USBHostBluetoothWriteBulk(const void *buffer, DWORD length) {
  return USBHostBluetoothWrite(&gc_BluetoothDevData.bulkOut, buffer, length);
}

BYTE USBHostBluetoothWriteControl(const void *buffer, DWORD length) {
  return USBHostBluetoothWrite(&gc_BluetoothDevData.ctrlOut, buffer, length);
}
