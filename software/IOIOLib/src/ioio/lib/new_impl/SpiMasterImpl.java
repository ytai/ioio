package ioio.lib.new_impl;

import ioio.lib.api.SpiMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.new_impl.FlowControlledPacketSender.Packet;
import ioio.lib.new_impl.FlowControlledPacketSender.Sender;
import ioio.lib.new_impl.IncomingState.DataModuleListener;

import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.Queue;

import android.util.Log;

public class SpiMasterImpl extends AbstractResource implements SpiMaster,
		DataModuleListener, Sender {
	class SpiResult {
		boolean ready_ = false;
		public byte[] data_;
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

	Queue<SpiResult> pendingRequests_ = new LinkedList<SpiMasterImpl.SpiResult>();
	FlowControlledPacketSender outgoing_ = new FlowControlledPacketSender(this);

	private final int spiNum_;
	private Map<Integer, Integer> ssPinToIndex_;
	private int[] indexToSsPin_;
	private int mosiPinNum_;
	private int misoPinNum_;
	private int clkPinNum_;

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
		checkState();
		SpiResult result = new SpiResult();
		result.data_ = readData;

		OutgoingPacket p = new OutgoingPacket();
		p.writeSize_ = writeSize;
		p.writeData_ = writeData;
		p.readSize_ = readSize;
		p.ssPin_ = indexToSsPin_[slave];
		p.totalSize_ = totalSize;

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
	}

	@Override
	synchronized public void dataReceived(byte[] data, int size) {
		SpiResult result = pendingRequests_.remove();
		synchronized (result) {
			result.ready_ = true;
			System.arraycopy(data, 0, result.data_, 0, size);
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
		ioio_.closeSpi(spiNum_);
		ioio_.closePin(mosiPinNum_);
		ioio_.closePin(misoPinNum_);
		ioio_.closePin(clkPinNum_);
		for (int pin: indexToSsPin_) {
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
