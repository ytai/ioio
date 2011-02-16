package ioio.lib.pic;

import ioio.lib.IOIOException.ConnectionLostException;
import ioio.lib.IOIOException.OutOfResourceException;
import ioio.lib.PwmOutput;

/**
 * Do we want to represent this as the abstract float% or
 * actually expose the bits of accuracy
 *
 * TODO(TF): TBI
 */
// ytai: for "analog" output, float is nicer. for servo control, absolute time in
// ms may be more intuitive, or maybe even a different float that represents the angle.
// why not expose both ways, commenting that they are different accessors to the same value?
public class PwmOutputImpl extends IOIOPin implements PwmOutput {

	/**
	 * percent duty cycle from 0 to 1
	 */
	private float dutyCycle = 0;
	private int periodUs = 0; // period measured in us
	private int period = 0;   // period as sent to IOIO
	private int dutyCyclePeriod = 0; // period as set as duty cycle

	private boolean scale256 = true;

	private IOIOImpl ioio;
	private Integer module;

	private IOIOPacket setPwm;
	private IOIOPacket setPeriod;
	private IOIOPacket setDutyCycle;
	private static final ModuleAllocator PWM_ID_ALLOCATOR = new ModuleAllocator(Constants.NUM_PWMS);
    private DigitalOutput digitalOutput;

	PwmOutputImpl(IOIOImpl ioio, int pin, int periodUs, boolean enableOpenDrain) throws OutOfResourceException {
		super(pin);
		this.ioio = ioio;
		this.module = PWM_ID_ALLOCATOR.allocateModule();
		if (module == null) {
		    throw new OutOfResourceException("all PWMs have been allocated");
		}
		this.periodUs = periodUs;
		digitalOutput = ioio.openDigitalOutput(pin, enableOpenDrain);
		init();
	}

	private void init() {
		setPwm = new IOIOPacket(
				Constants.SET_PWM,
				new byte[]{(byte)pin, (byte)(int)module}
		);

		if (periodUs == 0) {
		   period = 0;
		   scale256 = false;
		} else if (periodUs <= 4096) {
		  scale256 = false;
		  period = (periodUs * 16) - 1;
		} else {
		  scale256 = true;
		  period = (periodUs / 16) - 1;
		}

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
		// setDutyCycle(0); // disable at init if its not by default.  This is done automatially by IOIO
	}

	/**
	 * @param dutyCycle the dutyCycle to set
	 */
	@Override
    public void setDutyCycle(float dutyCycle) {
		this.dutyCycle = dutyCycle;
		byte fraction = 0;
		dutyCyclePeriod = (int) (dutyCycle * period);
		if (period != 0 && !scale256) {
		    fraction = (byte)((byte)(dutyCycle * 4 * period) & 0x03);
		}
        ioio.queuePacket(new IOIOPacket(
		        Constants.SET_DUTYCYCLE,
		        new byte[] {
		            (byte) ((module << 2) | fraction),
		            (byte) (dutyCyclePeriod & 0xff),
		            (byte) (dutyCyclePeriod >> 8)
		        }));
	}

	/**
	 * @return the dutyCycle
	 */
	public float getDutyCycle() {
		return dutyCycle;
	}

    @Override
    public void close() {
        digitalOutput.close();
        PWM_ID_ALLOCATOR.releaseModule(module);
        // TODO(TF): Implement this
    }

    @Override
    public void setPulseWidth(int pulseWidthUs) throws ConnectionLostException {
        float dutyCycle = ((float) pulseWidthUs) / periodUs;
        setDutyCycle(dutyCycle);
    }
}
