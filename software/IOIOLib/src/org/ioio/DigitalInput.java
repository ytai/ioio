package org.ioio;

// ytai: seems like it will be best if ALL API is interfaces and not classes.
// will make it easy for us to make implementations over different protocols and 
// specifically to introduce mocks for testing purposes.
public class DigitalInput extends DigitalIO {

	// ytai: don't understand the ctor. do you use the IOIOApi to obtain this object
	// (my personal preference) or do you construct the object and pass it the IOIOApi?
	// The latter isn't very clean, since it ruins some of the abstraction and will force us
	// to expose low-level methods on IOIOApi.
	public DigitalInput(IOIOApi ioio, int pin) {
		super(ioio, pin);
	}

	@Override
	public void write(boolean val) throws IOIOException{
		throw new IOIOException("Cannot write value to an input pin.");
	}
}
