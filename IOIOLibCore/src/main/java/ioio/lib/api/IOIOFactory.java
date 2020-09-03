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
import ioio.lib.spi.IOIOConnectionFactory;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOConnectionRegistry;

import java.util.Collection;
import java.util.NoSuchElementException;

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
	 * Create a IOIO instance. This specific implementation creates a IOIO
	 * instance which works with the actual IOIO board connected via a TCP
	 * connection (typically over a wired USB connection).
	 * 
	 * @return The IOIO instance.
	 */
	public static IOIO create() {
		Collection<IOIOConnectionFactory> factories = IOIOConnectionRegistry
				.getConnectionFactories();
		try {
			return create(factories.iterator().next().createConnection());
		} catch (NoSuchElementException e) {
			Log.e(TAG, "No connection is available. This shouldn't happen.");
			throw e;
		}
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

	private static final String TAG = "IOIOFactory";
}
