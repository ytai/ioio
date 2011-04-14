package ioio.lib.new_impl;

import ioio.lib.api.Twi;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.new_impl.FlowControlledPacketSender.Packet;
import ioio.lib.new_impl.FlowControlledPacketSender.Sender;
import ioio.lib.new_impl.IncomingState.DataModuleListener;

import java.io.IOException;
import java.util.LinkedList;
import java.util.Queue;

import android.util.Log;

public class TwiImpl extends AbstractResource implements Twi, DataModuleListener, Sender {
	class TwiResult {
		boolean ready_ = false;
		public int size_;
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
	
	Queue<TwiResult> pendingRequests_ = new LinkedList<TwiImpl.TwiResult>();
	FlowControlledPacketSender outgoing_ = new FlowControlledPacketSender(this); 
	
	private final int twiNum_;
	
	TwiImpl(IOIOImpl ioio, int twiNum) throws ConnectionLostException {
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
		return result.size_ != 0xFF;
	}

	@Override
	synchronized public void dataReceived(byte[] data, int size) {
		TwiResult result = pendingRequests_.remove();
		synchronized (result) {
			result.ready_ = true;
			if (size != 0xFF) {
				System.arraycopy(data, 0, result.data_, 0, size);
			}
			result.size_ = size;
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
