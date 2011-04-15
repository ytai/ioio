package ioio.lib.impl;

import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.DisconnectListener;

import java.io.Closeable;

public class AbstractResource implements Closeable, DisconnectListener {
	enum State {
		INIT,
		OPEN,
		CLOSED,
		DISCONNECTED
	}
	
	protected State state_ = State.INIT;
	protected IOIOImpl ioio_;

	public AbstractResource(IOIOImpl ioio) throws ConnectionLostException {
		ioio_ = ioio;
		ioio_.addDisconnectListener(this);
	}
	
	@Override
	synchronized public void disconnected() {
		if (state_ != State.CLOSED) {
			state_ = State.DISCONNECTED;
		}
	}

	synchronized public void close() {
		if (state_ == State.CLOSED) {
			throw new IllegalStateException("Trying to use a closed resouce");
		} else if (state_ == State.DISCONNECTED) {
			return;
		}
		state_ = State.CLOSED;
		ioio_.removeDisconnectListener(this);
	}
	
	protected void checkState() throws ConnectionLostException {
		if (state_ == State.CLOSED) {
			throw new IllegalStateException("Trying to use a closed resouce");
		} else if (state_ == State.DISCONNECTED) {
			throw new ConnectionLostException();
		}
	}
}
