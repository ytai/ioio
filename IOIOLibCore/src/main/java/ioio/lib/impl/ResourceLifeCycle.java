package ioio.lib.impl;

import ioio.lib.api.Closeable;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.DisconnectListener;

public class ResourceLifeCycle implements Closeable, DisconnectListener {

	protected enum State {
		OPEN, CLOSED, DISCONNECTED
	}

	private State state_ = State.OPEN;

	public ResourceLifeCycle() {
		super();
	}

	@Override
	public synchronized void disconnected() {
		if (state_ != State.CLOSED) {
			state_ = State.DISCONNECTED;
			notifyAll();
		}
	}

	@Override
	public synchronized void close() {
		checkClose();
		state_ = State.CLOSED;
		notifyAll();
	}

	protected synchronized void checkClose() {
		if (state_ == State.CLOSED) {
			throw new IllegalStateException("Trying to close a closed resouce");
		}
	}

	protected synchronized void checkState() throws ConnectionLostException {
		if (state_ == State.CLOSED) {
			throw new IllegalStateException("Trying to use a closed resouce");
		} else if (state_ == State.DISCONNECTED) {
			throw new ConnectionLostException();
		}
	}

	protected synchronized void safeWait() throws ConnectionLostException, InterruptedException {
		if (state_ == State.CLOSED) {
			throw new InterruptedException("Resource closed");
		} else if (state_ == State.DISCONNECTED) {
			throw new ConnectionLostException();
		}
		wait();
	}

}