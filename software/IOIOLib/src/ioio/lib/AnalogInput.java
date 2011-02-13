package ioio.lib;

// TBI
public class AnalogInput extends IOIOPin {

	IOIO ioio;
	
	public AnalogInput(IOIO ioio, int pin) {
		super(pin);
		this.ioio = ioio;
	}

	// TBI
	public float read() {
		return 3.3f;
	}
	
}
