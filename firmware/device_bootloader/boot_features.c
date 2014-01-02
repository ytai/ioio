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
#include "boot_features.h"
#include "boot_protocol_defs.h"
#include "bootloader_defs.h"
#include "Compiler.h"
#include "flash.h"
#include "logging.h"

void BootProtocolSendMessage(const OUTGOING_MESSAGE* msg);

static void ReadFingerprintToBuffer(BYTE* buffer) {
  int i;
  DWORD addr = BOOTLOADER_FINGERPRINT_ADDRESS;
  for (i = 0; i < FINGERPRINT_SIZE / 2; ++i) {
    DWORD_VAL dw = {FlashReadDWORD(addr)};
    *buffer++ = dw.byte.LB;
    *buffer++ = dw.byte.HB;
    addr += 2;
  }
}

static bool IsFingerprintErased(const BYTE *fp) {
  int i;
  for (i = 0; i < FINGERPRINT_SIZE; ++i) {
    if (*fp++ != 0xFF) return false;
  }
  return true;
}

bool ReadFingerprint() {
  log_printf("ReadFingerprint()");
  OUTGOING_MESSAGE msg;
  msg.type = FINGERPRINT;
  ReadFingerprintToBuffer(msg.args.fingerprint.fingerprint);
  BootProtocolSendMessage(&msg);
  return true;
}

BYTE ReadOscTun() {
  return FlashReadDWORD(BOOTLOADER_OSCTUN_ADDRESS) & 0xFF;
}

bool WriteOscTun(BYTE tun) {
  DWORD dw = 0xFFFFFF00 | tun;
  return FlashWriteDWORD(BOOTLOADER_OSCTUN_ADDRESS, dw);
}

bool EraseFingerprint() {
  // First, read the fingerprint. Avoid a Flash cycle if already erased.
  BYTE fp[FINGERPRINT_SIZE];
  ReadFingerprintToBuffer(fp);
  if (IsFingerprintErased(fp)) return true;

  // We actually need to erase. Backup OscTun, erase, rewrite OscTun.
  BYTE tun = ReadOscTun();
  return FlashErasePage(BOOTLOADER_CONFIG_PAGE)
      && WriteOscTun(tun);
}

bool EraseConfig() {
  return FlashErasePage(BOOTLOADER_CONFIG_PAGE);
}

bool WriteFingerprint(BYTE fp[FINGERPRINT_SIZE]) {
  log_printf("WriteFingerprint()");

  if (!EraseFingerprint()) return false;
  int i;
  DWORD addr = BOOTLOADER_FINGERPRINT_ADDRESS;
  BYTE* p = fp;
  for (i = 0; i < FINGERPRINT_SIZE / 2; ++i) {
    DWORD_VAL dw = {0};
    dw.byte.LB = *p++;
    dw.byte.HB = *p++;
    if (!FlashWriteDWORD(addr, dw.Val)) return false;
    addr += 2;
  }
  return true;
}

void SendChecksum(WORD checksum) {
  log_printf("SendChecksum()");
  OUTGOING_MESSAGE msg;
  msg.type = CHECKSUM;
  msg.args.checksum.checksum = checksum;
  BootProtocolSendMessage(&msg);
}

void HardReset() {
  log_printf("HardReset()");
  log_printf("Rebooting...");
  Reset();
}

void CheckInterface(BYTE interface_id[8]) {
  OUTGOING_MESSAGE msg;
  msg.type = CHECK_INTERFACE_RESPONSE;
  msg.args.check_interface_response.supported
      = (memcmp(interface_id, PROTOCOL_IID_IOIO0001, 8) == 0);
  BootProtocolSendMessage(&msg);
}
