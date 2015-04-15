package ioio.lib.util;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;

/**
 * A handler implementing interaction with a single IOIO over a single
 * connection period. The interface utilizes a basic workflow for working with a
 * IOIO instance: as soon as a connection is established, {@link #setup(IOIO)}
 * will be called. Then, the {@link #loop()} method will be called repeatedly as
 * long as the connection is alive. Last, the {@link #disconnected()} method
 * will be called upon losing the connection (as result of physical
 * disconnection, closing the application, etc). In case a IOIO with an
 * incompatible firmware is encountered, {@link #incompatible()} will be called
 * instead of {@link #setup(IOIO)}, and the IOIO instance is entirely useless,
 * until eventually {@link #disconnected()} gets called.
 *
 */
public interface IOIOLooper {
	/**
	 * Subclasses should override this method for performing operations to be
	 * done once as soon as IOIO communication is established.
	 */
	public abstract void setup(IOIO ioio) throws ConnectionLostException,
			InterruptedException;

	/**
	 * Subclasses should override this method for performing operations to be
	 * done repetitively as long as IOIO communication persists. Typically, this
	 * will be the main logic of the application, processing inputs and
	 * producing outputs.
	 */
	public abstract void loop() throws ConnectionLostException,
			InterruptedException;

	/**
	 * Subclasses should override this method for performing operations to be
	 * done once as soon as IOIO communication is lost or closed. Typically,
	 * this will include GUI changes corresponding to the change. This method
	 * will only be called if setup() has been called. The ioio argument passed
	 * to {@link #setup(IOIO)} must not be used from within this method - it is
	 * invalid. This method should not block for long, since it may cause an
	 * ANR.
	 */
	public abstract void disconnected();

	/**
	 * Subclasses should override this method for performing operations to be
	 * done if an incompatible IOIO firmware is detected. The ioio argument
	 * passed to {@link #setup(IOIO)} must not be used from within this method -
	 * it is invalid. This method will only be called once, until a compatible
	 * IOIO is connected (i.e. {@link #setup(IOIO)} gets called).
	 *
	 * @deprecated Please use {@link #incompatible(IOIO)} instead.
	 */
	public abstract void incompatible();


	/**
	 * Subclasses should override this method for performing operations to be
	 * done if an incompatible IOIO firmware is detected. The ioio argument can
	 * only be used for querying the version strings and disconnecting. It is
	 * otherwise unusable.
	 */
	public abstract void incompatible(IOIO ioio);

}