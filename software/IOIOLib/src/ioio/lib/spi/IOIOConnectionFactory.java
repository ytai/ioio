package ioio.lib.spi;

import ioio.lib.api.IOIOConnection;
import ioio.lib.api.IOIOFactory.ConnectionSpec;

public interface IOIOConnectionFactory extends ConnectionSpec {
	public IOIOConnection createConnection();
}