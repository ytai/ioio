package org.ioio;

// ytai: higher-level utility. should be moved to a ioio.lib.util package?
// i think it is generally a bad practice (confusing the client) to expose
// an API in which some constructs are just wrappers around others. better to have
// exactly one way of doing each thing.
public class ServoDriver {

	PWMOutput pwm;
	
	public ServoDriver(int pin) {
		pwm = new PWMOutput(IOIO.getInstance(), pin);
	}
}
