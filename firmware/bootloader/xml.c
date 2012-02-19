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

#include "xml.h"

#include <assert.h>

void XMLInit(XML_CONTEXT* context) {
  assert(context);
  context->state = XML_STATE_CHARACTERS;
  context->buf_pos = 0;
}

static void FlushBuf(XML_CONTEXT* context, XML_CALLBACKS* callbacks) {
  context->buf[context->buf_pos] = '\0';
  switch (context->state) {
    case XML_STATE_CHARACTERS:
    case XML_STATE_ATTRIBUTE_VALUE:
      if (context->buf_pos) {
        callbacks->Characters(context->buf, context->buf_pos);
      }
      break;

    case XML_STATE_ELEMENT_NAME:
      if (context->buf_pos) {
        if (!context->is_proc_inst) {
          callbacks->StartElement(context->buf);
        }
      } else {
        callbacks->Error();
        context->state = XML_STATE_ERROR;
      }
      break;

    case XML_STATE_ELEMENT_OPEN_EMPTY:
      callbacks->EndElement(0);
      break;

    case XML_STATE_ATTRIBUTE_NAME:
      callbacks->StartAttribute(context->buf);
      break;

    case XML_STATE_ELEMENT_CLOSE_NAME:
      callbacks->EndElement(context->buf);
      break;

    default:
      assert(0);
  }
  context->buf_pos = 0;
}

void XMLProcess(const char* data, int size, XML_CONTEXT* context, XML_CALLBACKS* callbacks) {
  assert(data);
  assert(context);
  assert(callbacks);

  while (size-- && context->state != XML_STATE_ERROR) {
    char c = *data++;
    // make sure there are at least two characters remaining in the buffer
    if (context->buf_pos > XML_BUF_SIZE - 2) {
      if (context->state == XML_STATE_CHARACTERS
          || context->state == XML_STATE_ATTRIBUTE_VALUE) {
        FlushBuf(context, callbacks);
      } else {
        callbacks->Error();
        context->state = XML_STATE_ERROR;
        return;
      }
    }
    switch (context->state) {
      case XML_STATE_CHARACTERS:
        switch (c) {
          case '<':
            FlushBuf(context, callbacks);
            context->state = XML_STATE_ELEMENT_NAME;
            context->is_proc_inst = 0;
            break;

          default:
            context->buf[context->buf_pos++] = c;
        }
        break;

      case XML_STATE_ELEMENT_NAME:
        switch (c) {
          case ' ':
          case '\t':
          case '\r':
          case '\n':
            FlushBuf(context, callbacks);
            context->state = XML_STATE_ELEMENT_OPEN;
            break;

          case '>':
            FlushBuf(context, callbacks);
            context->state = XML_STATE_CHARACTERS;
            break;

          case '/':
            if (context->buf_pos) {
              FlushBuf(context, callbacks);
              context->state = XML_STATE_ELEMENT_OPEN_EMPTY;
            } else {
              context->state = XML_STATE_ELEMENT_CLOSE_NAME;
            }
            break;

          case '?':
            if (context->buf_pos == 0) {
              context->is_proc_inst = 1;
              break;
            }
            // fall-through on purpose

          default:
            // TODO: validate name characters
            context->buf[context->buf_pos++] = c;
        }
        break;

      case XML_STATE_ELEMENT_OPEN:
        assert(context->buf_pos == 0);
        switch (c) {
          case ' ':
          case '\t':
          case '\r':
          case '\n':
            break;

          case '>':
            if (!context->is_proc_inst) {
              context->state = XML_STATE_CHARACTERS;
            } else {
              callbacks->Error();
              context->state = XML_STATE_ERROR;
            }
            break;

          case '?':
            if (context->is_proc_inst) {
              context->state = XML_STATE_PROC_INST_WAIT_CLOSE;
            } else {
              callbacks->Error();
              context->state = XML_STATE_ERROR;
            }
            break;

          case '/':
            context->state = XML_STATE_ELEMENT_OPEN_EMPTY;
            break;

          default:
            // TODO: validate name characters
            context->buf[context->buf_pos++] = c;
            context->state = XML_STATE_ATTRIBUTE_NAME;
        }
        break;

      case XML_STATE_ELEMENT_OPEN_EMPTY:
        switch (c) {
          case ' ':
          case '\t':
          case '\r':
          case '\n':
            break;

          case '>':
            FlushBuf(context, callbacks);
            context->state = XML_STATE_CHARACTERS;
            break;

          default:
            callbacks->Error();
            context->state = XML_STATE_ERROR;
        }
        break;

      case XML_STATE_ATTRIBUTE_NAME:
        switch (c) {
          case ' ':
          case '\t':
          case '\r':
          case '\n':
            FlushBuf(context, callbacks);
            context->state = XML_STATE_ATTRIBUTE_WAIT_EQ;
            break;

          case '=':
            FlushBuf(context, callbacks);
            context->state = XML_STATE_ATTRIBUTE_WAIT_QUOT;
            break;

          default:
            context->buf[context->buf_pos++] = c;
        }
        break;

      case XML_STATE_ATTRIBUTE_WAIT_EQ:
        switch (c) {
          case ' ':
          case '\t':
          case '\r':
          case '\n':
            break;

          case '=':
            context->state = XML_STATE_ATTRIBUTE_WAIT_QUOT;
            break;

          default:
            callbacks->Error();
            context->state = XML_STATE_ERROR;
        }
        break;

      case XML_STATE_ATTRIBUTE_WAIT_QUOT:
        switch (c) {
          case ' ':
          case '\t':
          case '\r':
          case '\n':
            break;

          case '"':
            context->state = XML_STATE_ATTRIBUTE_VALUE;
            context->attr_use_apos = 0;
            break;

          case '\'':
            context->state = XML_STATE_ATTRIBUTE_VALUE;
            context->attr_use_apos = 1;
            break;

          default:
            callbacks->Error();
            context->state = XML_STATE_ERROR;
        }
        break;

      case XML_STATE_ATTRIBUTE_VALUE:
        switch (c) {
          case '"':
            if (!context->attr_use_apos) {
              FlushBuf(context, callbacks);
              callbacks->EndAttribute();
              context->state = XML_STATE_ELEMENT_OPEN;
            } else {
              context->buf[context->buf_pos++] = c;
            }
            break;

          case '\'':
            if (context->attr_use_apos) {
              FlushBuf(context, callbacks);
              callbacks->EndAttribute();
              context->state = XML_STATE_ELEMENT_OPEN;
            } else {
              context->buf[context->buf_pos++] = c;
            }
            break;

          default:
            context->buf[context->buf_pos++] = c;
        }
        break;

      case XML_STATE_ELEMENT_CLOSE:
        switch (c) {
          case ' ':
          case '\t':
          case '\r':
          case '\n':
            break;

          case '>':
            context->state = XML_STATE_CHARACTERS;
            break;

          default:
            callbacks->Error();
            context->state = XML_STATE_ERROR;
        }
        break;

      case XML_STATE_ELEMENT_CLOSE_NAME:
        switch (c) {
          case ' ':
          case '\t':
          case '\r':
          case '\n':
            FlushBuf(context, callbacks);
            context->state = XML_STATE_ELEMENT_CLOSE;
            break;

          case '>':
            FlushBuf(context, callbacks);
            context->state = XML_STATE_CHARACTERS;
            break;

          default:
            context->buf[context->buf_pos++] = c;
        }
        break;

      case XML_STATE_PROC_INST_WAIT_CLOSE:
        if (c == '>') {
          context->state = XML_STATE_CHARACTERS;
        } else {
          callbacks->Error();
          context->state = XML_STATE_ERROR;
        }
        break;

      default:
        assert(0);
    }
  }
  if (context->state == XML_STATE_CHARACTERS || context->state == XML_STATE_ATTRIBUTE_VALUE) {
    FlushBuf(context, callbacks);
  }
}
