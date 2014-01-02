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
	final Matrix kind_;
	byte[] frame_;

	public RgbLedMatrixImpl(IOIOImpl ioio, Matrix kind)
			throws ConnectionLostException {
		super(ioio);
		kind_ = kind;
		frame_ = new byte[getFrameSize(kind)];
	}
	
	synchronized public void beginFrame() throws ConnectionLostException {
		try {
			//ioio_.protocol_.rgbLedMatrixWriteFile();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	synchronized public void writeFile(float fps) throws ConnectionLostException {
		try {
			ioio_.protocol_.rgbLedMatrixWriteFile(fps, getShifterLen(kind_));
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}
	
	@Override
	synchronized public void interactive() throws ConnectionLostException {
		try {
			ioio_.protocol_.rgbLedMatrixEnable(getShifterLen(kind_));
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}
	
	@Override
	synchronized public void playFile() throws ConnectionLostException {
		try {
			ioio_.protocol_.rgbLedMatrixEnable(0);
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	synchronized public void frame(short[] rgb565)
			throws ConnectionLostException {
		if (rgb565.length != kind_.width * kind_.height) {
			throw new IllegalArgumentException("Frame length must be "
					+ kind_.width * kind_.height);
		}
		checkState();
		switch (kind_) {
		case ADAFRUIT_32x16:
			convertAdafruit32x16(rgb565, frame_);
			break;

		case SEEEDSTUDIO_32x16:
			convertSeeedStudio32x16(rgb565, frame_);
			break;

		case SEEEDSTUDIO_32x32:
			convertSeeedStudio32x32(rgb565, frame_);
			break;

		case SEEEDSTUDIO_32x32_NEW:
			convertSeeedStudio32x32New(rgb565, frame_);
			break;

		default:
			throw new IllegalStateException("This format is not supported");
		}

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

	private static void convertAdafruit32x16(short[] rgb565, byte[] dest) {
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

					dest[outIndex++] = (byte) (r1 << 5 | g1 << 4 | b1 << 3
							| r2 << 2 | g2 << 1 | b2 << 0);
					++inIndex;
				}
			}
		}
	}

	private static void convertSeeedStudio32x32New(short[] rgb565, byte[] dest) {
		int outIndex = 0;
		for (int subframe = 0; subframe < 3; ++subframe) {
			for (int row = 0; row < 256; row += 32) {
				for (int half = 0; half < 1024; half += 512) {
					for (int col = 0; col < 32; ++col) {
						final int inIndex = col + half + row;
						int pixel1 = ((int) rgb565[inIndex]) & 0xFFFF;
						int pixel2 = ((int) rgb565[inIndex + 256]) & 0xFFFF;

						int r1 = (pixel1 >> (11 + 2 + subframe)) & 1;
						int g1 = (pixel1 >> (5 + 3 + subframe)) & 1;
						int b1 = (pixel1 >> (0 + 2 + subframe)) & 1;

						int r2 = (pixel2 >> (11 + 2 + subframe)) & 1;
						int g2 = (pixel2 >> (5 + 3 + subframe)) & 1;
						int b2 = (pixel2 >> (0 + 2 + subframe)) & 1;

						dest[outIndex++] = (byte) (r1 << 5 | g1 << 4 | b1 << 3
								| r2 << 2 | g2 << 1 | b2 << 0);
					}
				}
			}
		}
	}

	private static void convertSeeedStudio32x32(short[] rgb565, byte[] dest) {
		int outIndex = 0;
		for (int subframe = 0; subframe < 3; ++subframe) {
			int inIndex = 0;
			for (int row = 0; row < 8; ++row) {
				for (int col = 0; col < 32; ++col) {
					int pixel1 = ((int) rgb565[inIndex]) & 0xFFFF;
					int pixel2 = ((int) rgb565[inIndex + 256]) & 0xFFFF;
					int pixel3 = ((int) rgb565[inIndex + 512]) & 0xFFFF;
					int pixel4 = ((int) rgb565[inIndex + 768]) & 0xFFFF;

					int r1 = (pixel1 >> (11 + 2 + subframe)) & 1;
					int g1 = (pixel1 >> (5 + 3 + subframe)) & 1;
					int b1 = (pixel1 >> (0 + 2 + subframe)) & 1;

					int r2 = (pixel2 >> (11 + 2 + subframe)) & 1;
					int g2 = (pixel2 >> (5 + 3 + subframe)) & 1;
					int b2 = (pixel2 >> (0 + 2 + subframe)) & 1;

					int r3 = (pixel3 >> (11 + 2 + subframe)) & 1;
					int g3 = (pixel3 >> (5 + 3 + subframe)) & 1;
					int b3 = (pixel3 >> (0 + 2 + subframe)) & 1;

					int r4 = (pixel4 >> (11 + 2 + subframe)) & 1;
					int g4 = (pixel4 >> (5 + 3 + subframe)) & 1;
					int b4 = (pixel4 >> (0 + 2 + subframe)) & 1;

					dest[outIndex + SEEED_MAP[col]] = (byte) (r1 << 5 | g1 << 4
							| b1 << 3 | r3 << 2 | g3 << 1 | b3 << 0);
					dest[outIndex + SEEED_MAP[col + 32]] = (byte) (r2 << 5
							| g2 << 4 | b2 << 3 | r4 << 2 | g4 << 1 | b4 << 0);
					++inIndex;
				}
				outIndex += 64;
			}
		}
	}

	private static void convertSeeedStudio32x16(short[] rgb565, byte[] dest) {
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

					dest[outIndex + SEEED_MAP[col]] = (byte) (r1 << 5 | g1 << 4 | b1 << 3);
					dest[outIndex + SEEED_MAP[col + 32]] = (byte) (r2 << 5
							| g2 << 4 | b2 << 3);
					++inIndex;
				}
				outIndex += 64;
			}
		}
	}

	private static final int[] SEEED_MAP = { 7, 6, 5, 4, 3, 2, 1, 0, 23, 22,
			21, 20, 19, 18, 17, 16, 39, 38, 37, 36, 35, 34, 33, 32, 55, 54, 53,
			52, 51, 50, 49, 48, 8, 9, 10, 11, 12, 13, 14, 15, 24, 25, 26, 27,
			28, 29, 30, 31, 40, 41, 42, 43, 44, 45, 46, 47, 56, 57, 58, 59, 60,
			61, 62, 63 };

	public static int getShifterLen(Matrix kind) {
		switch (kind) {
		case ADAFRUIT_32x16:
			return 1;

		case SEEEDSTUDIO_32x16:
		case SEEEDSTUDIO_32x32:
		case SEEEDSTUDIO_32x32_NEW:
			return 2;

		default:
			throw new IllegalStateException("Unsupported kind.");
		}
	}

	private static int getFrameSize(Matrix kind) {
		switch (kind) {
		case ADAFRUIT_32x16:
			return 768;

		case SEEEDSTUDIO_32x16:
		case SEEEDSTUDIO_32x32:
		case SEEEDSTUDIO_32x32_NEW:
			return 1536;

		default:
			throw new IllegalStateException("Unsupported kind.");
		}
	}
}
