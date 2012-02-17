package ioio.lib.util;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;

/**
 * A convenience implementation of {@link IOIOLooper}.
 * 
 * This base class provides no-op implementations for all methods and provides
 * the {@link #ioio_} field for subclasses.
 * 
 */
public class BaseIOIOLooper implements IOIOLooper {
	protected IOIO ioio_;

	@Override
	public final void setup(IOIO ioio) throws ConnectionLostException,
			InterruptedException {
		ioio_ = ioio;
		setup();
	}

	/**
	 * This method will be called as soon as connection to the IOIO has been
	 * established. Typically, this will include opening pins and modules using
	 * the openXXX() methods of the {@link #ioio_} field.
	 * 
	 * @throws ConnectionLostException
	 *             The connection to the IOIO has been lost.
	 * @throws InterruptedException
	 *             The thread has been interrupted.
	 */
	protected void setup() throws ConnectionLostException, InterruptedException {
	}

	@Override
	public void loop() throws ConnectionLostException, InterruptedException {
		Thread.sleep(20);
	}

	@Override
	public void disconnected() {
	}

	@Override
	public void incompatible() {
	}
}