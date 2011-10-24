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

import java.lang.reflect.Constructor;

import ioio.lib.impl.IOIOImpl;
import ioio.lib.impl.SocketIOIOConnection;

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
	/** The TCP port used for communicating with the IOIO board. */
	public static final int IOIO_PORT = 4545;

	/**
	 * Create a IOIO instance. This specific implementation creates a IOIO
	 * instance which works with the actual IOIO board connected via a TCP
	 * connection (typically over a wired USB connection).
	 *
	 * @return The IOIO instance.
	 */
	public static IOIO create() {
		try {
			return create(SocketIOIOConnection.class.getName(), IOIO_PORT);
		} catch (ClassNotFoundException e) {
			// we shouldn't get here - this class must always exist.
			throw new RuntimeException("Something is very wrong here");
		}
	}

	/**
	 * Create a IOIO instance with a user-provided underlying connection class.
	 * This method should be used for establishing a non-standard connection to
	 * the IOIO board.
	 *
	 * @param connectionClassName
	 *            The name of the connection class. Must have a public default
	 *            constructor.
	 *
	 * @return The IOIO instance.
	 * @throws ClassNotFoundException The given class name was not found.
	 */
	public static IOIO create(String connectionClassName, Object... args) throws ClassNotFoundException {
		IOIOConnection connection = createConnectionDynamically(connectionClassName, args);
		return create(connection);
	}

	public static IOIO create(IOIOConnection connection) {
		return new IOIOImpl(connection);
	}

	public static IOIOConnection createConnectionDynamically(
			String connectionClassName, Object... args)
			throws ClassNotFoundException {
		Class<?> cls;
		cls = Class.forName(connectionClassName);
		Object instance;
		try {
			Class<?>[] argTypes = new Class<?>[args.length];
			for (int i = 0; i < args.length; ++i) {
				argTypes[i] = args[i].getClass();
			}
			Constructor<?> constructor = cls.getConstructor(argTypes);
			instance = constructor.newInstance(args);
		} catch (Exception e) {
			throw new IllegalArgumentException(
					"Provided class does not have a public ctor with the right signature", e);
		}
		if (!(instance instanceof IOIOConnection)) {
			throw new IllegalArgumentException(
					"Provided class does not implement IOIOConnection");
		}
		return (IOIOConnection) instance;
	}
}
