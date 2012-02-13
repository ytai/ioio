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

//
// USB Host Bluetooth dongle Driver (Header File)
//
// The driver passes on the following events to the application event handler
// (defined by the USB_HOST_APP_EVENT_HANDLER macro):
// - EVENT_DETACH - when device is detached.
// - EVENT_SUSPEND - when device is suspended.
// - EVENT_RESUME - when device is resumed.
//
// This driver can be configured to either use transfer events from usb_host.c
// or use a polling mechanism.  If USB_ENABLE_TRANSFER_EVENT is defined, this
// driver will utilize transfer events.  Otherwise, this driver will utilize
// polling.
//
// Since the generic class is performed with interrupt transfers,
// USB_SUPPORT_INTERRUPT_TRANSFERS must be defined.

#ifndef __USBHOSTBLUETOOTH_H__
#define __USBHOSTBLUETOOTH_H__

#include "GenericTypeDefs.h"
#include "usb_config.h"
#include "USB/usb_common.h"
#include "USB/usb_host.h"
#include "usb_host_driver_common.h"

//////////////////////////////////////////////////////////////////////////////////
//// The following code is not intended for the client to use directly.
//////////////////////////////////////////////////////////////////////////////////

typedef struct {
  BYTE address;
  BOOL busy;
  void *data;
} BLUETOOTH_ENDPOINT;

// Generic Device Information
// This structure contains information about an attached device, including
// status flags and device identification.
typedef struct {
  USB_DEVICE_ID      ID;                // Identification information about the device
  BOOL               initialized;       // Is the driver initialized
  BYTE               driverID;          // Driver ID
  BYTE               interface;         // Index of the relevant index within the device

  BLUETOOTH_ENDPOINT bulkIn;
  BLUETOOTH_ENDPOINT intIn;
  BLUETOOTH_ENDPOINT bulkOut;
  BLUETOOTH_ENDPOINT ctrlOut;
} BLUETOOTH_DEVICE;

extern BLUETOOTH_DEVICE gc_BluetoothDevData; // Information about the attached device.

////////////////////////////////////////////////////////////////////////////////
// The following two functions are to be put in the driver table and called by
// the USB host layer. Should not be called directly by the client.
////////////////////////////////////////////////////////////////////////////////

BOOL USBHostBluetoothInit(BYTE address,
                          DWORD flags,
                          BYTE clientDriverID,
                          USB_DEVICE_INFO *pDevInfo,
                          USB_INTERFACE_INFO *pIntInfo);

BOOL USBHostBluetoothEventHandler(BYTE address,
                                  USB_EVENT event,
                                  void *data,
                                  DWORD size );

////////////////////////////////////////////////////////////////////////////////
// The following code is the public API of the driver - should be used by the
// client.
////////////////////////////////////////////////////////////////////////////////

typedef enum {
  BLUETOOTH_EVENT_ATTACHED,
  BLUETOOTH_EVENT_DETACHED,
  BLUETOOTH_EVENT_READ_BULK_DONE,
  BLUETOOTH_EVENT_READ_INTERRUPT_DONE,
  BLUETOOTH_EVENT_WRITE_BULK_DONE,
  BLUETOOTH_EVENT_WRITE_CONTROL_DONE,
} BLUETOOTH_EVENT;

// Client must implement this callback for getting notifications from the
// bluetooth driver.
BOOL USBHostBluetoothCallback(BLUETOOTH_EVENT event,
                              USB_EVENT status,
                              void *data,
                              DWORD size);

// Check whether a device is currently attached.
#define USBHostBluetoothIsDeviceAttached() (gc_BluetoothDevData.initialized)

// Resets the device and restarts all the attachment process.
// Device must be attached.
void USBHostBluetoothReset();

// Returns the ID of the currently attached device.
// Device must be attached at the time of call, and argument must not be NULL.
void USBHostBluetoothGetDeviceId(USB_DEVICE_ID *pDevID);

// Issue a read request from the bulk in endpoint.
// Actual read will be done asynchronously. Client should wait for a callback
// upon termination.
// Returns USB_SUCCESS if succeeded.
// Device must be attached.
BYTE USBHostBluetoothReadBulk(void *buffer, DWORD length);

#define USBHostBlueToothBulkInBusy() (gc_BluetoothDevData.bulkIn.busy)

// Issue a read request from the interrupt in endpoint.
// Actual read will be done asynchronously. Client should wait for a callback
// upon termination.
// Returns USB_SUCCESS if succeeded.
// Device must be attached.
BYTE USBHostBluetoothReadInt(void *buffer, DWORD length);

#define USBHostBlueToothIntInBusy() (gc_BluetoothDevData.intIn.busy)

// Issue a write request to the bulk output endpoint.
// Actual write will be done asynchronously. Client should wait for a callback
// upon termination.
// Returns USB_SUCCESS if succeeded.
// Device must be attached.
BYTE USBHostBluetoothWriteBulk(const void *buffer, DWORD length);

#define USBHostBlueToothBulkOutBusy() (gc_BluetoothDevData.bulkOut.busy)

// Issue a write request to the control output endpoint.
// Actual write will be done asynchronously. Client should wait for a callback
// upon termination.
// Returns USB_SUCCESS if succeeded.
// Device must be attached.
BYTE USBHostBluetoothWriteControl(const void *buffer, DWORD length);

#define USBHostBlueToothControlOutBusy() (gc_BluetoothDevData.ctrlOut.busy)

// This function must be called periodically by the client to provide context to
// the driver IF NOT working with transfer events (USB_ENABLE_TRANSFER_EVENT)
// It will poll for the status of transfers.
#ifndef USB_ENABLE_TRANSFER_EVENT
void USBHostBluetoothTasks( void );
#endif  // USB_ENABLE_TRANSFER_EVENT


#endif  // __USBHOSTBLUETOOTH_H__
