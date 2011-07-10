package ioio.manager;

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

	public static void programIOIOFileBlock(IcspMaster icsp, IOIOFileReader file)
			throws ConnectionLostException, InterruptedException,
			FormatException, TimeoutException {
		int[] block = new int[64];
		parseBlock(file.currentBlock(), block);
		Scripts.writeBlock(icsp, file.currentAddress(), block);
	}

	public static boolean verifyIOIOFileBlock(IcspMaster icsp,
			IOIOFileReader file) throws ConnectionLostException,
			InterruptedException, FormatException {
		int[] fileBlock = new int[64];
		int[] actualBlock = new int[64];
		parseBlock(file.currentBlock(), fileBlock);
		Scripts.readBlock(icsp, file.currentAddress(), 64, actualBlock);
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
