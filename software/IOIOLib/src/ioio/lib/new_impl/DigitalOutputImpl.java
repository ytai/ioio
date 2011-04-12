package ioio.lib.new_impl;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.exception.ConnectionLostException;

import java.io.IOException;

public class DigitalOutputImpl extends AbstractPin implements DigitalOutput {
	DigitalOutputImpl(IOIOImpl ioio, int pin) throws ConnectionLostException {
		super(ioio, pin);
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
