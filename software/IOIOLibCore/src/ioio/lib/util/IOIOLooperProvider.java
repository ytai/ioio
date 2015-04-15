package ioio.lib.util;

/**
 * An entity that provides {@link IOIOLooper} instances on demand, per
 * connection specifications.
 */
public interface IOIOLooperProvider {

	/**
	 * Subclasses should implement this method by returning an implementation of
	 * {@link IOIOLooper}. The caller provide enough information to uniquely
	 * identify the connection, through the parameters. <code>null</code> may be
	 * returned if the client is not interested to create a thread for this
	 * IOIO. This can be used in order to filter out unwanted connections, for
	 * example if the application is only intended for wireless connection, any
	 * wired connection attempts may be rejected, thus saving resources used for
	 * listening for incoming wired connections.
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
	 * @return An implementation of {@link IOIOLooper}, or <code>null</code> to
	 *         skip.
	 */
	public abstract IOIOLooper createIOIOLooper(String connectionType,
			Object extra);

}