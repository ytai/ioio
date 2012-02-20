package ioio.lib.impl;

import ioio.lib.api.PulseInput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.DataModuleListener;

import java.util.LinkedList;
import java.util.Queue;

class IncapImpl extends AbstractPin implements DataModuleListener,
		PulseInput {
	private static final int MAX_QUEUE_LEN = 32;
	private final PulseMode mode_;
	private final int incapNum_;
	private long lastDuration_;
	private final float timeBase_;
	private final boolean doublePrecision_;
	private boolean valid_ = false;
	// TODO: a fixed-size array would have been much better than a linked list.
	private Queue<Long> pulseQueue_ = new LinkedList<Long>();

	public IncapImpl(IOIOImpl ioio, PulseMode mode, int incapNum, int pin,
			int clockRate, int scale, boolean doublePrecision)
			throws ConnectionLostException {
		super(ioio, pin);
		mode_ = mode;
		incapNum_ = incapNum;
		timeBase_ = 1.0f / (scale * clockRate);
		doublePrecision_ = doublePrecision;
	}

	@Override
	public float getFrequency() throws InterruptedException,
			ConnectionLostException {
		if (mode_ != PulseMode.FREQ && mode_ != PulseMode.FREQ_SCALE_4
				&& mode_ != PulseMode.FREQ_SCALE_16) {
			throw new IllegalStateException(
					"Cannot query frequency when module was not opened in frequency mode.");
		}
		return 1.0f / getDuration();
	}

	@Override
	public synchronized float getDuration() throws InterruptedException,
			ConnectionLostException {
		checkState();
		while (!valid_) {
			wait();
			checkState();
		}
		return timeBase_ * lastDuration_;
	}

	@Override
	public synchronized float waitPulseGetDuration()
			throws InterruptedException, ConnectionLostException {
		if (mode_ != PulseMode.POSITIVE && mode_ != PulseMode.NEGATIVE) {
			throw new IllegalStateException(
					"Cannot wait for pulse when module was not opened in pulse mode.");
		}
		checkState();
		while (pulseQueue_.isEmpty() && state_ == State.OPEN) {
			wait();
		}
		checkState();
		return timeBase_ * pulseQueue_.remove();
	}

	@Override
	public synchronized void dataReceived(byte[] data, int size) {
		lastDuration_ = ByteArrayToLong(data, size);
		if (pulseQueue_.size() == MAX_QUEUE_LEN) {
			pulseQueue_.remove();
		}
		pulseQueue_.add(lastDuration_);
		valid_ = true;
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
		ioio_.closeIncap(incapNum_, doublePrecision_);
		super.close();
	}

	@Override
	public synchronized void disconnected() {
		notifyAll();
		super.disconnected();
	}

}
