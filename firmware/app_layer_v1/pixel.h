/* 
 * File:   pixel.h
 * Author: mchintamani
 *
 * Created on December 3, 2013, 12:23 AM
 */


#ifndef PIXEL_H
#define	PIXEL_H


// Gets called once, upon reset. Will put Pixel into the initial state, which
// is "play file".
void PixelInit();

// Gets called periodically, to do Pixel's recurring tasks.
void PixelTasks();

// Commits a frame to Pixel. This frame will either be written directly to the
// display or to the currently open file, depending on state.
void PixelFrame(const BYTE frame[]);

// Switch to interactive mode - any frame written to Pixel from this point on
// goes directly to the display.
void PixelInteractive(int shifter_len_32);

// Switch to write-file mode - any frame writen to Pixel from this point on
// goes to the currently open file.
void PixelWriteFile(int frame_delay, int shifter_len_32);

// Start playing animations from the file. Any frame coming in to Pixel at this
// state is discarded.
void PixelPlayFile();

#endif	/* PIXEL_H */

