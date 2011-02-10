package org.ioio;

public class ServoDriver {

	PWMOutput pwm;
	
	public ServoDriver(int pin) {
		pwm = new PWMOutput(IOIO.getInstance(), pin);
	}
}
