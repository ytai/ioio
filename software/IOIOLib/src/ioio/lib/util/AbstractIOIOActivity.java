package ioio.lib.util;

import ioio.lib.api.IOIO;
import ioio.lib.api.IOIOFactory;
import ioio.lib.api.exception.ConnectionLostException;
import android.app.Activity;
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
 */
public abstract class AbstractIOIOActivity extends Activity {
	private static final String TAG = "AbstractIOIOAdkActivity";
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
		Log.v(TAG, "onPause");
		super.onPause();
		Log.v(TAG, "aborting");
		ioio_thread_.abort();
		Log.v(TAG, "joining");
		try {
			ioio_thread_.join();
		} catch (InterruptedException e) {
		}
		Log.v(TAG, "joined");
	}

	/**
	 * Subclasses should implement this method by returning a concrete subclass
	 * of {@link IOIOThread}.
	 * 
	 * @return An implementation of {@link IOIOThread}
	 */
	protected abstract IOIOThread createIOIOThread();

	/**
	 * Subclasses may override this method for alternate creation modes of the
	 * IOIO instance.
	 * 
	 * @return A IOIO instance.
	 */
	protected IOIO createIOIO() {
		return IOIOFactory.create();
	}

	/**
	 * An abstract class, which facilitates a thread dedicated for IOIO
	 * communication.
	 */
	protected abstract class IOIOThread extends Thread {
		/** Subclasses should use this field for controlling the IOIO. */
		protected IOIO ioio_;
		private boolean abort_ = false;

		/** Not relevant to subclasses. */
		@Override
		public final void run() {
			super.run();
			while (true) {
				try {
					synchronized (this) {
						if (abort_) {
							break;
						}
						Log.v(TAG, "Creating IOIO");
						ioio_ = createIOIO();
						Log.v(TAG, "Created IOIO");
					}
					ioio_.waitForConnect();
					setup();
					while (true) {
						loop();
					}
				} catch (ConnectionLostException e) {
					Log.v(TAG, "Caught ConnectionLostException");
					if (abort_) {
						break;
					}
				} catch (Exception e) {
					Log.e(TAG, "Unexpected exception caught", e);
					ioio_.disconnect();
					break;
				} finally {
					try {
						Log.v(TAG, "Waiting for disconnect");
						ioio_.waitForDisconnect();
					} catch (InterruptedException e) {
					}
					Log.v(TAG, "thread done");
				}
			}
		}

		/**
		 * Subclasses should override this method for performing operations to
		 * be done once as soon as IOIO communication is established. Typically,
		 * this will include opening pins and modules using the openXXX()
		 * methods of the {@link #ioio_} field.
		 */
		protected void setup() throws ConnectionLostException {
		}

		/**
		 * Subclasses should override this method for performing operations to
		 * be done repetitively as long as IOIO communication persists.
		 * Typically, this will be the main logic of the application, processing
		 * inputs and producing outputs.
		 */
		protected void loop() throws ConnectionLostException {
		}

		/** Not relevant to subclasses. */
		public synchronized final void abort() {
			Log.v(TAG, "abort()");
			abort_ = true;
			if (ioio_ != null) {
				ioio_.disconnect();
			}
		}
	}
}
