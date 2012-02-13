package ioio.lib.spi;

import ioio.lib.api.IOIOConnection;

public interface IOIOConnectionFactory {
	/**
	 * A unique name of the connection type. Typically a fully-qualified
	 * name of the connection class.
	 */
	public String getType();

	/**
	 * Extra information on the connection. This is specific to the
	 * connection type. For example, for a Bluetooth connection, this is an
	 * array containing the name and the Bluetooth address of the remote
	 * IOIO.
	 */
	public Object getExtra();

	public IOIOConnection createConnection();
}