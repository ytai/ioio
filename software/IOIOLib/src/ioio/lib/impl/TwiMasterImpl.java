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

import ioio.lib.api.TwiMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.FlowControlledPacketSender.Packet;
import ioio.lib.impl.FlowControlledPacketSender.Sender;
import ioio.lib.impl.IncomingState.DataModuleListener;
import ioio.lib.spi.Log;

import java.io.IOException;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

class TwiMasterImpl extends AbstractResource implements TwiMaster,
		DataModuleListener, Sender {
	class TwiResult implements Result {
		boolean ready_ = false;
		boolean success_;
		final byte[] data_;
		
		public TwiResult(byte[] data) {
			data_ = data;
		}

		@Override
		public synchronized boolean waitReady() throws ConnectionLostException,
				InterruptedException {
			while (!ready_ && state_ != State.DISCONNECTED) {
				wait();
			}
			checkState();
			return success_;
		}
	}

	class OutgoingPacket implements Packet {
		int writeSize_;
		byte[] writeData_;
		boolean tenBitAddr_;
		int addr_;
		int readSize_;

		@Override
		public int getSize() {
			return writeSize_ + 4;
		}

	}

	private final Queue<TwiResult> pendingRequests_ = new ConcurrentLinkedQueue<TwiMasterImpl.TwiResult>();
	private final FlowControlledPacketSender outgoing_ = new FlowControlledPacketSender(
			this);
	private final int twiNum_;

	TwiMasterImpl(IOIOImpl ioio, int twiNum) throws ConnectionLostException {
		super(ioio);
		twiNum_ = twiNum;
	}

	@Override
	synchronized public void disconnected() {
		super.disconnected();
		outgoing_.kill();
		for (TwiResult tr : pendingRequests_) {
			synchronized (tr) {
				tr.notify();
			}
		}
	}

	@Override
	public boolean writeRead(int address, boolean tenBitAddr, byte[] writeData,
			int writeSize, byte[] readData, int readSize)
			throws ConnectionLostException, InterruptedException {
		Result result = writeReadAsync(address, tenBitAddr, writeData,
				writeSize, readData, readSize);
		return result.waitReady();
	}

	@Override
	public Result writeReadAsync(int address, boolean tenBitAddr,
			byte[] writeData, int writeSize, byte[] readData, int readSize)
			throws ConnectionLostException {
		checkState();
		TwiResult result = new TwiResult(readData);

		OutgoingPacket p = new OutgoingPacket();
		p.writeSize_ = writeSize;
		p.writeData_ = writeData;
		p.tenBitAddr_ = tenBitAddr;
		p.readSize_ = readSize;
		p.addr_ = address;

		synchronized (this) {
			pendingRequests_.add(result);
			try {
				outgoing_.write(p);
			} catch (IOException e) {
				Log.e("SpiMasterImpl", "Exception caught", e);
			}
		}
		return result;
	}

	@Override
	public void dataReceived(byte[] data, int size) {
		TwiResult result = pendingRequests_.remove();
		synchronized (result) {
			result.ready_ = true;
			result.success_ = (size != 0xFF);
			if (result.success_ && size > 0) {
				System.arraycopy(data, 0, result.data_, 0, size);
			}
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
		ioio_.closeTwi(twiNum_);
	}

	@Override
	public void send(Packet packet) {
		OutgoingPacket p = (OutgoingPacket) packet;
		try {
			ioio_.protocol_.i2cWriteRead(twiNum_, p.tenBitAddr_, p.addr_,
					p.writeSize_, p.readSize_, p.writeData_);
		} catch (IOException e) {
			Log.e("TwiImpl", "Caught exception", e);
		}
	}
}
