package ioio.lib.new_impl;

import ioio.lib.api.PwmOutput;
import ioio.lib.api.exception.ConnectionLostException;

import java.io.IOException;

public class PwmImpl extends AbstractResource implements PwmOutput {
	private int pwmNum_;
	private int pinNum_;
	private int periodUs_;
	private int period_;

	public PwmImpl(IOIOImpl ioio, int pinNum, int pwmNum, int period, int periodUs) throws ConnectionLostException {
		super(ioio);
		ioio_ = ioio;
		pwmNum_ = pwmNum;
		pinNum_ = pinNum;
		periodUs_ = periodUs;
		period_ = period;
	}

	@Override
	public synchronized void close() {
		super.close();
		ioio_.closePwm(pwmNum_);
		ioio_.closePin(pinNum_);
	}

	@Override
	public void setDutyCycle(float dutyCycle) throws ConnectionLostException {
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
	public void setPulseWidth(int pulseWidthUs) throws ConnectionLostException {
		assert(pulseWidthUs >= 0);
		if (pulseWidthUs > periodUs_) {
			pulseWidthUs = periodUs_;
		}
		setDutyCycle(((float) pulseWidthUs) / periodUs_);
	}
}
