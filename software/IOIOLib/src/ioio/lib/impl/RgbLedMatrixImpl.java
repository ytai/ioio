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
package ioio.lib.impl;

import java.io.IOException;

import ioio.lib.api.RgbLedMatrix;
import ioio.lib.api.exception.ConnectionLostException;

class RgbLedMatrixImpl extends AbstractResource implements RgbLedMatrix {
	byte[] frame_ = new byte[768];

	public RgbLedMatrixImpl(IOIOImpl ioio) throws ConnectionLostException {
		super(ioio);
	}

	@Override
	synchronized public void frame(short[] rgb565)
			throws ConnectionLostException {
		if (rgb565.length != 512) {
			throw new IllegalArgumentException("Frame length must be 512");
		}
		checkState();
		convert(rgb565, frame_);
		try {
			ioio_.protocol_.rgbLedMatrixFrame(frame_);
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	synchronized public void close() {
		super.close();
		ioio_.closeRgbLedMatrix();
	}
	
	private static void convert(short[] rgb565, byte[] dest) {
		int outIndex = 0;
		for (int subframe = 0; subframe < 3; ++subframe) {
			int inIndex = 0;
			for (int row = 0; row < 8; ++row) {
				for (int col = 0; col < 32; ++col) {
					int pixel1 = ((int) rgb565[inIndex]) & 0xFFFF;
					int pixel2 = ((int) rgb565[inIndex + 256]) & 0xFFFF;
	
					int r1 = (pixel1 >> (11 + 2 + subframe)) & 1;
					int g1 = (pixel1 >> (5 + 3 + subframe)) & 1;
					int b1 = (pixel1 >> (0 + 2 + subframe)) & 1;
	
					int r2 = (pixel2 >> (11 + 2 + subframe)) & 1;
					int g2 = (pixel2 >> (5 + 3 + subframe)) & 1;
					int b2 = (pixel2 >> (0 + 2 + subframe)) & 1;
					
					dest[outIndex++] = (byte) (
							r1 << 5 | g1 << 4 | b1 << 3 | r2 << 2 | g2 << 1 | b2 << 0);
					++inIndex;
				}
			}
		}
	}
}
