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
public class PwmOutput extends IOIOPin {

	/**
	 * percent duty cycle from 0 to 1
	 */
	private float dutyCycle = 0;
	private int period = 0; // period measured in us
	
	public PwmOutput(int pin) {
		super(pin);
	}
	
	public PwmOutput(IOIO ioio, int pin) {
		super(pin);
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
	public void setPeriod(int period) {
		this.period = period;
	}
	
	public int getPeriod() {
		return period;
	}
}
