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
package ioio.manager;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

public class IOIOFileReader {
	int address_;
	byte[] buf_ = new byte[192];
	private InputStream in_;
	private final File file_;

	public class FormatException extends Exception {
		private static final long serialVersionUID = 7944061537571462938L;

		public FormatException(String msg) {
			super(msg);
		}

		public FormatException(Exception e) {
			super(e);
		}
	}


	public IOIOFileReader(File file) throws FormatException, FileNotFoundException {
		file_ = file;
		rewind();
	}

	public void rewind() throws FileNotFoundException, FormatException {
		in_ = new FileInputStream(file_);
		mustRead(8);
		if (buf_[0] != 'I' || buf_[1] != 'O' || buf_[2] != 'I'
				|| buf_[3] != 'O') {
			throw new FormatException("Bad header, expected 'IOIO'");
		}
		int ver = readInt(4);
		if (ver != 1) {
			throw new FormatException(
					"Unsupported file format version, expected 1, got: " + ver);
		}
	}

	public boolean next() throws FormatException {
		try {
			int r = read(4);
			if (r == 0) {
				return false;
			}
			if (r != 4) {
				throw new FormatException("Unexpected EOF");
			}
		} catch (IOException e) {
			throw new FormatException(e);
		}
		address_ = readInt(0);
		mustRead(192);
		return true;
	}

	public byte[] currentBlock() {
		return buf_;
	}

	public int currentAddress() {
		return address_;
	}

	private int read(int size) throws IOException {
		int offset = 0;
		while (offset < size) {
			int r = in_.read(buf_, offset, size - offset);
			if (r == -1) {
				return offset;
			}
			offset += r;
		}
		return offset;
	}

	private void mustRead(int size) throws FormatException {
		try {
			if (read(size) != size) {
				throw new FormatException("Unexpected EOF");
			}
		} catch (IOException e) {
			throw new FormatException(e);
		}
	}

	private int readInt(int offset) {
		return (byteToInt(buf_[offset]) << 0)
				| (byteToInt(buf_[offset + 1]) << 8)
				| (byteToInt(buf_[offset + 2]) << 16)
				| (byteToInt(buf_[offset + 3]) << 24);
	}
	
	private static int byteToInt(byte b) {
		return ((int) b) & 0xFF;
	}
}
