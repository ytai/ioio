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

import ioio.lib.api.PeriodicDigitalInput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.DataModuleListener;

import java.io.IOException;
import java.util.ArrayDeque;
import java.util.BitSet;
import java.util.Deque;
import android.util.Log;

class PeriodicDigitalInputImpl extends AbstractResource implements PeriodicDigitalInput, DataModuleListener {
	private int[] pins_;
	private BitSet currentValues_;
	private BitSet validValues_;
	private Deque<BitSet> values_ = new ArrayDeque<BitSet>();

	PeriodicDigitalInputImpl(IOIOImpl ioio, int[] pins) throws ConnectionLostException {
		super(ioio);
		pins_ = pins;
		currentValues_ = new BitSet(pins_.length);
		validValues_ = new BitSet(pins_.length);
	}
	
	@Override
	synchronized public void close() {
		super.close();
		try {
			for (int i = 0 ; i < pins_.length ; i++) {
				ioio_.protocol_.registerPeriodicDigitalSampling(pins_[i], 0);
			}
		} catch (IOException e) {
			Log.e("ioio.lib.api.PeriodicDigitalInputImpl", "Exception occurred during close.");
		}
	}

	@Override
	synchronized public BitSet nextFrame() throws InterruptedException,
			ConnectionLostException {
		checkState();
		while (0 == values_.size() && state_ != State.DISCONNECTED ) {
			wait();
		}
		checkState();
		Log.v("PeriodicDigitalInputImpl", "Internal Buffer Size: " + Integer.toString(values_.size() - 1));
		return values_.removeFirst();
	}

	@Override
	public synchronized void disconnected() {
		super.disconnected();
		notifyAll();
	}

	@Override
	synchronized public void dataReceived(byte[] data, int size) {
		assert(size == 2);
		int pin = (int) data[0];
		byte value = data[1];
		int i = 0;
		while (i < pins_.length) {
			if (pins_[i] == pin) {
				break;
			}
			i++;
		}
		assert (i < pins_.length);
		
		if (validValues_.get(i)) {
			// We have an incomplete message. Clear it and start another one.
			validValues_.clear();
			currentValues_.clear();
		}

		validValues_.set(i);
		if (0 != value) {
			currentValues_.set(i);
		}
		
		if (validValues_.cardinality() == validValues_.length()) {
			// Complete frame
			BitSet currentValuesClone = (BitSet) currentValues_.clone();
			values_.addLast(currentValuesClone);
			currentValues_.clear();
			validValues_.clear();
			// TODO Catch overflow here. We should just clear the buffer.
		}
		notifyAll();
	}

	@Override
	public void reportAdditionalBuffer(int bytesToAdd) {
		// Do Nothing
		assert(false);
	}
}
