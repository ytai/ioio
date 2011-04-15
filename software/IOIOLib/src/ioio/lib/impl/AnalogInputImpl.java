/*
 * Copyright 2011. All rights reserved.
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

import ioio.lib.api.AnalogInput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.InputPinListener;
import android.util.Log;

public class AnalogInputImpl extends AbstractPin implements AnalogInput, InputPinListener {
	private int value_;
	private boolean valid_ = false;
	
	AnalogInputImpl(IOIOImpl ioio, int pin) throws ConnectionLostException {
		super(ioio, pin);
	}
	
	@Override
	public float getVoltage() throws InterruptedException, ConnectionLostException {
		return read() * getReference();
	}

	@Override
	public float getReference() {
		return 3.3f;
	}

	@Override
	synchronized public void setValue(int value) {
		Log.v("DigitalInputImpl", "Pin " + pinNum_ + " value is " + value);
		assert(value >= 0 || value < 1024);
		value_ = value; 
		if (!valid_) {
			valid_ = true;
			notifyAll();
		}
	}

	@Override
	synchronized public float read() throws InterruptedException, ConnectionLostException {
		checkState();
		while (!valid_ && state_ != State.DISCONNECTED) {
			wait();
		}
		checkState();
		return (float) value_ / 1023.0f;
	}

	@Override
	public synchronized void disconnected() {
		super.disconnected();
		notifyAll();
	}
}
