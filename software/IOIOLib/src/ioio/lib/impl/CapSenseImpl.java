/*
 * Copyright 2013 Ytai Ben-Tsvi. All rights reserved.
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

import ioio.lib.api.CapSense;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.InputPinListener;

import java.io.IOException;

class CapSenseImpl extends AbstractPin implements CapSense, InputPinListener {
	private static final float CHARGE = 89.1f; // In [pC] units.
	private static final float SAMPLE_PERIOD_MS = 16.f;
	private float value_;
	private boolean valid_ = false;
	private float coef_;

	CapSenseImpl(IOIOImpl ioio, int pin, float filterCoef)
			throws ConnectionLostException {
		super(ioio, pin);
		setFilterCoef(filterCoef);
	}

	@Override
	synchronized public void setValue(int value) {
		assert (value >= 0 && value < 1024);
		final float fVal = value / 1023.f;
		if (!valid_) {
			value_ = fVal;
			valid_ = true;
		} else {
			value_ = fVal * (1.f - coef_) + value_ * coef_;
		}
		notifyAll();
	}

	@Override
	synchronized public float read() throws InterruptedException,
			ConnectionLostException {
		while (!valid_ && state_ == State.OPEN) {
			wait();
		}
		checkState();
		final float voltage = 3.3f * value_;
		return CHARGE / voltage;
	}

	@Override
	public synchronized void waitOver(float threshold) throws ConnectionLostException,
			InterruptedException {
		while (read() <= threshold) {
			wait();
		}
	}

	@Override
	public synchronized void waitUnder(float threshold)
			throws ConnectionLostException, InterruptedException {
		while (read() >= threshold) {
			wait();
		}
	}

	@Override
	public synchronized void disconnected() {
		super.disconnected();
		notifyAll();
	}

	@Override
	public synchronized void close() {
		try {
			ioio_.protocol_.setAnalogInSampling(pinNum_, false);
		} catch (IOException e) {
		}
		super.close();
	}

	@Override
	public synchronized void setFilterCoef(float t)
			throws ConnectionLostException {
		// We define t as the time it takes for an impulse response to fade to
		// 0.1.
		coef_ = (float) Math.pow(0.1, SAMPLE_PERIOD_MS / t);
	}
}
