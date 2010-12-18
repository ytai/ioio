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
