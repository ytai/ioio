package org.ioio;

/**
 * Do we want to represent this as the abstract float% or 
 * actually expose the bits of accuracy
 * @author arshan
 *
 */
public class PWMOutput extends DigitalOutput {

	/**
	 * percent duty cycle from 0 to 1
	 */
	private float dutyCycle = 0;
	private int period = 0; // period measured in us
	
	public PWMOutput(int pin) {
		super(IOIO.getInstance(), pin);
	}
	
	public PWMOutput(IOIO ioio, int pin) {
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

	public void setPeriod(int period) {
		this.period = period;
	}
	
	public int getPeriod() {
		return period;
	}
}
