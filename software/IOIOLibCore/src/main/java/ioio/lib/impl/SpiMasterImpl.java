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
import ioio.lib.impl.ResourceManager.Resource;
import ioio.lib.spi.Log;

import java.io.IOException;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

class SpiMasterImpl extends AbstractResource implements SpiMaster, DataModuleListener, Sender {
	public class SpiResult extends ResourceLifeCycle implements Result {
		private boolean ready_ = false;
		private final byte[] data_;

		SpiResult(byte[] data) {
			data_ = data;
		}

		@Override
		public synchronized void waitReady() throws ConnectionLostException, InterruptedException {
			checkState();
			while (!ready_) {
				safeWait();
			}
		}

		public synchronized void ready() {
			ready_ = true;
			notifyAll();
		}

		public byte[] getData() {
			return data_;
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
	private final FlowControlledPacketSender outgoing_ = new FlowControlledPacketSender(this);

	private final Resource spi_;
	private final Resource[] indexToSsPin_;
	private final Resource mosiPin_;
	private final Resource misoPin_;
	private final Resource clkPin_;

	SpiMasterImpl(IOIOImpl ioio, Resource spi, Resource mosiPin, Resource misoPin, Resource clkPin,
			Resource[] ssPins) throws ConnectionLostException {
		super(ioio);
		spi_ = spi;
		mosiPin_ = mosiPin;
		misoPin_ = misoPin;
		clkPin_ = clkPin;
		indexToSsPin_ = ssPins.clone();
	}

	@Override
	synchronized public void disconnected() {
		outgoing_.kill();
		for (SpiResult result : pendingRequests_) {
			result.disconnected();
		}
		super.disconnected();
	}

	@Override
	public void writeRead(int slave, byte[] writeData, int writeSize, int totalSize,
			byte[] readData, int readSize) throws ConnectionLostException, InterruptedException {
		Result result = writeReadAsync(slave, writeData, writeSize, totalSize, readData, readSize);
		result.waitReady();
	}

	@Override
	public SpiResult writeReadAsync(int slave, byte[] writeData, int writeSize, int totalSize,
			byte[] readData, int readSize) throws ConnectionLostException {
		checkState();
		SpiResult result = new SpiResult(readData);

		OutgoingPacket p = new OutgoingPacket();
		p.writeSize_ = writeSize;
		p.writeData_ = writeData;
		p.readSize_ = readSize;
		p.ssPin_ = indexToSsPin_[slave].id;
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
	public void writeRead(byte[] writeData, int writeSize, int totalSize, byte[] readData,
			int readSize) throws ConnectionLostException, InterruptedException {
		writeRead(0, writeData, writeSize, totalSize, readData, readSize);
	}

	@Override
	public void dataReceived(byte[] data, int size) {
		SpiResult result = pendingRequests_.remove();
		synchronized (result) {
			System.arraycopy(data, 0, result.getData(), 0, size);
			result.ready();
			result.notify();
		}
	}

	@Override
	public void reportAdditionalBuffer(int bytesRemaining) {
		outgoing_.readyToSend(bytesRemaining);
	}

	@Override
	synchronized public void close() {
		checkClose();
		outgoing_.close();
		for (SpiResult result : pendingRequests_) {
			result.close();
		}

		try {
			ioio_.protocol_.spiClose(spi_.id);
			ioio_.resourceManager_.free(spi_);
		} catch (IOException e) {
		}
		ioio_.closePin(mosiPin_);
		ioio_.closePin(misoPin_);
		ioio_.closePin(clkPin_);
		for (Resource pin : indexToSsPin_) {
			ioio_.closePin(pin);
		}
		super.close();
	}

	@Override
	public void send(Packet packet) {
		OutgoingPacket p = (OutgoingPacket) packet;
		try {
			ioio_.protocol_.spiMasterRequest(spi_.id, p.ssPin_, p.writeData_, p.writeSize_,
					p.totalSize_, p.readSize_);
		} catch (IOException e) {
			Log.e("SpiImpl", "Caught exception", e);
		}
	}

}
