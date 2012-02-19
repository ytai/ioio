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

#include "auth.h"

#include <string.h>

#include "xml.h"
#include "bootloader_defs.h"

typedef enum {
  AUTH_STATE_DONE_PASS = AUTH_DONE_PASS,
  AUTH_STATE_DONE_FAIL = AUTH_DONE_FAIL,
  AUTH_STATE_ERROR = AUTH_DONE_PARSE_ERROR,
  AUTH_STATE_TOP = AUTH_BUSY,
  AUTH_STATE_PACKAGES,
  AUTH_STATE_PACKAGE,
  AUTH_STATE_NAME_ATTR,
  AUTH_STATE_SIGS,
  AUTH_STATE_CERT,
  AUTH_STATE_CERT_KEY,
  AUTH_STATE_CERT_FAIL,
} AUTH_STATE;

static const char FORCEROM cert_key[] =
  "30820312308202d0a00302010202044cdeff27300b06072a8648ce3804030500"
  "306b310b300906035504061302494c3110300e06035504081307556e6b6e6f77"
  "6e3111300f0603550407130854656c2d41766976310d300b060355040a130449"
  "4f494f3110300e060355040b1307556e6b6e6f776e311630140603550403130d"
  "597461692d42656e2d547376693020170d3130313131333231313230375a180f"
  "32313130313032303231313230375a306b310b300906035504061302494c3110"
  "300e06035504081307556e6b6e6f776e3111300f0603550407130854656c2d41"
  "766976310d300b060355040a1304494f494f3110300e060355040b1307556e6b"
  "6e6f776e311630140603550403130d597461692d42656e2d54737669308201b8"
  "3082012c06072a8648ce3804013082011f02818100fd7f53811d75122952df4a"
  "9c2eece4e7f611b7523cef4400c31e3f80b6512669455d402251fb593d8d58fa"
  "bfc5f5ba30f6cb9b556cd7813b801d346ff26660b76b9950a5a49f9fe8047b10"
  "22c24fbba9d7feb7c61bf83b57e7c6a8a6150f04fb83f6d3c51ec3023554135a"
  "169132f675f3ae2b61d72aeff22203199dd14801c70215009760508f15230bcc"
  "b292b982a2eb840bf0581cf502818100f7e1a085d69b3ddecbbcab5c36b857b9"
  "7994afbbfa3aea82f9574c0b3d0782675159578ebad4594fe67107108180b449"
  "167123e84c281613b7cf09328cc8a6e13c167a8b547c8d28e0a3ae1e2bb3a675"
  "916ea37f0bfa213562f1fb627a01243bcca4f1bea8519089a883dfe15ae59f06"
  "928b665e807b552564014c3bfecf492a038185000281810090e22346696394b3"
  "9a0e31202f4232eba95e4e71c71c1a5b7b39042509df889f6c60f54198569ff3"
  "15c06015e82a9286d2a1f628878422cb4ecf9db0173353dbf149424dfa934dcc"
  "7e1845cd415ed8f066581aab8ee3141fa99449597b8ebde2ea8eee0a9ee65e25"
  "877b36a00194b48b6dbac9324c33f2792ad22e6271584dc7300b06072a8648ce"
  "3804030500032f00302c02145a05308f318f9bb7783b09352dbdcf218c6bd97a"
  "02146464d844931dd0a4359f876305339f4ced0a4a35";

static AUTH_STATE state;
static int depth;
static int name_matches;
static char package_name[sizeof(manager_app_name)];
static int package_name_cursor;
static int package_key_cursor;

static void StartElement(const char* name) {
  switch (state) {
    case AUTH_STATE_TOP:
      if (depth == 0 && strcmp(name, "packages") == 0) {
        state = AUTH_STATE_PACKAGES;
      }
      break;

    case AUTH_STATE_PACKAGES:
      if (depth == 1 && strcmp(name, "package") == 0) {
        state = AUTH_STATE_PACKAGE;
        name_matches = 0;
      }
      break;

    case AUTH_STATE_PACKAGE:
      if (depth == 2 && name_matches && strcmp(name, "sigs") == 0) {
        state = AUTH_STATE_SIGS;
      }
      break;

    case AUTH_STATE_SIGS:
      if (depth == 3 && name_matches && strcmp(name, "cert") == 0) {
        state = AUTH_STATE_CERT;
      }
      break;

    default:
      break;  // nothing
  }
  ++depth;
}

static void EndElement(const char* name) {
  switch (state) {
    case AUTH_STATE_PACKAGES:
      if (depth == 1) {
        state = AUTH_STATE_TOP;
      }
      break;

    case AUTH_STATE_PACKAGE:
      if (depth == 2) {
        state = AUTH_STATE_PACKAGES;
      }
      break;

    case AUTH_STATE_SIGS:
      if (depth == 3) {
        state = AUTH_STATE_PACKAGE;
      }
      break;

    case AUTH_STATE_CERT:
      if (depth == 4) {
        state = AUTH_STATE_SIGS;
      }
      break;

    default:
      break;  // nothing
  }
  --depth;
}

static void StartAttribute(const char* name) {
  switch (state) {
    case AUTH_STATE_PACKAGE:
      if (depth == 2 && strcmp(name, "name") == 0) {
        package_name_cursor = 0;
        state = AUTH_STATE_NAME_ATTR;
      }
      break;

    case AUTH_STATE_CERT:
      if (depth == 4 && strcmp(name, "key") == 0) {
        package_key_cursor = 0;
        state = AUTH_STATE_CERT_KEY;
      }
      break;

    default:
      break;  // nothing
  }

  ++depth;
}

static void EndAttribute() {
  --depth;
  switch (state) {
    case AUTH_STATE_NAME_ATTR:
      if (package_name_cursor == sizeof(manager_app_name) - 1
          && memcmp(package_name, manager_app_name, package_name_cursor) == 0) {
        name_matches = 1;
      }
      state = AUTH_STATE_PACKAGE;
      break;

    case AUTH_STATE_CERT_KEY:
    case AUTH_STATE_CERT_FAIL:
      if (state == AUTH_STATE_CERT_KEY && package_key_cursor == sizeof(cert_key) - 1) {
        state = AUTH_STATE_DONE_PASS;
      } else {
        state = AUTH_STATE_DONE_FAIL;
      }
      break;

    default:
      break;  // nothing
  }
}

static void Characters(const char* characters, int size) {
  switch (state) {
    case AUTH_STATE_NAME_ATTR:
      if (package_name_cursor + size <= sizeof(package_name)) {
        memcpy(package_name + package_name_cursor, characters, size);
        package_name_cursor += size;
      } else {
        state = AUTH_STATE_PACKAGE;
      }
      break;

    case AUTH_STATE_CERT_KEY:
      if (size + package_key_cursor < sizeof(cert_key)
          && memcmp(cert_key + package_key_cursor, characters, size) == 0) {
        package_key_cursor += size;
      } else {
        state = AUTH_STATE_CERT_FAIL;
      }
      break;

    default:
      break;  // nothing
  }
}

static void Error() {
  state = AUTH_STATE_ERROR;
}

static XML_CONTEXT context;
static XML_CALLBACKS callbacks = {
  &StartElement,
  &EndElement,
  &StartAttribute,
  &EndAttribute,
  &Characters,
  &Error
};

void AuthInit() {
  state = AUTH_STATE_TOP;
  depth = 0;
  XMLInit(&context);
}

AUTH_RESULT AuthProcess(const char* data, int size) {
    XMLProcess(data, size, &context, &callbacks);
    if (context.state == XML_STATE_ERROR) state = AUTH_DONE_PARSE_ERROR;
    if (state >= AUTH_BUSY) return AUTH_BUSY;
    return (AUTH_STATE) state;
}
