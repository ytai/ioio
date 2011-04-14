package ioio.lib.new_impl;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.new_impl.IncomingState.InputPinListener;
import android.util.Log;

public class DigitalInputImpl extends AbstractPin implements DigitalInput, InputPinListener {
	boolean value_;
	boolean valid_ = false;
	
	DigitalInputImpl(IOIOImpl ioio, int pin) throws ConnectionLostException {
		super(ioio, pin);
	}

	@Override
	synchronized public void setValue(int value) {
		Log.v("DigitalInputImpl", "Pin " + pinNum_ + " value is " + value);
		assert(value == 0 || value == 1);
		value_ = (value == 1); 
		if (!valid_) {
			valid_ = true;
		}
		notifyAll();
	}
	
	synchronized public void waitForValue(boolean value) throws InterruptedException, ConnectionLostException {
		checkState();
		while ((!valid_ || value_ != value) && state_ != State.DISCONNECTED) {
			wait();
		}
		checkState();
	}

	@Override
	synchronized public boolean read() throws InterruptedException, ConnectionLostException {
		checkState();
		while (!valid_ && state_ != State.DISCONNECTED) {
			wait();
		}
		checkState();
		return value_;
	}

	@Override
	public synchronized void disconnected() {
		super.disconnected();
		notifyAll();
	}
}
