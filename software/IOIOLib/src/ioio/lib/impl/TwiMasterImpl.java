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

import ioio.lib.api.TwiMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.FlowControlledPacketSender.Packet;
import ioio.lib.impl.FlowControlledPacketSender.Sender;
import ioio.lib.impl.IncomingState.DataModuleListener;

import java.io.IOException;
import java.util.LinkedList;
import java.util.Queue;

import android.util.Log;

public class TwiMasterImpl extends AbstractResource implements TwiMaster, DataModuleListener, Sender {
	class TwiResult {
		boolean ready_ = false;
		public boolean success_;
		public byte[] data_;
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
	
	Queue<TwiResult> pendingRequests_ = new LinkedList<TwiMasterImpl.TwiResult>();
	FlowControlledPacketSender outgoing_ = new FlowControlledPacketSender(this); 
	
	private final int twiNum_;
	
	TwiMasterImpl(IOIOImpl ioio, int twiNum) throws ConnectionLostException {
		super(ioio);
		twiNum_ = twiNum;
	}

	@Override
	synchronized public void disconnected() {
		super.disconnected();
		outgoing_.kill();
		for (TwiResult tr: pendingRequests_) {
			synchronized (tr) {
				tr.notify();
			}
		}
	}

	@Override
	public boolean writeRead(int address, boolean tenBitAddr,
			byte[] writeData, int writeSize, byte[] readData, int readSize)
			throws ConnectionLostException, InterruptedException {
		checkState();
		TwiResult result = new TwiResult();
		result.data_ = readData;
		
		OutgoingPacket p = new OutgoingPacket();
		p.writeSize_ = writeSize;
		p.writeData_ = writeData;
		p.tenBitAddr_ = tenBitAddr;
		p.readSize_ = readSize;
		p.addr_ = address;

		synchronized (this) {
			pendingRequests_.add(result);
			outgoing_.write(p);
		}

		synchronized (result) {
			while (!result.ready_ && state_ != State.DISCONNECTED) {
				result.wait();
			}
			checkState();
		}
		return result.success_;
	}

	@Override
	synchronized public void dataReceived(byte[] data, int size) {
		TwiResult result = pendingRequests_.remove();
		synchronized (result) {
			result.ready_ = true;
			result.success_ = (size != 0xFF);
			if (result.success_) {
				System.arraycopy(data, 0, result.data_, 0, size);
			}
			result.notify();
		}
	}

	@Override
	synchronized public void reportBufferRemaining(int bytesRemaining) {
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
