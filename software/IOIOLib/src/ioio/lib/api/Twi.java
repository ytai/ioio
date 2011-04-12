package ioio.lib.api;

import java.io.Closeable;

public interface Twi extends Closeable {
	public void writeRead(int address, boolean tenBitAddr, byte[] writeData, byte[] readData);
}
