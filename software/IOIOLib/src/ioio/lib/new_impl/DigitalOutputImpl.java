package ioio.lib.new_impl;

import java.io.IOException;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.InvalidStateException;
import ioio.lib.new_impl.IncomingState.PinMode;

public class DigitalOutputImpl extends AbstractPin implements DigitalOutput {
	DigitalOutputImpl(IOIOImpl ioio, int pin) {
		super(ioio, pin);
	}

	@Override
	synchronized public void opened(PinMode mode) {
		assert(mode == PinMode.DIGITAL_OUT);
		super.opened(mode);
	}

	@Override
	synchronized public void setValue(int value) {
		assert(false);
	}

	@Override
	synchronized public void write(Boolean val) throws ConnectionLostException {
		checkState();
		try {
			ioio_.protocol_.setDigitalOutLevel(pinNum_, val);
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

}
