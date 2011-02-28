package ioio.lib.pic;

import java.io.IOException;

import ioio.lib.IoioException.ConnectionLostException;
import ioio.lib.IoioException.InvalidOperationException;
import ioio.lib.IoioException.InvalidStateException;
import ioio.lib.IoioException.OutOfResourceException;
import ioio.lib.Output;
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
public class PwmOutputImpl extends IoioPin implements PwmOutput {

	/**
	 * percent duty cycle from 0 to 1
	 */
	private float dutyCycle = 0;
	private int periodUs = 0; // period measured in us
	private int period = 0;   // period as sent to IOIO
	private int dutyCyclePeriod = 0; // period as set as duty cycle

	private boolean scale256 = true;

	private IoioImpl ioio;
	private Integer module;

	private IoioPacket setPwm;
	private IoioPacket setPeriod;

	private static final ModuleAllocator PWM_ID_ALLOCATOR = new ModuleAllocator(Constants.NUM_PWMS);
    private Output<Boolean> digitalOutput;
    PwmOutputImpl(IoioImpl ioio, int pin, int freqHz, boolean enableOpenDrain)
    throws OutOfResourceException, ConnectionLostException, InvalidOperationException {
		super(pin);
		this.ioio = ioio;
        this.module = PWM_ID_ALLOCATOR.allocateModule();
		if (module == null) {
		    throw new OutOfResourceException("all PWMs have been allocated");
		}
		this.periodUs = 1000000 / freqHz;
		digitalOutput = ioio.openDigitalOutput(pin, enableOpenDrain);
		init();
	}

	private void init() throws ConnectionLostException {
		setPwm = new IoioPacket(
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

		setPeriod = new IoioPacket(
				Constants.SET_PERIOD,
				new byte[]{
						(byte)(module << 1 | (scale256?1:0)),
						(byte)(period & 0xFF),
						(byte)(period >> 8)
					}
		);

		ioio.sendPacket(setPwm);
		// Must always set period before duty cycle.
		ioio.sendPacket(setPeriod);
		// setDutyCycle(0); // disable at init if its not by default.  This is done automatially by IOIO
	}

	/**
	 * @param dutyCycle the dutyCycle to set
	 * @throws ConnectionLostException
	 * @throws InvalidStateException
	 */
	@Override
    public void setDutyCycle(float dutyCycle) throws ConnectionLostException, InvalidStateException {
        if (isInvalid()) {
            throw Constants.INVALID_STATE_EXCEPTION;
        }
		this.dutyCycle = dutyCycle;
		byte fraction = 0;
		dutyCyclePeriod = (int) (dutyCycle * period);
		if (period != 0 && !scale256) {
		    fraction = (byte)((byte)(dutyCycle * 4 * period) & 0x03);
		}
        ioio.sendPacket(new IoioPacket(
		        Constants.SET_DUTYCYCLE,
		        new byte[] {
		            (byte) ((module << 2) | fraction),
		            (byte) (dutyCyclePeriod & 0xff),
		            (byte) (dutyCyclePeriod >> 8)
		        }));
	}

    @Override
    public void close() {
        try {
            digitalOutput.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        PWM_ID_ALLOCATOR.releaseModule(module);
    }

    @Override
    public void setPulseWidth(int pulseWidthUs) throws ConnectionLostException, InvalidStateException {
        float dutyCycle = ((float) pulseWidthUs) / periodUs;
        setDutyCycle(dutyCycle);
    }

    @Override
    public void handlePacket(IoioPacket packet) {
        // do nothing
    }
}
