package ioio.lib.new_impl;

import java.io.IOException;

import ioio.lib.api.PwmOutput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.InvalidStateException;
import ioio.lib.new_impl.IncomingState.PinMode;

public class PwmImpl extends AbstractPin implements PwmOutput {
	private int pwmNum_;
	private int periodUs_;
	private int period_;

	public PwmImpl(IOIOImpl ioio, int pinNum, int pwmNum, int period, int periodUs) {
		super(ioio, pinNum);
		ioio_ = ioio;
		pwmNum_ = pwmNum;
		periodUs_ = periodUs;
		period_ = period;
	}

	@Override
	synchronized public void opened(PinMode mode) {
		assert(mode == PinMode.PWM);
		super.opened(mode);
	}

	@Override
	public synchronized void close() {
		super.close();
		ioio_.closePwm(pwmNum_);
	}

	@Override
	synchronized public void setValue(int value) {
		assert(false);
	}

	@Override
	public void setDutyCycle(float dutyCycle) throws ConnectionLostException,
			InvalidStateException {
		checkState();
		assert(dutyCycle <= 1 && dutyCycle >= 0);
		int pw, fraction;
		float p = (period_ + 1) * dutyCycle - 1;
		if (p < 1) {
			pw = 0;
			fraction = 0;
		} else {
			pw = (int) p;
			fraction = ((int) p * 4) & 0x03;
		}
		try {
			ioio_.protocol_.setPwmDutyCycle(pwmNum_, pw, fraction);
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	public void setPulseWidth(int pulseWidthUs) throws ConnectionLostException,
			InvalidStateException {
		assert(pulseWidthUs >= 0);
		if (pulseWidthUs > periodUs_) {
			pulseWidthUs = periodUs_;
		}
		setDutyCycle(((float) pulseWidthUs) / periodUs_);
	}

}
