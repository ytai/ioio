package ioio.lib.pic;

import ioio.lib.Closeable;

/**
 * Base class for the IOIOPins.
 *
 * TODO(TF): account for the pin usage, restrict illegal use and already claimed pins.
 *
 * @author arshan
 */
public abstract class IOIOPin implements Closeable {

	protected int pin;

	public IOIOPin(int pin) {
		this.pin = pin;
	}
}
