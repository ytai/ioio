package ioio.lib;

public class DigitalOutput extends DigitalIO {

	public DigitalOutput(IOIOApi ioio, int pin) {
			super(ioio, pin);
			super.setOutput(true);
	}

}
