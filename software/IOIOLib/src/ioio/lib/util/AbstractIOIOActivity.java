package ioio.lib.util;

import ioio.lib.api.IOIO;
import ioio.lib.api.IOIOFactory;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
import ioio.lib.util.IOIOConnectionDiscovery.IOIOConnectionSpec;

import java.util.Collection;
import java.util.LinkedList;

import android.app.Activity;
import android.os.Looper;
import android.util.Log;

/**
 * A convenience class for easy creation of IOIO-based applications.
 * 
 * It is used by creating a concrete Activity in your application, which extends
 * this class. This class then takes care of proper creation and abortion of the
 * IOIO connection and of a dedicated thread for IOIO communication.
 * 
 * In the basic usage the client should extend this class and implement
 * {@link #createIOIOThread()}, which should return an implementation of the
 * {@link IOIOThread} abstract class. In this implementation, the client
 * implements the {@link IOIOThread#setup()} method, which gets called as soon
 * as communication with the IOIO is established, and the {@link IOIOThread
 * #loop()} method, which gets called repetitively as long as the IOIO is
 * connected. Both methods should access the {@link IOIOThread#ioio_} field for
 * controlling the IOIO.
 * 
 * In addition, the {@link IOIOThread#disconnected()} method may be overridden
 * in order to execute logic as soon as a disconnection occurs for whichever
 * reason. The {@link IOIOThread#incompatible()} method may be overridden in
 * order to take action in case where a IOIO whose firmware is incompatible with
 * the IOIOLib version that application is built with.
 * 
 * In a more advanced use case, more than one IOIO is available. In this case, a
 * thread will be created for each IOIO, whose semantics are as defined above.
 * If the client needs to be able to distinguish between them, it is possible to
 * override {@link #createIOIOThread(String, Object[])} instead of
 * {@link #createIOIOThread()}. The first argument provided will contain the
 * connection class name, such as ioio.lib.impl.SocketIOIOConnection for a
 * connection established over a TCP socket (which is used over ADB). The second
 * argument will contain information specific to the connection type. For
 * example, in the case of SocketIOIOConnection, the array will contain an
 * {@link Integer} representing the local port number.
 * 
 */
public abstract class AbstractIOIOActivity extends Activity {
	private static final String TAG = "AbstractIOIOActivity";
	private IOIOConnectionSpec currentSpec_;
	private Collection<IOIOThread> threads_ = new LinkedList<IOIOThread>();

	/**
	 * Subclasses should call this method from their own onResume() if
	 * overloaded. It takes care of connecting with the IOIO.
	 */
	@Override
	protected void onResume() {
		super.onResume();
		createAllThreads();
		startAllThreads();
	}

	/**
	 * Subclasses should call this method from their own onPause() if
	 * overloaded. It takes care of disconnecting from the IOIO.
	 */
	@Override
	protected void onPause() {
		super.onPause();
		abortAllThreads();
		try {
			joinAllThreads();
		} catch (InterruptedException e) {
		}
	}

	/**
	 * Subclasses should implement this method by returning a concrete subclass
	 * of {@link IOIOThread}. <code>null</code> may be returned if the client
	 * is not interested to connect a thread for this IOIO. In multi-IOIO
	 * scenarios, where you want to identify which IOIO the thread is for,
	 * consider using {@link #createIOIOThread()} instead.
	 * 
	 * @return An implementation of {@link IOIOThread}, or <code>null</code> to
	 *         skip.
	 */
	protected IOIOThread createIOIOThread() {
		return null;
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
	 * @param connectionClass
	 *            The fully-qualified name of the connection class used to
	 *            connect to the IOIO.
	 * @param connectionArgs
	 *            A list of arguments passed to the constructor of the
	 *            connection class. Should provide information that enables
	 *            distinguishing between different IOIO instances using the same
	 *            connection class.
	 * 
	 * @return An implementation of {@link IOIOThread}, or <code>null</code> to
	 *         skip.
	 */
	protected IOIOThread createIOIOThread(String connectionClass,
			Object[] connectionArgs) {
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
		private final IOIOConnectionSpec spec_ = currentSpec_;

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
		 * {@link #ioio_} member must not be used from within this method.
		 */
		protected void disconnected() throws InterruptedException {
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
			Looper.prepare();
			while (true) {
				try {
					synchronized (this) {
						if (abort_) {
							break;
						}
						ioio_ = IOIOFactory.create(spec_.className, spec_.args);
					}
					ioio_.waitForConnect();
					connected_ = true;
					setup();
					while (!abort_) {
						loop();
					}
					ioio_.disconnect();
				} catch (ConnectionLostException e) {
					if (abort_) {
						break;
					}
				} catch (InterruptedException e) {
					ioio_.disconnect();
					break;
				} catch (IncompatibilityException e) {
					Log.e(TAG, "Incompatible IOIO firmware", e);
					incompatible();
					// nothing to do - just wait until physical disconnection
					try {
						ioio_.waitForDisconnect();
					} catch (InterruptedException e1) {
						ioio_.disconnect();
					}
				} catch (Exception e) {
					Log.e(TAG, "Unexpected exception caught", e);
					ioio_.disconnect();
					break;
				} finally {
					try {
						if (ioio_ != null) {
							ioio_.waitForDisconnect();
							if (connected_) {
								disconnected();
							}
						}
					} catch (InterruptedException e) {
					}
				}
			}
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
		Collection<IOIOConnectionSpec> specs = getConnectionSpecs();
		for (IOIOConnectionSpec spec : specs) {
			currentSpec_ = spec;
			IOIOThread thread = createIOIOThread(spec.className, spec.args);
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

	private Collection<IOIOConnectionSpec> getConnectionSpecs() {
		Collection<IOIOConnectionSpec> result = new LinkedList<IOIOConnectionSpec>();
		addConnectionSpecs("ioio.lib.util.SocketIOIOConnectionDiscovery",
				result);
		addConnectionSpecs(
				"ioio.lib.bluetooth.BluetoothIOIOConnectionDiscovery", result);
		return result;
	}

	private void addConnectionSpecs(String discoveryClassName,
			Collection<IOIOConnectionSpec> result) {
		try {
			Class<?> cls = Class.forName(discoveryClassName);
			IOIOConnectionDiscovery discovery = (IOIOConnectionDiscovery) cls
					.newInstance();
			discovery.getSpecs(result);
		} catch (ClassNotFoundException e) {
			Log.d(TAG, "Discovery class not found: " + discoveryClassName
					+ ". Not adding.");
		} catch (Exception e) {
			Log.w(TAG,
					"Exception caught while discovering connections - not adding connections of class "
							+ discoveryClassName, e);
		}
	}
}
