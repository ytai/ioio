package ioio.lib.api;

public class DigitalInputSpec {
	public enum Mode {
	    FLOATING,
	    PULL_UP,
	    PULL_DOWN
	}
	public int pin;
	public Mode mode;
	
	public DigitalInputSpec(int pin, Mode mode) {
		this.pin = pin;
		this.mode = mode;
	}
	
	public DigitalInputSpec(int pin) {
		this(pin, Mode.FLOATING);
	}
}
