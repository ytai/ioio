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

import ioio.lib.api.DigitalInput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.InputPinListener;

import java.io.IOException;

class DigitalInputImpl extends AbstractPin implements DigitalInput,
		InputPinListener {
	private boolean value_;
	private boolean valid_ = false;

	DigitalInputImpl(IOIOImpl ioio, ResourceManager.Resource pin)
			throws ConnectionLostException {
		super(ioio, pin);
	}

	@Override
	synchronized public void setValue(int value) {
		// Log.v("DigitalInputImpl", "Pin " + pinNum_ + " value is " + value);
		assert (value == 0 || value == 1);
		value_ = (value == 1);
		if (!valid_) {
			valid_ = true;
		}
		notifyAll();
	}

	@Override
	synchronized public void waitForValue(boolean value)
			throws InterruptedException, ConnectionLostException {
		checkState();
		while (!valid_ || value_ != value) {
			safeWait();
		}
	}

	@Override
	synchronized public void close() {
		checkClose();
		try {
			ioio_.protocol_.setChangeNotify(pin_.id, false);
		} catch (IOException e) {
		}
		super.close();
	}

	@Override
	synchronized public boolean read() throws InterruptedException,
			ConnectionLostException {
		checkState();
		while (!valid_) {
			safeWait();
		}
		return value_;
	}
}
