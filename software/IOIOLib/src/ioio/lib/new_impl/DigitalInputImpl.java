package ioio.lib.new_impl;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.new_impl.IncomingState.PinMode;
import android.util.Log;

public class DigitalInputImpl extends AbstractPin implements DigitalInput {
	boolean value_;
	boolean valid_ = false;
	
	DigitalInputImpl(IOIOImpl ioio, int pin) {
		super(ioio, pin);
	}

	@Override
	synchronized public void opened(PinMode mode) {
		assert(mode == PinMode.DIGITAL_IN);
		super.opened(mode);
	}

	@Override
	synchronized public void setValue(int value) {
		Log.i("DigitalInputImpl", "Pin " + pinNum_ + " value is " + value);
		assert(value == 0 || value == 1);
		value_ = (value == 1); 
		if (!valid_) {
			valid_ = true;
			notifyAll();
		}
	}

	@Override
	synchronized public boolean read() throws InterruptedException, ConnectionLostException {
		checkState();
		while (!valid_) {
			wait();
		}
		return value_;
	}

}
