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

import ioio.lib.api.SpiMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.FlowControlledPacketSender.Packet;
import ioio.lib.impl.FlowControlledPacketSender.Sender;
import ioio.lib.impl.IncomingState.DataModuleListener;
import ioio.lib.spi.Log;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

class SpiMasterImpl extends AbstractResource implements SpiMaster,
		DataModuleListener, Sender {
	public class SpiResult implements Result {
		boolean ready_;
		final byte[] data_;
		
		SpiResult(byte[] data) {
			data_ = data;
		}

		@Override
		public synchronized void waitReady() throws ConnectionLostException,
				InterruptedException {
			while (!ready_ && state_ != State.DISCONNECTED) {
				wait();
			}
			checkState();
		}
	}

	class OutgoingPacket implements Packet {
		int writeSize_;
		byte[] writeData_;
		int ssPin_;
		int readSize_;
		int totalSize_;

		@Override
		public int getSize() {
			return writeSize_ + 4;
		}
	}

	private final Queue<SpiResult> pendingRequests_ = new ConcurrentLinkedQueue<SpiMasterImpl.SpiResult>();
	private final FlowControlledPacketSender outgoing_ = new FlowControlledPacketSender(
			this);

	private final int spiNum_;
	private final Map<Integer, Integer> ssPinToIndex_;
	private final int[] indexToSsPin_;
	private final int mosiPinNum_;
	private final int misoPinNum_;
	private final int clkPinNum_;

	SpiMasterImpl(IOIOImpl ioio, int spiNum, int mosiPinNum, int misoPinNum,
			int clkPinNum, int[] ssPins) throws ConnectionLostException {
		super(ioio);
		spiNum_ = spiNum;
		mosiPinNum_ = mosiPinNum;
		misoPinNum_ = misoPinNum;
		clkPinNum_ = clkPinNum;
		indexToSsPin_ = ssPins.clone();
		ssPinToIndex_ = new HashMap<Integer, Integer>(ssPins.length);
		for (int i = 0; i < ssPins.length; ++i) {
			ssPinToIndex_.put(ssPins[i], i);
		}
	}

	@Override
	synchronized public void disconnected() {
		super.disconnected();
		outgoing_.kill();
		for (SpiResult tr : pendingRequests_) {
			synchronized (tr) {
				tr.notify();
			}
		}
	}

	@Override
	public void writeRead(int slave, byte[] writeData, int writeSize,
			int totalSize, byte[] readData, int readSize)
			throws ConnectionLostException, InterruptedException {
		Result result = writeReadAsync(slave, writeData, writeSize,
				totalSize, readData, readSize);
		result.waitReady();
	}

	@Override
	public SpiResult writeReadAsync(int slave, byte[] writeData,
			int writeSize, int totalSize, byte[] readData, int readSize)
			throws ConnectionLostException {
		checkState();
		SpiResult result = new SpiResult(readData);

		OutgoingPacket p = new OutgoingPacket();
		p.writeSize_ = writeSize;
		p.writeData_ = writeData;
		p.readSize_ = readSize;
		p.ssPin_ = indexToSsPin_[slave];
		p.totalSize_ = totalSize;

		if (p.readSize_ > 0) {
			synchronized (this) {
				pendingRequests_.add(result);
			}
		} else {
			result.ready_ = true;
		}
		try {
			outgoing_.write(p);
		} catch (IOException e) {
			Log.e("SpiMasterImpl", "Exception caught", e);
		}
		return result;
	}

	@Override
	public void writeRead(byte[] writeData, int writeSize, int totalSize,
			byte[] readData, int readSize) throws ConnectionLostException,
			InterruptedException {
		writeRead(0, writeData, writeSize, totalSize, readData, readSize);
	}

	@Override
	public void dataReceived(byte[] data, int size) {
		SpiResult result = pendingRequests_.remove();
		synchronized (result) {
			result.ready_ = true;
			System.arraycopy(data, 0, result.data_, 0, size);
			result.notify();
		}
	}

	@Override
	public void reportAdditionalBuffer(int bytesRemaining) {
		outgoing_.readyToSend(bytesRemaining);
	}

	@Override
	synchronized public void close() {
		super.close();
		outgoing_.close();
		ioio_.closeSpi(spiNum_);
		ioio_.closePin(mosiPinNum_);
		ioio_.closePin(misoPinNum_);
		ioio_.closePin(clkPinNum_);
		for (int pin : indexToSsPin_) {
			ioio_.closePin(pin);
		}
	}

	@Override
	public void send(Packet packet) {
		OutgoingPacket p = (OutgoingPacket) packet;
		try {
			ioio_.protocol_.spiMasterRequest(spiNum_, p.ssPin_, p.writeData_,
					p.writeSize_, p.totalSize_, p.readSize_);
		} catch (IOException e) {
			Log.e("SpiImpl", "Caught exception", e);
		}
	}

}
