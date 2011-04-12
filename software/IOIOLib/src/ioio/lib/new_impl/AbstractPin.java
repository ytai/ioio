package ioio.lib.new_impl;

import ioio.lib.api.exception.ConnectionLostException;

public abstract class AbstractPin extends AbstractResource {
	protected int pinNum_;
	
	AbstractPin(IOIOImpl ioio, int pinNum) throws ConnectionLostException {
		super(ioio);
		pinNum_ = pinNum;
	}

	synchronized public void close() {
		super.close();
		ioio_.closePin(pinNum_);
	}
}
