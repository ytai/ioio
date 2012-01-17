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
package ioio.lib.api;

import ioio.lib.impl.IOIOImpl;
import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.spi.IOIOConnectionFactory;

import java.util.Collection;
import java.util.LinkedList;
import java.util.NoSuchElementException;

import android.util.Log;

/**
 * Factory class for creating instances of the IOIO interface.
 * <p>
 * This class acts as the single entry-point to the IOIO API. It creates the
 * bootstrapping between a specific implementation of the IOIO interface and any
 * dependencies it might have, such as the underlying connection logic.
 * <p>
 * Typical usage:
 * 
 * <pre>
 * IOIO ioio = IOIOFactory.create();
 * try {
 *   ioio.waitForConnect();
 *   ...
 *   ioio.disconnect();
 * } catch (ConnectionLostException e) {
 * } finally {
 *   ioio.waitForDisconnect();
 * }
 * </pre>
 */
public class IOIOFactory {
	/**
	 * A connection specification. Represents a possible communication channel
	 * to a IOIO.
	 */
	public interface ConnectionSpec {
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
	}

	/**
	 * Create a IOIO instance. This specific implementation creates a IOIO
	 * instance which works with the actual IOIO board connected via a TCP
	 * connection (typically over a wired USB connection).
	 * 
	 * @return The IOIO instance.
	 */
	public static IOIO create() {
		Collection<ConnectionSpec> specs = getConnectionSpecs();
		try {
			return create(specs.iterator().next());
		} catch (NoSuchElementException e) {
			Log.e(TAG, "No connection is available. This shouldn't happen.");
			throw e;
		}
	}

	/**
	 * Create a IOIO instance, based on a connection specification.
	 * 
	 * @param spec
	 *            The connection specification, which was obtained from
	 *            {@link #getConnectionSpecs()}.
	 * @return The IOIO instance.
	 */
	public static IOIO create(ConnectionSpec spec) {
		IOIOConnectionFactory factory = (IOIOConnectionFactory) spec;
		IOIOConnection connection = factory.createConnection();
		return create(connection);
	}

	/**
	 * Get all available connection specifications. This is a list of all
	 * currently available communication channels in which a IOIO may be
	 * available. The client typically passes elements of this collection to
	 * {@link #create(ConnectionSpec)}, possibly after filtering based on the
	 * specification's properties.
	 * 
	 * @return A collection of specifications.
	 */
	public static Collection<ConnectionSpec> getConnectionSpecs() {
		Collection<IOIOConnectionFactory> result = new LinkedList<IOIOConnectionFactory>();
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			bootstrap.getFactories(result);
		}
		return new LinkedList<ConnectionSpec>(result);
	}

	/**
	 * Create a IOIO instance with a user-provided underlying connection class.
	 * This method should be used for establishing a non-standard connection to
	 * the IOIO board.
	 * 
	 * @param connection
	 *            An instance of a IOIO connection.
	 * 
	 * @return The IOIO instance.
	 */
	public static IOIO create(IOIOConnection connection) {
		return new IOIOImpl(connection);
	}
	
	/**
	 * For advanced usage only!
	 * Used for special runtime handling of bootstrap classes.
	 * @return The bootstraps.
	 */
	public static Collection<IOIOConnectionBootstrap> getBootstraps() {
		return bootstraps_;
	}

	private static final String TAG = "IOIOFactory";
	private static Collection<IOIOConnectionBootstrap> bootstraps_ = initializeBootstraps();

	private static Collection<IOIOConnectionBootstrap> initializeBootstraps() {
		final Collection<IOIOConnectionBootstrap> result = new LinkedList<IOIOConnectionBootstrap>();
		String[] classNames = new String[] {
				"ioio.lib.impl.SocketIOIOConnectionBootstrap",
				"ioio.lib.android.accessory.AccessoryConnectionBootstrap",
				"ioio.lib.android.bluetooth.BluetoothIOIOConnectionBootstrap" };
		for (String className : classNames) {
			initializeBootstrap(className, result);
		}
		return result;
	}

	private static void initializeBootstrap(String className,
			Collection<IOIOConnectionBootstrap> result) {
		try {
			Class<? extends IOIOConnectionBootstrap> bootstrapClass = Class
					.forName(className).asSubclass(
							IOIOConnectionBootstrap.class);
			result.add(bootstrapClass.newInstance());
		} catch (ClassNotFoundException e) {
			Log.d(TAG, "Bootstrap class not found: " + className
					+ ". Not adding.");
		} catch (Exception e) {
			Log.w(TAG,
					"Exception caught while attempting to initialize accessory connection factory",
					e);
		}
	}
}
