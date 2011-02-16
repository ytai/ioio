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
	private int periodUs = 0; // period measured in us

	private boolean scale256 = true;

	private IOIOImpl ioio;
	private int module;

	private IOIOPacket setPwm;
	private IOIOPacket setPeriod;
	private IOIOPacket setDutyCycle;

	PwmOutput(IOIOImpl ioio, int pin, int module, int periodUs) {
		super(pin);
		this.ioio = ioio;
		this.module = module;
		this.periodUs = periodUs;
		init();
	}

	private void init() {
		setPwm = new IOIOPacket(
				Constants.SET_PWM,
				new byte[]{(byte)pin, (byte)module}
		);

		// TODO: calculate period from periodUs
		int period = periodUs;
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
		setDutyCycle(0); // disable at init if its not by default.
	}

	/**
	 * @param dutyCycle the dutyCycle to set
	 */
	public void setDutyCycle(float dutyCycle) {
		this.dutyCycle = dutyCycle;
		// TODO(TF) -- compute fraction?
		byte fraction = 0;
		int dutyCycleInt = (int) (dutyCycle * 0xffff);
        new IOIOPacket(
		        Constants.SET_DUTYCYCLE,
		        new byte[] {
		            (byte) ((module << 2) | fraction),
		            (byte) (dutyCycleInt & 0xff),
		            (byte) (dutyCycleInt >> 8)
		        });
	}

	/**
	 * @return the dutyCycle
	 */
	public float getDutyCycle() {
		return dutyCycle;
	}
}
