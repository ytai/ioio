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

import ioio.lib.api.PulseInput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.DataModuleListener;

import java.io.IOException;
import java.util.LinkedList;
import java.util.Queue;

class IncapImpl extends AbstractPin implements DataModuleListener, PulseInput {
	private static final int MAX_QUEUE_LEN = 32;
	private final PulseMode mode_;
	private final ResourceManager.Resource incap_;
	private long lastDuration_;
	private final float timeBase_;
	private final boolean doublePrecision_;
	private long sampleCount_ = 0;

	// TODO: a fixed-size array would have been much better than a linked list.
	private Queue<Long> pulseQueue_ = new LinkedList<Long>();

	public IncapImpl(IOIOImpl ioio, PulseMode mode, ResourceManager.Resource incap,
			ResourceManager.Resource pin, int clockRate, int scale, boolean doublePrecision)
			throws ConnectionLostException {
		super(ioio, pin);
		mode_ = mode;
		incap_ = incap;
		timeBase_ = 1.0f / (scale * clockRate);
		doublePrecision_ = doublePrecision;
	}

	@Override
	public float getFrequency() throws InterruptedException, ConnectionLostException {
		if (mode_ != PulseMode.FREQ && mode_ != PulseMode.FREQ_SCALE_4
				&& mode_ != PulseMode.FREQ_SCALE_16) {
			throw new IllegalStateException(
					"Cannot query frequency when module was not opened in frequency mode.");
		}
		return 1.0f / getDuration();
	}

	@Override
	public float getFrequencySync() throws InterruptedException, ConnectionLostException {
		if (mode_ != PulseMode.FREQ && mode_ != PulseMode.FREQ_SCALE_4
				&& mode_ != PulseMode.FREQ_SCALE_16) {
			throw new IllegalStateException(
					"Cannot query frequency when module was not opened in frequency mode.");
		}
		return 1.0f / getDurationSync();
	}

	@Override
	public synchronized float getDuration() throws InterruptedException, ConnectionLostException {
		checkState();
		// Wait for sample count to be non-zero.
		while (sampleCount_ == 0) {
			safeWait();
		}
		return timeBase_ * lastDuration_;
	}

	@Override
	public synchronized float getDurationSync() throws InterruptedException, ConnectionLostException {
		checkState();
		final long initialSampleCount = sampleCount_;
		// Wait for sample count to increase.
		while (sampleCount_ == initialSampleCount) {
			safeWait();
		}
		return timeBase_ * lastDuration_;
	}

	@Override
	public synchronized float waitPulseGetDuration() throws InterruptedException,
			ConnectionLostException {
		return getDurationBuffered();
	}

	@Override
	public synchronized float getDurationBuffered() throws InterruptedException,
			ConnectionLostException {
		if (mode_ != PulseMode.POSITIVE && mode_ != PulseMode.NEGATIVE) {
			throw new IllegalStateException(
					"Cannot wait for pulse when module was not opened in pulse mode.");
		}
		checkState();
		while (pulseQueue_.isEmpty()) {
			safeWait();
		}
		return timeBase_ * pulseQueue_.remove();
	}

	@Override
	public synchronized void dataReceived(byte[] data, int size) {
		lastDuration_ = ByteArrayToLong(data, size);
		if (pulseQueue_.size() == MAX_QUEUE_LEN) {
			pulseQueue_.remove();
		}
		pulseQueue_.add(lastDuration_);
		++sampleCount_;
		notifyAll();
	}

	private static long ByteArrayToLong(byte[] data, int size) {
		long result = 0;
		int i = size;
		while (i-- > 0) {
			result <<= 8;
			result |= ((int) data[i]) & 0xFF;
		}
		if (result == 0) {
			result = 1 << (size * 8);
		}
		return result;
	}

	@Override
	public synchronized void reportAdditionalBuffer(int bytesToAdd) {
	}

	@Override
	public synchronized void close() {
		checkClose();
		try {
			ioio_.protocol_.incapClose(incap_.id, doublePrecision_);
			ioio_.resourceManager_.free(incap_);
		} catch (IOException e) {
		}
		super.close();
	}
}
