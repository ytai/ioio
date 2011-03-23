package ioio.lib;

import java.io.IOException;
import java.io.InputStream;

/**
 * Utility class to handle byte operations.
 *
 * @author arshan
 * @author birmiwal
 */
public class Bytes {

    public static int asInt(byte[] bytes, int len, int offset) {
    	int result = 0;
    	for (int x = 0; x < len; x++) {
    		result |= (0xFF & bytes[x + offset]) << (x * 8);
    	}
    	return result;
    }

    // Utility to allocate the array and return it with bytes filled.
    public static byte[] readBytes(InputStream in, int size) throws IOException {
    	byte[] bytes = new byte[size];
    	if (Bytes.readFully(in, bytes)) {
    		return bytes;
    	}
    	throw new IOException("stream is broke");
    }

    public static int readByte(InputStream in) throws IOException{
        return in.read();
    }

    public static boolean readFully(InputStream in, byte[] buffer, int offset) throws IOException {
    	int val = 0;
    	int current = offset;
    	while (current < buffer.length) {
    		val = in.read(buffer, current, buffer.length-current);
    		if (val == -1) {
    			return false;
    		}
    		current += val;
    	}
    	return true;
    }

    /**
     * poll the input stream until the bytes of the buffer are filled.
     * @param buffer
     */
    public static boolean readFully(InputStream in, byte[] buffer) throws IOException {
    	return readFully(in, buffer, 0);
    }
}
