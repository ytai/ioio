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
package ioio.lib.api;

import ioio.lib.api.exception.ConnectionLostException;

/**
 * An interface to control an RGB LED matrix of any if these models:
 * <ul>
 * <li><a href="http://www.adafruit.com/products/420">Adafruit 32x16</a></li>
 * <li>SeeedStudio 32x32</li>
 * <li>SeeedStudio 32x32 (NEW)</li>
 * <li>SeeedStudio 32x16</li>
 * </ul>
 * <p>
 * Connection: In this diagram the female connector is depicted - it is a mirror
 * of the male on that is on the matrix board. Place the connector such that the
 * red wire is on the top left.
 * <pre>
 *          +-----+
 *     23   | o o |   24
 *          | o o |   22
 *     20   | o o |   21
 *          | o o |   19
 *     10   | o o |   7
 *          | o o |   11
 *     27   | o o |   25
 *     GND  | o o |   28
 *          +-----+
 * </pre>
 * 
 * On the SeeedStudio 32x16 model, pins 19, 20, 21 can be left unconnected.
 * 
 */
public interface RgbLedMatrix extends Closeable {
	enum Matrix {
		ADAFRUIT_32x16(32, 16),
		SEEEDSTUDIO_32x16(32, 16),
		SEEEDSTUDIO_32x32(32, 32),
		SEEEDSTUDIO_32x32_NEW(32, 32);
		
		public final int width;
		public final int height;
		
		private Matrix(int w, int h) {
			width = w;
			height = h;
		}
	}
	
	/**
	 * Write a frame. Buffer must be of size 512 for 32x16 or 1024 for 32x32,
	 * where each element is a pixel with the following ordering:
	 * 
	 * <pre>
	 *   0    1    2       ...  31
	 *   32   33   34      ...  63
	 *   ...
	 *   480  481  482     ...  512
	 * </pre>
	 * 
	 * Or for 32x32:
	 * 
	 * <pre>
	 *   0    1    2       ...  31
	 *   32   33   34      ...  63
	 *   ...
	 *   992  993  994     ...  1023
	 * </pre>
	 * 
	 * The encoding is RGB565.
	 * 
	 * @param rgb565
	 *            The frame.
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 */
	void frame(short rgb565[]) throws ConnectionLostException;
	public void interactive() throws ConnectionLostException;
	public void playFile() throws ConnectionLostException;
	public void writeFile(float fps) throws ConnectionLostException;
	
}
