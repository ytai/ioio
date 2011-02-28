package ioio.lib.pic;

import java.io.Closeable;

/**
 * Base class for the IOIOPins.
 * 
 * @author arshan
 */
public abstract class IoioPin implements IoioPacketListener, Closeable {

    private boolean isInvalid;
	protected int pin;

	public IoioPin(int pin) {
		this.pin = pin;
	}

	@Override
	public void disconnectNotification() {
	    isInvalid = true;
	}

	public boolean isInvalid() {
        return isInvalid;
    }
}
