package ioio.lib.util;

import ioio.lib.api.IOIO;
import ioio.lib.api.IOIOFactory;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.spi.IOIOConnectionFactory;

import java.util.Collection;
import java.util.LinkedList;

import android.util.Log;

/**
 * A helper class for creating different kinds of IOIO based applications.
 * 
 * This class implements a common life-cycle for applications interacting with
 * IOIO devices.
 * <p>
 * When the application starts, call {@link #start()}, which will in turn
 * attempt to create a thread for each possible IOIO connection channel. Each
 * thread will have a respective {@link IOIOLooper}, which the client provides,
 * through which the client gets context for working with the IOIO.
 * <p>
 * When the application exits, call {@link #stop()}, which will disconnect all
 * open connections and will abort and join all the threads.
 * 
 */
public class IOIOApplicationHelper {
	/**
	 * An abstract class, which facilitates a thread dedicated for communication
	 * with a single physical IOIO device.
	 */
	static private class IOIOThread extends Thread {
		protected IOIO ioio_;
		private boolean abort_ = false;
		private boolean connected_ = true;
		private final IOIOLooper looper_;
		private final IOIOConnectionFactory connectionFactory_;

		IOIOThread(IOIOLooper looper, IOIOConnectionFactory factory) {
			looper_ = looper;
			connectionFactory_ = factory;
		}

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
					looper_.setup(ioio_);
					while (!abort_ && ioio_.getState() == IOIO.State.CONNECTED) {
						looper_.loop();
					}
				} catch (ConnectionLostException e) {
				} catch (InterruptedException e) {
					ioio_.disconnect();
				} catch (IncompatibilityException e) {
					Log.e(TAG, "Incompatible IOIO firmware", e);
					looper_.incompatible();
					// nothing to do - just wait until physical
					// disconnection
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
						looper_.disconnected();
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

	protected static final String TAG = "IOIOAndroidApplicationHelper";
	protected final IOIOLooperProvider looperProvider_;
	private Collection<IOIOThread> threads_ = new LinkedList<IOIOThread>();
	protected Collection<IOIOConnectionBootstrap> bootstraps_ = IOIOConnectionRegistry
			.getBootstraps();

	public IOIOApplicationHelper(IOIOLooperProvider provider) {
		looperProvider_ = provider;
	}

	protected void abortAllThreads() {
		for (IOIOThread thread : threads_) {
			thread.abort();
		}
	}

	protected void joinAllThreads() throws InterruptedException {
		for (IOIOThread thread : threads_) {
			thread.join();
		}
	}

	protected void createAllThreads() {
		threads_.clear();
		Collection<IOIOConnectionFactory> factories = IOIOConnectionRegistry
				.getConnectionFactories();
		for (IOIOConnectionFactory factory : factories) {
			IOIOLooper looper = looperProvider_.createIOIOLooper(
					factory.getType(), factory.getExtra());
			if (looper != null) {
				threads_.add(new IOIOThread(looper, factory));
			}
		}
	}

	protected void startAllThreads() {
		for (IOIOThread thread : threads_) {
			thread.start();
		}
	}

	public void start() {
		createAllThreads();
		startAllThreads();
	}

	public void stop() {
		abortAllThreads();
		try {
			joinAllThreads();
		} catch (InterruptedException e) {
		}
	}

}