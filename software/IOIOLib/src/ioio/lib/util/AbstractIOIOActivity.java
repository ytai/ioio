package ioio.lib.util;

import java.util.Vector;

import ioio.lib.api.IOIO;
import ioio.lib.api.IOIOConnection;
import ioio.lib.api.IOIOFactory;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
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
 * In the intended usage the client should extend this class and implement
 * {@link #createIOIOThread()}, which should return an implementation of the
 * IOIOThread abstract class. In this implementation, the client implements the
 * {@link IOIOThread#setup()} method, which gets called as soon as communication
 * with the IOIO is established, and the {@link IOIOThread#loop()} method, which
 * gets called repetitively as long as the IOIO is connected. Both methods
 * should access the {@link IOIOThread#ioio_} field for controlling the IOIO.
 * 
 */
public abstract class AbstractIOIOActivity extends Activity {
	private IOIOThread ioio_thread_;

	/**
	 * Subclasses should call this method from their own onResume() if
	 * overloaded. It takes care of connecting with the IOIO.
	 */
	@Override
	protected void onResume() {
		super.onResume();
		ioio_thread_ = createIOIOThread();
		ioio_thread_.start();
	}

	/**
	 * Subclasses should call this method from their own onPause() if
	 * overloaded. It takes care of disconnecting from the IOIO.
	 */
	@Override
	protected void onPause() {
		super.onPause();
		ioio_thread_.abort();
		try {
			ioio_thread_.join();
		} catch (InterruptedException e) {
		}
	}

	/**
	 * Subclasses should implement this method by returning a concrete subclass
	 * of {@link IOIOThread}.
	 * 
	 * @return An implementation of {@link IOIOThread}
	 */
	protected abstract IOIOThread createIOIOThread();

	/**
	 * An abstract class, which facilitates a thread dedicated for IOIO
	 * communication.
	 */
	protected abstract class IOIOThread extends Thread {
		private static final String TAG = "AbstractIOIOActivity";
		/** Subclasses should use this field for controlling the IOIO. */
		protected IOIO ioio_;
		private boolean abort_ = false;
		private boolean connected_ = true;

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
						ioio_ = IOIOFactory
								.create(createMultiplexedConnection());
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
		public synchronized final void abort() {
			abort_ = true;
			if (ioio_ != null) {
				ioio_.disconnect();
			}
			if (connected_) {
				interrupt();
			}
		}

		private IOIOConnection createMultiplexedConnection() {
			Vector<IOIOConnection> connections = new Vector<IOIOConnection>();
			try {
				connections
						.add(IOIOFactory
								.createConnectionDynamically("ioio.lib.bluetooth.BluetoothIOIOConnection"));
			} catch (ClassNotFoundException e) {
				Log.w(TAG, "Bluetooth not supported");
			}
			try {
				connections.add(IOIOFactory.createConnectionDynamically(
						"ioio.lib.impl.SocketIOIOConnection", 4545));
			} catch (ClassNotFoundException e) {
			}

			IOIOConnectionMultiplexer multiplexer = new IOIOConnectionMultiplexer(
					connections.toArray(new IOIOConnection[connections.size()]));
			return multiplexer;
		}
	}
}
