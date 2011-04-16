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

#ifndef __XML_H__
#define __XML_H__

#define XML_BUF_SIZE 128

typedef enum {
  XML_STATE_CHARACTERS,
  XML_STATE_ELEMENT_NAME,
  XML_STATE_ELEMENT_OPEN,
  XML_STATE_ELEMENT_OPEN_EMPTY,
  XML_STATE_ATTRIBUTE_NAME,
  XML_STATE_ATTRIBUTE_WAIT_EQ,
  XML_STATE_ATTRIBUTE_WAIT_QUOT,
  XML_STATE_ATTRIBUTE_VALUE,
  XML_STATE_ELEMENT_CLOSE,
  XML_STATE_ELEMENT_CLOSE_NAME,
  XML_STATE_PROC_INST_WAIT_CLOSE,
  XML_STATE_ERROR
} XML_STATE;

typedef struct {
  char buf[XML_BUF_SIZE];
  int buf_pos;
  int attr_use_apos;
  int is_proc_inst;
  XML_STATE state;
} XML_CONTEXT;

typedef struct {
  void (*StartElement)(const char* name);
  void (*EndElement)(const char* name);
  void (*StartAttribute)(const char* name);
  void (*EndAttribute)();
  void (*Characters)(const char* characters, int size);
  void (*Error)();
} XML_CALLBACKS;

void XMLInit(XML_CONTEXT* state);
void XMLProcess(const char* data, int size, XML_CONTEXT* context, XML_CALLBACKS* callbacks);


#endif  // __XML_H__
