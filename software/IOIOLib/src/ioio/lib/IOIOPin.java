package ioio.lib;

import java.io.Closeable;

/**
 * Base class for the IOIOPins.
 * 
 * @author arshan
 */
public abstract class IOIOPin implements IOIOPacketListener, Closeable {

    private boolean isInvalid;
	protected int pin;

	public IOIOPin(int pin) {
		this.pin = pin;
	}

	@Override
	public void disconnectNotification() {
	    isInvalid = true;
	}

	public boolean isInvalid() {
        return isInvalid;
    }
	
	public int getPinNumber() {
	    return pin;
	}
	
}
