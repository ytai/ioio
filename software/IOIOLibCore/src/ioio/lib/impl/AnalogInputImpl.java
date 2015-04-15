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

import ioio.lib.api.AnalogInput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.InputPinListener;

import java.io.IOException;

class AnalogInputImpl extends AbstractPin implements AnalogInput, InputPinListener {
	private int value_;
	private long sampleCount_ = 0;

	short[] buffer_;
	int bufferSize_;
	int bufferCapacity_;
	int bufferReadCursor_;
	int bufferWriteCursor_;
	int bufferOverflowCount_ = 0;

	AnalogInputImpl(IOIOImpl ioio, ResourceManager.Resource pin) throws ConnectionLostException {
		super(ioio, pin);
	}

	@Override
	public float getVoltage() throws InterruptedException, ConnectionLostException {
		return read() * getReference();
	}

	@Override
	public float getVoltageSync() throws InterruptedException, ConnectionLostException {
		return readSync() * getReference();
	}

	@Override
	public float getReference() {
		return 3.3f;
	}

	@Override
	synchronized public void setValue(int value) {
		// Log.v("AnalogInputImpl", "Pin " + pinNum_ + " value is " + value);
		assert (value >= 0 && value < 1024);
		value_ = value;
		++sampleCount_;
		bufferPush((short) value);
		notifyAll();
	}

	@Override
	synchronized public float read() throws InterruptedException, ConnectionLostException {
		checkState();
		// Wait for sample count to be non-zero.
		while (sampleCount_ == 0) {
			safeWait();
		}
		return (float) value_ / 1023.0f;
	}

	@Override
	public synchronized float readSync() throws InterruptedException, ConnectionLostException {
		checkState();
		final long initialSampleCount = sampleCount_;
		// Wait for sample count to increase.
		while (sampleCount_ == initialSampleCount) {
			safeWait();
		}
		return (float) value_ / 1023.0f;
	}

	@Override
	public synchronized void close() {
		checkClose();
		try {
			ioio_.protocol_.setAnalogInSampling(pin_.id, false);
		} catch (IOException e) {
		}
		super.close();
	}

	@Override
	public synchronized void setBuffer(int capacity) throws ConnectionLostException {
		checkState();
		if (capacity <= 0) {
			buffer_ = null;
		} else {
			buffer_ = new short[capacity];
		}
		bufferCapacity_ = capacity;
		bufferSize_ = 0;
		bufferReadCursor_ = 0;
		bufferWriteCursor_ = 0;
		bufferOverflowCount_ = 0;
	}

	@Override
	public float readBuffered() throws InterruptedException, ConnectionLostException {
		checkState();
		return (float) bufferPull() / 1023.0f;
	}

	@Override
	public float getVoltageBuffered() throws InterruptedException, ConnectionLostException {
		return readBuffered() * getReference();
	}

	private void bufferPush(short value) {
		if (buffer_ == null) {
			return;
		}
		if (bufferSize_ == bufferCapacity_) {
			++bufferOverflowCount_;
		} else {
			++bufferSize_;
		}
		buffer_[bufferWriteCursor_++] = value;
		if (bufferWriteCursor_ == bufferCapacity_) {
			bufferWriteCursor_ = 0;
		}
	}

	private synchronized short bufferPull() throws InterruptedException, ConnectionLostException {
		checkState();
		if (buffer_ == null) {
			throw new IllegalStateException(
					"Need to call setBuffer() before reading buffered values.");
		}
		while (bufferSize_ == 0) {
			safeWait();
		}
		short result = buffer_[bufferReadCursor_++];
		if (bufferReadCursor_ == bufferCapacity_) {
			bufferReadCursor_ = 0;
		}
		--bufferSize_;
		return result;
	}

	@Override
	public int getOverflowCount() throws ConnectionLostException {
		return bufferOverflowCount_;
	}

	@Override
	public float getSampleRate() throws ConnectionLostException {
		return 1000.0f;
	}

	@Override
	public int available() throws ConnectionLostException {
		return bufferSize_;
	}
}
