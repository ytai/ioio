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

package ioio.lib.util;

import ioio.lib.api.IOIO;
import ioio.lib.api.IOIOFactory;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.spi.IOIOConnectionFactory;
import ioio.lib.spi.Log;
import ioio.lib.util.android.ContextWrapperDependent;

import java.util.Collection;
import java.util.LinkedList;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

/**
 * A convenience class for easy creation of IOIO-based applications.
 * 
 * It is used by creating a concrete Activity in your application, which extends
 * this class. This class then takes care of proper creation and abortion of the
 * IOIO connection and of a dedicated thread for IOIO communication.
 * 
 * In the basic usage the client should extend this class and implement
 * {@link #createIOIOThread()}, which should return an implementation of the
 * {@link AbstractIOIOActivity.IOIOThread} abstract class. In this
 * implementation, the client implements the
 * {@link AbstractIOIOActivity.IOIOThread#setup()} method, which gets called as
 * soon as communication with the IOIO is established, and the
 * {@link AbstractIOIOActivity.IOIOThread#loop()} method, which gets called
 * repetitively as long as the IOIO is connected. Both methods should access the
 * {@link AbstractIOIOActivity.IOIOThread#ioio_} field for controlling the IOIO.
 * 
 * In addition, the {@link AbstractIOIOActivity.IOIOThread#disconnected()}
 * method may be overridden in order to execute logic as soon as a disconnection
 * occurs for whichever reason. The
 * {@link AbstractIOIOActivity.IOIOThread#incompatible()} method may be
 * overridden in order to take action in case where a IOIO whose firmware is
 * incompatible with the IOIOLib version that application is built with.
 * 
 * In a more advanced use case, more than one IOIO is available. In this case, a
 * thread will be created for each IOIO, whose semantics are as defined above.
 * If the client needs to be able to distinguish between them, it is possible to
 * override {@link #createIOIOThread(String, Object)} instead of
 * {@link #createIOIOThread()}. The first argument provided will contain the
 * connection class name, such as ioio.lib.impl.SocketIOIOConnection for a
 * connection established over a TCP socket (which is used over ADB). The second
 * argument will contain information specific to the connection type. For
 * example, in the case of SocketIOIOConnection, the second argument will
 * contain an {@link Integer} representing the local port number.
 * 
 * @deprecated Please use {@link ioio.lib.util.android.IOIOActivity} instead.
 */
public abstract class AbstractIOIOActivity extends Activity {
	private static final String TAG = "AbstractIOIOActivity";

	static {
		IOIOConnectionRegistry
				.addBootstraps(new String[] {
						"ioio.lib.android.accessory.AccessoryConnectionBootstrap",
						"ioio.lib.android.bluetooth.BluetoothIOIOConnectionBootstrap" });
	}

	private Collection<IOIOThread> threads_ = new LinkedList<IOIOThread>();
	private Collection<IOIOConnectionBootstrap> bootstraps_ = IOIOConnectionRegistry
			.getBootstraps();
	private IOIOConnectionFactory currentConnectionFactory_;

	/**
	 * Subclasses should call this method from their own onCreate() if
	 * overloaded. It takes care of connecting with the IOIO.
	 */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			if (bootstrap instanceof ContextWrapperDependent) {
				((ContextWrapperDependent) bootstrap).onCreate(this);
			}
		}
	}

	/**
	 * Subclasses should call this method from their own onDestroy() if
	 * overloaded. It takes care of connecting with the IOIO.
	 */
	@Override
	protected void onDestroy() {
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			if (bootstrap instanceof ContextWrapperDependent) {
				((ContextWrapperDependent) bootstrap).onDestroy();
			}
		}
		super.onDestroy();
	}

	/**
	 * Subclasses should call this method from their own onStart() if
	 * overloaded. It takes care of connecting with the IOIO.
	 */
	@Override
	protected void onStart() {
		super.onStart();
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			if (bootstrap instanceof ContextWrapperDependent) {
				((ContextWrapperDependent) bootstrap).open();
			}
		}
		createAllThreads();
		startAllThreads();
	}

	/**
	 * Subclasses should call this method from their own onStop() if overloaded.
	 * It takes care of disconnecting from the IOIO.
	 */
	@Override
	protected void onStop() {
		abortAllThreads();
		try {
			joinAllThreads();
		} catch (InterruptedException e) {
		}
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			if (bootstrap instanceof ContextWrapperDependent) {
				((ContextWrapperDependent) bootstrap).close();
			}
		}
		super.onStop();
	}

	@Override
	protected void onNewIntent(Intent intent) {
		super.onNewIntent(intent);
		if ((intent.getFlags() & Intent.FLAG_ACTIVITY_NEW_TASK) != 0) {
			for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
				if (bootstrap instanceof ContextWrapperDependent) {
					((ContextWrapperDependent) bootstrap).open();
				}
			}
		}
	}

	/**
	 * Subclasses must either implement this method or its other overload by
	 * returning a concrete subclass of {@link IOIOThread}. <code>null</code>
	 * may be returned if the client is not interested to create a thread for
	 * this IOIO. In multi-IOIO scenarios, where you want to identify which IOIO
	 * the thread is for, consider using {@link #createIOIOThread()} instead.
	 * 
	 * @return An implementation of {@link IOIOThread}, or <code>null</code> to
	 *         skip.
	 */
	protected IOIOThread createIOIOThread() {
		throw new RuntimeException(
				"Client must override on of the createIOIOThread overloads!");
	}

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
	protected IOIOThread createIOIOThread(String connectionType, Object extra) {
		return createIOIOThread();
	}

	/**
	 * An abstract class, which facilitates a thread dedicated for communication
	 * with a single physical IOIO device.
	 */
	protected abstract class IOIOThread extends Thread {
		/** Subclasses should use this field for controlling the IOIO. */
		protected IOIO ioio_;
		private boolean abort_ = false;
		private boolean connected_ = true;
		private final IOIOConnectionFactory connectionFactory_ = currentConnectionFactory_;

		/**
		 * Subclasses should override this method for performing operations to
		 * be done once as soon as IOIO communication is established. Typically,
		 * this will include opening pins and modules using the openXXX()
		 * methods of the {@link #ioio_} field.
		 */
		protected void setup() throws ConnectionLostException,
				InterruptedException {
		}

		/**
		 * Subclasses should override this method for performing operations to
		 * be done repetitively as long as IOIO communication persists.
		 * Typically, this will be the main logic of the application, processing
		 * inputs and producing outputs.
		 */
		protected void loop() throws ConnectionLostException,
				InterruptedException {
			sleep(100000);
		}

		/**
		 * Subclasses should override this method for performing operations to
		 * be done once as soon as IOIO communication is lost or closed.
		 * Typically, this will include GUI changes corresponding to the change.
		 * This method will only be called if setup() has been called. The
		 * {@link #ioio_} member must not be used from within this method. This
		 * method should not block for long, since it may cause an ANR.
		 */
		protected void disconnected() {
		}

		/**
		 * Subclasses should override this method for performing operations to
		 * be done if an incompatible IOIO firmware is detected. The
		 * {@link #ioio_} member must not be used from within this method. This
		 * method will only be called once, until a compatible IOIO is connected
		 * (i.e. {@link #setup()} gets called).
		 */
		protected void incompatible() {
		}

		/** Not relevant to subclasses. */
		@Override
		public final void run() {
			super.run();
			while (!abort_) {
				try {
					synchronized (this) {
						if (abort_) {
							break;
						}
						ioio_ = IOIOFactory.create(connectionFactory_
								.createConnection());
					}
				} catch (Exception e) {
					Log.e(TAG, "Failed to create IOIO, aborting IOIOThread!");
					return;
				}
				// if we got here, we have a ioio_!
				try {
					ioio_.waitForConnect();
					connected_ = true;
					setup();
					while (!abort_) {
						loop();
					}
				} catch (ConnectionLostException e) {
				} catch (InterruptedException e) {
					ioio_.disconnect();
				} catch (IncompatibilityException e) {
					Log.e(TAG, "Incompatible IOIO firmware", e);
					incompatible();
					// nothing to do - just wait until physical disconnection
				} catch (Exception e) {
					Log.e(TAG, "Unexpected exception caught", e);
					ioio_.disconnect();
					break;
				} finally {
					try {
						ioio_.waitForDisconnect();
					} catch (InterruptedException e1) {
					}
					synchronized (this) {
						ioio_ = null;
					}
					if (connected_) {
						disconnected();
						connected_ = false;
					}
				}
			}
			Log.d(TAG, "IOIOThread is exiting");
		}

		/** Not relevant to subclasses. */
		public synchronized final void abort() {
			abort_ = true;
			if (ioio_ != null) {
				ioio_.disconnect();
			}
			if (connected_) {
				interrupt();
			}
		}
	}

	private void abortAllThreads() {
		for (IOIOThread thread : threads_) {
			thread.abort();
		}
	}

	private void joinAllThreads() throws InterruptedException {
		for (IOIOThread thread : threads_) {
			thread.join();
		}
	}

	private void createAllThreads() {
		threads_.clear();
		Collection<IOIOConnectionFactory> factories = IOIOConnectionRegistry
				.getConnectionFactories();
		for (IOIOConnectionFactory factory : factories) {
			currentConnectionFactory_ = factory;
			IOIOThread thread = createIOIOThread(factory.getType(),
					factory.getExtra());
			if (thread != null) {
				threads_.add(thread);
			}
		}
	}

	private void startAllThreads() {
		for (IOIOThread thread : threads_) {
			thread.start();
		}
	}

}
