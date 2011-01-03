// This file introduces common types to the ADB layer.

#ifndef __ADBTYPES_H__
#define __ADBTYPES_H__

// A result code of ADB operations.
typedef enum {
  ADB_RESULT_OK,      // Operation succeeded.
  ADB_RESULT_ERROR,   // Operation failed.
  ADB_RESULT_BUSY     // Operation not yet complete.
} ADB_RESULT;



#endif  // __ADBTYPES_H__
