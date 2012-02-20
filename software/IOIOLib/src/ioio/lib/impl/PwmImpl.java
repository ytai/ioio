/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *  
 * 
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */
package ioio.lib.impl;

import ioio.lib.api.PwmOutput;
import ioio.lib.api.exception.ConnectionLostException;

import java.io.IOException;

class PwmImpl extends AbstractResource implements PwmOutput {
	private final int pwmNum_;
	private final int pinNum_;
	private final float baseUs_;
	private final int period_;

	public PwmImpl(IOIOImpl ioio, int pinNum, int pwmNum, int period,
			float baseUs) throws ConnectionLostException {
		super(ioio);
		pwmNum_ = pwmNum;
		pinNum_ = pinNum;
		baseUs_ = baseUs;
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
		assert (dutyCycle <= 1 && dutyCycle >= 0);
		setPulseWidthInClocks(period_ * dutyCycle);
	}

	@Override
	public void setPulseWidth(int pulseWidthUs) throws ConnectionLostException {
		setPulseWidth((float) pulseWidthUs);
	}

	@Override
	public void setPulseWidth(float pulseWidthUs)
			throws ConnectionLostException {
		assert (pulseWidthUs >= 0);
		float p = pulseWidthUs / baseUs_;
		setPulseWidthInClocks(p);
	}

	synchronized private void setPulseWidthInClocks(float p)
			throws ConnectionLostException {
		checkState();
		if (p > period_) {
			p = period_;
		}
		int pw;
		int fraction;
		p -= 1; // period parameter is one less than the actual period length
				// yes, there is 0 and then 2 (no 1) - this is not a bug, that
				// is how the hardware PWM module works.
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
}
