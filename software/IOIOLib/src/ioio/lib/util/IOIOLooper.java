package ioio.lib.util;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;

public interface IOIOLooper {
	/**
	 * Subclasses should override this method for performing operations to
	 * be done once as soon as IOIO communication is established. Typically,
	 * this will include opening pins and modules using the openXXX()
	 * methods of the {@link #ioio_} field.
	 */
	public abstract void setup(IOIO ioio) throws ConnectionLostException,
			InterruptedException;

	/**
	 * Subclasses should override this method for performing operations to
	 * be done repetitively as long as IOIO communication persists.
	 * Typically, this will be the main logic of the application, processing
	 * inputs and producing outputs.
	 */
	public abstract void loop() throws ConnectionLostException,
			InterruptedException;

	/**
	 * Subclasses should override this method for performing operations to
	 * be done once as soon as IOIO communication is lost or closed.
	 * Typically, this will include GUI changes corresponding to the change.
	 * This method will only be called if setup() has been called. The
	 * {@link #ioio_} member must not be used from within this method. This
	 * method should not block for long, since it may cause an ANR.
	 */
	public abstract void disconnected();

	/**
	 * Subclasses should override this method for performing operations to
	 * be done if an incompatible IOIO firmware is detected. The
	 * {@link #ioio_} member must not be used from within this method. This
	 * method will only be called once, until a compatible IOIO is connected
	 * (i.e. {@link #setup()} gets called).
	 */
	public abstract void incompatible();

}