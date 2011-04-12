package ioio.lib.api;

import ioio.lib.api.exception.ConnectionLostException;

import java.io.Closeable;

public interface Twi extends Closeable {
	enum Rate {
		RATE_100KHz,
		RATE_400KHz,
		RATE_1MHz
	}
	
	public boolean writeRead(int address, boolean tenBitAddr, byte[] writeData, int writeSize,
			byte[] readData, int readSize) throws ConnectionLostException, InterruptedException;
}
