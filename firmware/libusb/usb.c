/*
 * Copyright 2012 Ytai Ben-Tsvi. All rights reserved.
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

#include "USB/usb.h"
#ifndef DISABLE_BLUETOOTH
#include "usb_host_bluetooth.h"
#endif
#if defined(USB_SUPPORT_HOST) && !(defined(DISABLE_ACCESSORY) && defined(DISABLE_ADB))
#include "usb_host_android.h"
#endif
#ifdef USB_SUPPORT_DEVICE
#include "usb_device.h"
#endif
#ifdef USB_USE_CDC
#include "usb_device_cdc.h"
#endif

void USBInitialize() {
#ifdef USB_SUPPORT_OTG
  USBOTGInitialize();
#else
#ifdef USB_SUPPORT_HOST
  USBHostInit(0);
#endif
#ifdef USB_SUPPORT_DEVICE
  USBDeviceInit();
#endif
#endif
}

void USBShutdown() {
#ifdef USB_SUPPORT_OTG
  USBOTGSession(END_SESSION);
#else
#ifdef USB_SUPPORT_HOST
  USBHostShutdown();
  USBHostTasks();
#endif
#ifdef USB_SUPPORT_DEVICE
  USBSoftDetach();
#endif
#endif
  // hard power off of the USB module to ensure no interrupts to follow.
  U1PWRCbits.USBPWR = 0;
}

int USBTasks() {
#ifdef USB_SUPPORT_OTG
  //If Role Switch Occurred Then
  if (USBOTGRoleSwitch()) {
    //Clear Role Switch Flag
    USBOTGClearRoleSwitch();
  }
  if (!USBOTGHnpIsActive()) {
    if (USBOTGCurrentRoleIs() == ROLE_DEVICE) {
#ifdef USB_SUPPORT_DEVICE
#ifdef USB_INTERRUPT
      if (USB_BUS_SENSE && (USBGetDeviceState() == DETACHED_STATE)) {
        USBDeviceAttach();
      }
#endif
#ifdef USB_POLLING
      USBDeviceTasks();
#endif
#ifdef USB_USE_CDC
      USBDeviceCDCTasks();
#endif
#else
      if (USBIDIF && USBIDIE) {
        //Re-detect & Initialize
        USBOTGInitialize();
        USBClearInterruptFlag(USBIDIFReg, USBIDIFBitNum);
      }
#endif
    }
#ifdef USB_SUPPORT_HOST
    if (USBOTGCurrentRoleIs() == ROLE_HOST) {
      USBHostTasks();
#ifndef USB_ENABLE_TRANSFER_EVENT
      USBHostAndroidTasks();
#ifndef DISABLE_BLUETOOTH
      USBHostBluetoothTasks();
#endif
#endif
#endif
    }
  }
  return USBOTGCurrentRoleIs();
#else
#ifdef USB_SUPPORT_DEVICE
#ifdef USB_POLLING
  USBDeviceTasks();
#endif
#ifdef USB_USE_CDC
  USBDeviceCDCTasks();
#endif
  return 0;
#endif
#ifdef USB_SUPPORT_HOST
  USBHostTasks();
#ifndef USB_ENABLE_TRANSFER_EVENT
  USBHostAndroidTasks();
#ifndef DISABLE_BLUETOOTH
  USBHostBluetoothTasks();
#endif
#endif
  return 1;
#endif
#endif
}
