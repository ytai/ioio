package ioio.lib.util;


public interface IOIOLooperProvider {

	/**
	 * Subclasses should implement this method by returning a concrete subclass
	 * of {@link IOIOThread}. This overload is useful in multi-IOIO scenarios,
	 * where you want to identify which IOIO the thread is for. The provided
	 * arguments should provide enough information to be unique per connection.
	 * <code>null</code> may be returned if the client is not interested to
	 * connect a thread for this IOIO. This can be used in order to filter out
	 * unwanted connections, for example if the application is only intended for
	 * wireless connection, any wired connection attempts may be rejected, thus
	 * saving resources used for listening for incoming wired connections.
	 * 
	 * @param connectionType
	 *            A unique name of the connection type. Typically, the
	 *            fully-qualified name of the connection class used to connect
	 *            to the IOIO.
	 * @param extra
	 *            A connection-type-specific object with extra information on
	 *            the specific connection. Should provide information that
	 *            enables distinguishing between different IOIO instances using
	 *            the same connection class. For example, a Bluetooth connection
	 *            type, might have the remote IOIO's Bluetooth name as extra.
	 * 
	 * @return An implementation of {@link IOIOThread}, or <code>null</code> to
	 *         skip.
	 */
	public abstract IOIOLooper createIOIOLooper(String connectionType, Object extra);

}