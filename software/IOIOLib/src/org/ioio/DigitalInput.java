package org.ioio;

public class DigitalInput extends DigitalIO {

	public DigitalInput(IOIOApi ioio, int pin) {
		super(ioio, pin);
	}

	@Override
	public void write(boolean val) throws IOIOException{
		throw new IOIOException("Cannot write value to an input pin.");
	}
}
