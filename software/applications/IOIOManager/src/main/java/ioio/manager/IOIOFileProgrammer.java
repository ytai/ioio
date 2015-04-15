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

import ioio.lib.api.IOIO;
import ioio.lib.api.IcspMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.manager.IOIOFileReader.FormatException;

import java.util.Arrays;

import android.util.Log;

public class IOIOFileProgrammer {
	private static final String TAG = "IOIOFileProgrammer";

	interface ProgressListener {
		public void blockDone();
	}

	public static int countBlocks(IOIOFileReader file) throws FormatException {
		int nblocks = 0;
		while (file.next()) {
			++nblocks;
		}
		return nblocks;
	}

	public static void programIOIOFileBlock(IOIO ioio, IcspMaster icsp, IOIOFileReader file)
			throws ConnectionLostException, InterruptedException,
			FormatException, TimeoutException {
		int[] block = new int[64];
		parseBlock(file.currentBlock(), block);
		Scripts.writeBlock(ioio, icsp, file.currentAddress(), block);
	}

	public static boolean verifyIOIOFileBlock(IOIO ioio, IcspMaster icsp,
			IOIOFileReader file) throws ConnectionLostException,
			InterruptedException, FormatException {
		int[] fileBlock = new int[64];
		int[] actualBlock = new int[64];
		parseBlock(file.currentBlock(), fileBlock);
		Scripts.readBlock(ioio, icsp, file.currentAddress(), 64, actualBlock);
		if (!Arrays.equals(fileBlock, actualBlock)) {
			for (int i = 0; i < 64; ++i) {
				if (fileBlock[i] != actualBlock[i]) {
					Log.w(TAG,
							"Failed verification, address = 0x"
									+ file.currentAddress() + i);
					Log.w(TAG,
							"Expected: 0x" + Integer.toHexString(fileBlock[i]));
					Log.w(TAG,
							"Actual:   0x"
									+ Integer.toHexString(actualBlock[i]));
					break;
				}
			}
			return false;
		}
		return true;
	}

	private static void parseBlock(byte[] src, int[] dest) {
		for (int i = 0; i < 64; ++i) {
			dest[i] = readInst(src, i * 3);
		}
	}

	private static int readInst(byte[] buf, int offset) {
		return (byteToInt(buf[offset]) << 0)
				| (byteToInt(buf[offset + 1]) << 8)
				| (byteToInt(buf[offset + 2]) << 16);
	}

	private static int byteToInt(byte b) {
		return ((int) b) & 0xFF;
	}

}
