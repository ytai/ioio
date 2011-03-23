package ioio.lib;

import ioio.api.DigitalOutput;
import ioio.api.PwmOutput;
import ioio.api.PeripheralException.ConnectionLostException;
import ioio.api.PeripheralException.InvalidOperationException;
import ioio.api.PeripheralException.InvalidStateException;
import ioio.api.PeripheralException.OutOfResourceException;

import java.io.IOException;

public class IOIOPwmOutput extends IOIOPin implements PwmOutput {

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

	private static final ModuleAllocator PWM_ID_ALLOCATOR = new ModuleAllocator(Constants.NUM_PWMS);
    private DigitalOutput digitalOutput;
    IOIOPwmOutput(IOIOImpl ioio, DigitalOutput pin, int freqHz)
    throws OutOfResourceException, ConnectionLostException, InvalidOperationException {
		super(pin.getPinNumber());
		this.ioio = ioio;
        this.module = PWM_ID_ALLOCATOR.allocateModule();
		if (module == null) {
		    throw new OutOfResourceException("all PWMs have been allocated");
		}
		this.periodUs = 1000000 / freqHz;
		digitalOutput = pin;
		init();
	}

	private void init() throws ConnectionLostException {
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
            throw new InvalidStateException("");
        }
		this.dutyCycle = dutyCycle;
		byte fraction = 0;
		dutyCyclePeriod = (int) (dutyCycle * period);
		if (period != 0 && !scale256) {
		    fraction = (byte)((byte)(dutyCycle * 4 * period) & 0x03);
		}
        ioio.sendPacket(new IOIOPacket(
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
    public void handlePacket(IOIOPacket packet) {
        // do nothing
    }
}
