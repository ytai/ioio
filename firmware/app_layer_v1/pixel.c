#include "Compiler.h"
#include "libconn/connection.h"
#include "features.h"
#include "protocol.h"
#include "logging.h"
#include "rgb_led_matrix.h"

#include "sdcard/FSIO.h"
#include "sdcard/SD-SPI.h"
#include "timer.h"
#include "pixel.h"
#include <stdio.h>

#define ANIMATION_FILENAME "bubbles.bin"
#define METADATA_FILENAME "metadata.bin"
#define SHIFTER_LENGTH_FILENAME "shift_len.bin"

typedef enum {
  STATE_NONE,
  STATE_PLAY_FILE,
  STATE_WRITE_FILE,
  STATE_INTERACTIVE
} STATE;

static STATE state = STATE_NONE;
static int frame_delay;
static int shifter_len_32;
static FSFILE *animation_file;
static FSFILE *metadata_file;
static FSFILE *shifter_length_file;

// TODO: this can be optimized - we already have a buffer of this size for this
// purpose in RgbLedMatrix.
BYTE frame[1536] __attribute__((far));

////////////////////////////////////////////////////////////////////////////////
// PlayFile stuff

static void StartPlayFile() {
  
  if (!FSInit()) {
    // Failed to initialize FAT16 ? do something?
    return;
  }

  // Read metadata file.
  //Open the shifter length file.

  BYTE shiftbuff[sizeof(int)];
  shifter_length_file = FSfopen(SHIFTER_LENGTH_FILENAME, "r");
  if (!shifter_length_file) return;
  FSfread(shiftbuff, sizeof( int ), 1, shifter_length_file);
  FSfclose(shifter_length_file);

  //Store the shifter length
  shifter_len_32 = shiftbuff[0] | (shiftbuff[1]  << 8);



  //Open the metadata File
  BYTE buff[sizeof(int)];
  metadata_file = FSfopen(METADATA_FILENAME, "r");
  if (!metadata_file) return;
  FSfread(buff, sizeof( int ), 1, metadata_file);
  FSfclose(metadata_file);

  //Store the framerate into frame_delay
  frame_delay = buff[0] | (buff[1]  << 8);


  // Open the animation file.
  animation_file = FSfopen(ANIMATION_FILENAME, "r");
  if (!animation_file) {
    // Either file is not present or card is not present
    return;
  }

  // Initialize the matrix.
  RgbLedMatrixEnable(shifter_len_32);

  // Initialize the timer.
  // Stop the timer
  T1CON = 0x0000;
  // Period is (PR1 + 1) / 62500 (seconds)
  PR1 = frame_delay;
  TMR1 = 0x0000;
  _T1IF = 0;
  // Start the timer @ 62.5[kHz]
  T1CON = 0x8030;

  state = STATE_PLAY_FILE;
}

static void MaybeFrameFromFile() {
  // If our timer elapsed, push a frame to the display.
  if (_T1IF) {
    _T1IF = 0;
    FSfread(frame, 768 * shifter_len_32, 1, animation_file);
    RgbLedMatrixFrame(frame);
    if (FSfeof(animation_file)) {
      // Rewind
      FSfseek(animation_file, 0, SEEK_SET);
    }
  }
}

static void StopPlayFile() {
  // Stop the timer.
  T1CON = 0x0000;

  // Close the file.
  FSfclose(animation_file);

  // Close the matrix.
  RgbLedMatrixEnable(0);

  state = STATE_NONE;
}

////////////////////////////////////////////////////////////////////////////////
// WriteFile stuff

static void StartWriteFile(int fd, int sl32) {
 
  // Initialize
  if (!FSInit()) {
    // Failed to initialize FAT16 ? do something?
    return;
  }
  
  // Write the shifterlength file.
  BYTE shiftbuff[sizeof( int ) ];
  shiftbuff[0] = (BYTE)((sl32 & 0x00FF));
  shiftbuff[1] = (BYTE)((sl32 & 0xFF00) >> 8);

  shifter_length_file = FSfopen(SHIFTER_LENGTH_FILENAME, "w");
  if (!shifter_length_file) return;
  FSfwrite(shiftbuff, sizeof shiftbuff, 1, shifter_length_file);
  FSfclose(shifter_length_file);
  // write the arguments into the meta file.

  // Write the metadata file.
  BYTE buff[sizeof( int ) ];
  buff[0] = (BYTE)((fd & 0x00FF));
  buff[1] = (BYTE)((fd & 0xFF00) >> 8);

  metadata_file = FSfopen(METADATA_FILENAME, "w");
  if (!metadata_file) return;
  FSfwrite(buff, sizeof buff, 1, metadata_file);
  FSfclose(metadata_file);
  // write the arguments into the meta file.
  
  // Open the animation file for writing.
  animation_file = FSfopen(ANIMATION_FILENAME, "w");
  if (!animation_file) {
    // Either file is not present or card is not present
    return;
  }

  frame_delay = fd;
  shifter_len_32 = sl32;
  state = STATE_WRITE_FILE;
}

static void WriteFrameToFile(const BYTE f[]) {
  // Write the frame to the animation file.
  FSfwrite(f, 768 * shifter_len_32, 1, animation_file);
}

static void StopWriteFile() {
  // Close the animation file.
  FSfclose(animation_file);
  state = STATE_NONE;
}

////////////////////////////////////////////////////////////////////////////////
// Interactive stuff

static void StartInteractive(int shifter_len_32) {
  // Intialize the matrix.
  RgbLedMatrixEnable(shifter_len_32);

  state = STATE_INTERACTIVE;
}

static void StopInteractive() {
  // Close the matrix.
  RgbLedMatrixEnable(0);

  state = STATE_NONE;
}

////////////////////////////////////////////////////////////////////////////////
// External API

void PixelInit() {
  StartPlayFile();
}

void PixelTasks() {
  switch (state) {
    case STATE_PLAY_FILE:
      MaybeFrameFromFile();
      break;

    case STATE_NONE:
    case STATE_WRITE_FILE:
    case STATE_INTERACTIVE:
      // Nothing
      break;
  }
}


void PixelFrame(const BYTE frame[]) {
  switch (state) {
    case STATE_NONE:
    case STATE_PLAY_FILE:
      // Ignore.
      break;

    case STATE_WRITE_FILE:
      WriteFrameToFile(frame);
      break;

    case STATE_INTERACTIVE:
      RgbLedMatrixFrame(frame);
      break;
  }
}

static void ExitCurrentState() {
  switch (state) {
    case STATE_NONE:
      // Nothing
      break;

    case STATE_PLAY_FILE:
      StopPlayFile();
      break;

    case STATE_WRITE_FILE:
      StopWriteFile();
      break;

    case STATE_INTERACTIVE:
      StopInteractive();
      break;
  }
}

void PixelInteractive(int shifter_len_32) {
  ExitCurrentState();
  StartInteractive(shifter_len_32);
}

void PixelWriteFile(int frame_delay, int shifter_len_32) {
  ExitCurrentState();
  StartWriteFile(frame_delay, shifter_len_32);
}

void PixelPlayFile() {
  ExitCurrentState();
  StartPlayFile();
}
