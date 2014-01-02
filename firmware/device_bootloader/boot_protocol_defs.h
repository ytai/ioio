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

#ifndef __BOOTPROTOCOLDEFS_H__
#define __BOOTPROTOCOLDEFS_H__

#include <GenericTypeDefs.h>

#define PACKED __attribute__ ((packed))

#define IOIO_MAGIC 0x4F494F49L
#define BOOT_MAGIC 0x544F4F42L

#define PROTOCOL_IID_IOIO0001 "BOOT0001"

// hard reset
typedef struct PACKED {
  DWORD magic;
} HARD_RESET_ARGS;

// establish connection
typedef struct PACKED {
  DWORD magic;
  BYTE hw_impl_ver[8];
  BYTE bl_impl_ver[8];
  BYTE plat_ver[8];
} ESTABLISH_CONNECTION_ARGS;

// check interface
typedef struct PACKED {
  BYTE interface_id[8];
} CHECK_INTERFACE_ARGS;

// check interface response
typedef struct PACKED {
  BYTE supported : 1;
  BYTE : 7;
} CHECK_INTERFACE_RESPONSE_ARGS;

// read fingerprint
typedef struct PACKED {
} READ_FINGERPRINT_ARGS;

// fingerprint
typedef struct PACKED {
  BYTE fingerprint[16];
} FINGERPRINT_ARGS;

// write fingerprint
typedef struct PACKED {
  BYTE fingerprint[16];
} WRITE_FINGERPRINT_ARGS;

// write image
typedef struct PACKED {
  DWORD size;
} WRITE_IMAGE_ARGS;

// checksum
typedef struct PACKED {
  WORD checksum;
} CHECKSUM_ARGS;

// reserved
typedef struct PACKED {
} RESERVED_ARGS;

// BOOKMARK(add_feature): Add a struct for the new incoming / outgoing message
// arguments.

typedef struct PACKED {
  BYTE type;
  union PACKED {
    HARD_RESET_ARGS        hard_reset;
    CHECK_INTERFACE_ARGS   check_interface;
    READ_FINGERPRINT_ARGS  read_fingerprint;
    WRITE_FINGERPRINT_ARGS write_fingerprint;
    WRITE_IMAGE_ARGS       write_image;
    // BOOKMARK(add_feature): Add argument struct to the union.
  } args;
} INCOMING_MESSAGE;

typedef struct PACKED {
  BYTE type;
  union PACKED {
    ESTABLISH_CONNECTION_ARGS     establish_connection;
    CHECK_INTERFACE_RESPONSE_ARGS check_interface_response;
    FINGERPRINT_ARGS              fingerprint;
    CHECKSUM_ARGS                 checksum;
    // BOOKMARK(add_feature): Add argument struct to the union.
  } args;
} OUTGOING_MESSAGE;


typedef enum {
  HARD_RESET                          = 0x00,
  ESTABLISH_CONNECTION                = 0x00,
  CHECK_INTERFACE                     = 0x01,
  CHECK_INTERFACE_RESPONSE            = 0x01,
  READ_FINGERPRINT                    = 0x02,
  FINGERPRINT                         = 0x02,
  WRITE_FINGERPRINT                   = 0x03,
  WRITE_IMAGE                         = 0x04,
  CHECKSUM                            = 0x04,

  // BOOKMARK(add_feature): Add new message type to enum.
  MESSAGE_TYPE_LIMIT
} MESSAGE_TYPE;


#endif  // __BOOTPROTOCOLDEFS_H__
