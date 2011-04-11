package ioio.lib.new_impl;

import android.util.Log;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.InvalidStateException;
import ioio.lib.new_impl.IncomingState.Listener;
import ioio.lib.new_impl.IncomingState.PinMode;

public abstract class AbstractPin implements Listener {
	enum State {
		INIT,
		OPEN,
		CLOSED,
		LOST
	}
	
	protected IOIOImpl ioio_;
	protected State state_ = State.INIT;
	protected int pinNum_;
	
	AbstractPin(IOIOImpl ioio, int pinNum) {
		ioio_ = ioio;
		pinNum_ = pinNum;
	}
	
	@Override
	synchronized public void opened(PinMode mode) {
		Log.i("AbstractPin", "Pin " + pinNum_ + " opened as " + mode.name());
		state_ = State.OPEN;
	}

	@Override
	synchronized public void closed() {
		Log.i("AbstractPin", "Pin " + pinNum_ + " closed");
		state_ = State.CLOSED;
	}

	@Override
	synchronized public void lost() {
		Log.i("AbstractPin", "Pin " + pinNum_ + " lost");
		if (state_ != State.CLOSED) {
			state_ = State.LOST;
		}
	}

	synchronized public void close() {
		Log.i("AbstractPin", "Closing pin " + pinNum_);
		ioio_.closePin(pinNum_);
		state_ = State.CLOSED;
	}
	
	protected void checkState() throws ConnectionLostException {
		if (state_ == State.CLOSED) {
			throw new InvalidStateException("Trying to use a closed pin");
		} else if (state_ == State.LOST) {
			throw new ConnectionLostException();
		}
	}

}
