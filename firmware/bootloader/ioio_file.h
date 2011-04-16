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

// This module handles streamed processing of a IOIO image file and writing it
// to Flash.
// A IOIO file is essetially a list of <address, block> pairs, sorted by address,
// intended to be installed on Flash memory.
// The stream is processed block-by-block: whenever a block occpying a new page
// is encountered, the page is first erased. Then the block is written to Flash.
//
// Typical usage (error handling omitted):
// ...
// IOIOFileInit();
// while (have_more_data()) {
//   IOIOFileHandleBuffer(data, size);
// }
// IOIOFileDone();
//
// Whenever an error occurs due to a failed write or invalid file format, FALSE
// is returned.
// When this happens, a call to IOIOFileInit() will reset the state completely
// and another write sequence can be attempted.

#ifndef __IOIOFILE_H__
#define __IOIOFILE_H__

// Reset the state of this module, prepare to process a new file.
void IOIOFileInit();

// Handle a buffer of data.
// Data can be of any size, and can be safely discarded once the function exits.
// The function will return FALSE if file format is corrupt or a write failed.
BOOL IOIOFileHandleBuffer(const void * buffer, size_t size);

// Notify that EOF has been reached.
// The function will return FALSE if EOF is unexpeceed (e.g. we are still in the
// middle of a block.
BOOL IOIOFileDone();


#endif  // __IOIOFILE_H__
