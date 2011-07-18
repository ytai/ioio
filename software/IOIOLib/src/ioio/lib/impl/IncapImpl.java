package ioio.lib.impl;

import ioio.lib.api.PulseDurationInput;
import ioio.lib.api.PulseFrequencyInput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.DataModuleListener;

import java.util.LinkedList;
import java.util.Queue;

public class IncapImpl extends AbstractPin implements DataModuleListener,
		PulseDurationInput, PulseFrequencyInput {
	private static final int MAX_QUEUE_LEN = 32;
	private final int incapNum_;
	private int lastDuration_;
	private final float timeBase_;
	// TODO: a fixed-size array would have been much better than a linked list.
	private Queue<Integer> pulseQueue_ = new LinkedList<Integer>();

	public IncapImpl(IOIOImpl ioio, int incapNum, int pin, int clockRate,
			int scale) throws ConnectionLostException {
		super(ioio, pin);
		incapNum_ = incapNum;
		timeBase_ = ((float) scale) / clockRate;
	}

	@Override
	public float getFrequency() throws InterruptedException,
			ConnectionLostException {
		return 1.0f / getDuration();
	}

	@Override
	public synchronized float getDuration() throws InterruptedException,
			ConnectionLostException {
		checkState();
		return timeBase_ * lastDuration_;
	}

	@Override
	public synchronized float waitPulseGetDuration()
			throws InterruptedException, ConnectionLostException {
		checkState();
		while (pulseQueue_.isEmpty() && state_ == State.OPEN) {
			wait();
		}
		checkState();
		return timeBase_ * pulseQueue_.remove();
	}

	@Override
	public synchronized void dataReceived(byte[] data, int size) {
		assert size == 2;
		lastDuration_ = data[0] | (((int) data[1]) << 8);
		if (pulseQueue_.size() == MAX_QUEUE_LEN) {
			pulseQueue_.remove();
		}
		pulseQueue_.add(lastDuration_);
	}

	@Override
	public synchronized void reportAdditionalBuffer(int bytesToAdd) {
	}

	@Override
	public synchronized void close() {
		ioio_.closeIncap(incapNum_);
		super.close();
	}

	@Override
	public synchronized void disconnected() {
		notifyAll();
		super.disconnected();
	}

}
