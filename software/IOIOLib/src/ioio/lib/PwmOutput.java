package ioio.lib;

/**
 * Do we want to represent this as the abstract float% or 
 * actually expose the bits of accuracy
 * @author arshan
 *
 */
// ytai: for "analog" output, float is nicer. for servo control, absolute time in
// ms may be more intuitive, or maybe even a different float that represents the angle.
// why not expose both ways, commenting that they are different accessors to the same value?
public class PwmOutput extends DigitalOutput {

	/**
	 * percent duty cycle from 0 to 1
	 */
	private float dutyCycle = 0;
	private int period = 0; // period measured in us
	
	public PwmOutput(int pin) {
		super(IOIO.getInstance(), pin);
	}
	
	public PwmOutput(IOIO ioio, int pin) {
		super(ioio, pin);
	}
	
	
	/**
	 * @param dutyCycle the dutyCycle to set
	 */
	public void setDutyCycle(float dutyCycle) {
		this.dutyCycle = dutyCycle;
	}

	/**
	 * @return the dutyCycle
	 */
	public float getDutyCycle() {
		return dutyCycle;
	}

	// ytai: should be final and set upon construction i think.
	// resetting the period has the side effect of setting the DC to 0
	// which may have undesired behavior.
	// ouch, too bad. 
	// ytai: i could change this behavior, only that is isn't clear what to change it to.
	//       when changing the frequency, what should the new duty cycle be? should the
	//       relative on-time be preserved? should the absolute on-time be preserved? what if the
	//       absolute on-time is now greater than the entire period?
	//       in all use-cases i had in mind, you initially set PWM frequency to something
	//       that suits the peripheral you have conected there, and never touch it. i might
	//       be overlooking something.
	public void setPeriod(int period) {
		this.period = period;
	}
	
	public int getPeriod() {
		return period;
	}
}
