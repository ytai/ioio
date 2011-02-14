package ioio.lib;

/**
 * Do we want to represent this as the abstract float% or 
 * actually expose the bits of accuracy
 * 
 * TODO(TF): TBI
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

	private boolean scale256 = false;
	
	private IOIOImplPic24f ioio;
	private int module;
	
	private IOIOPacket setPwm;
	private IOIOPacket setPeriod;
	private IOIOPacket setDutyCycle;
	
	PwmOutput(IOIOImplPic24f ioio, int pin, int module, int period) {
		super(pin);
		this.ioio = ioio;
		this.module = module;
		this.period = period;
		init();
	}	
	
	private void init() {
		setPwm = new IOIOPacket(
				Constants.SET_PWM,
				new byte[]{(byte)pin, (byte)module}
		);
	
		setPeriod = new IOIOPacket(
				Constants.SET_PERIOD,
				new byte[]{
						(byte)(module << 1 | (scale256?1:0)),
						(byte)(period & 0xFF),
						(byte)(period >> 8)
					}
		);
		
		ioio.queuePacket(setPwm);
		// Must always set period before duty cycle.
		ioio.queuePacket(setPeriod);	
		// setDutyCycle(0); // disable at init if its not by default.
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
}
