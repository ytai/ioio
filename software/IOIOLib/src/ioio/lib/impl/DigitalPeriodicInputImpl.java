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

import ioio.lib.api.DigitalPeriodicInput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.DataModuleListener;

import java.io.IOException;
import java.util.ArrayDeque;
import java.util.BitSet;
import java.util.Deque;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.TreeMap;
import java.util.concurrent.ConcurrentLinkedQueue;

import android.util.Log;
import android.util.Pair;

class DigitalPeriodicInputImpl extends AbstractResource implements DigitalPeriodicInput {
	static public DigitalPeriodicInputMasterImpl master_ = null;
	private int freq_scale_ = 0;
	private List<Integer> pins_;
	private Deque<BitSet> values_ = new ArrayDeque<BitSet>();

	DigitalPeriodicInputImpl(IOIOImpl ioio, List<Integer> pins, int freq_scale) throws ConnectionLostException {
		super(ioio);
		if ( null ==  master_) {
			master_ = new DigitalPeriodicInputMasterImpl(ioio);
		}
		master_.addInput(this);
		pins_ = pins;
		freq_scale_ = freq_scale;
	}
	
	synchronized public int getFreqScale() {
		return freq_scale_;
	}
	
	synchronized public List<Integer> getPins() {
		return pins_;
	}

	synchronized public void addValues(BitSet values) {
		// Log.v("DigitalInputImpl", "Pin " + pinNum_ + " value is " + value);
		assert (values.size() == pins_.size());
		values_.addLast(values);
		// TODO Catch overflow here. We should just clear the buffer.
		notifyAll();
	}

	@Override
	synchronized public void close() {
		super.close();
		try {
			master_.removeInput(this);
			Iterator<Integer> pin_it = pins_.iterator();
			while (pin_it.hasNext()) {
				int pin = (Integer) pin_it.next();
				ioio_.protocol_.registerPeriodicDigitalSampling(pin, 0);
			}
			freq_scale_ = 0;
		} catch (IOException e) {
			Log.e("ioio.lib.api.DigitalPeriodicInputImpl", "Exception occurred during close.");
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
		Log.v("DigitalPeriodicInputImpl", "Internal Buffer Size: " + Integer.toString(values_.size() - 1));
		return values_.removeFirst();
	}

	@Override
	public synchronized void disconnected() {
		super.disconnected();
		notifyAll();
	}
}

class DigitalPeriodicInputMasterImpl extends AbstractResource implements DataModuleListener {
	private Queue<DigitalPeriodicInputImpl> openDPIs_ = new ConcurrentLinkedQueue<DigitalPeriodicInputImpl>();
	
	public DigitalPeriodicInputMasterImpl(IOIOImpl ioio)
			throws ConnectionLostException {
		super(ioio);
		// TODO Auto-generated constructor stub
	}

	public void addInput(DigitalPeriodicInputImpl input) {
		openDPIs_.add(input);
	}
	
	public void removeInput(DigitalPeriodicInputImpl input) {
		// TODO Important... How do I remove just this one? Maybe I need a map
	}
	
	@Override
	public void dataReceived(byte[] data, int size) {
		if (size <= 0) { return; }
		byte frame_num = data[0];
		
		DigitalPeriodicInputImpl[] reportingInputs                 = new DigitalPeriodicInputImpl[openDPIs_.size()];
		Map<Integer, Pair<Integer, Integer> > pinCorrespondences   = new TreeMap<Integer, Pair<Integer, Integer> >();
		BitSet[]                   values                          = new BitSet[openDPIs_.size()];
		int numReportingInputs = 0;
		for (DigitalPeriodicInputImpl input : openDPIs_) {
			
			if (input.getFreqScale() == 0 || input.getFreqScale() > 240) { continue; }

			if ( frame_num % input.getFreqScale() == 0 && input.getPins().size() > 0) {
				// There are bits that belong to this input.
				
				reportingInputs[numReportingInputs] = input;
				values[numReportingInputs] = new BitSet(input.getPins().size());
			
				for (int i = 0 ; i < input.getPins().size() ; ++i) {
					pinCorrespondences.put(input.getPins().get(i), new Pair<Integer, Integer>(numReportingInputs, i));
				}
				numReportingInputs++;
			}
		}

		int currentByte = 1;
		byte mask = 1;
		for (Integer pin : pinCorrespondences.keySet()) {
			BitSet bitSet = values[pinCorrespondences.get(pin).first];
			if (0 == mask) {
				mask = 1;
				currentByte++;
			}
			// Set the next bit (size) to the bit in question.
			bitSet.set(pinCorrespondences.get(pin).second, 0 != (data[currentByte] & mask));
			mask <<= 1;
		}
		
		for (int i = 0 ; i < numReportingInputs ; i++) {
			reportingInputs[i].addValues(values[i]);
		}
	}

	@Override
	public void reportAdditionalBuffer(int bytesToAdd) {
		// Never gets called.
	}
	
}

