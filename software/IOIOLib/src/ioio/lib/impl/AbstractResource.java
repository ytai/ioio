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

import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.DisconnectListener;
import ioio.lib.api.Closeable;

class AbstractResource implements Closeable, DisconnectListener {
	enum State {
		OPEN,
		CLOSED,
		DISCONNECTED
	}
	
	protected State state_ = State.OPEN;
	protected final IOIOImpl ioio_;

	public AbstractResource(IOIOImpl ioio) throws ConnectionLostException {
		ioio_ = ioio;
	}
	
	@Override
	synchronized public void disconnected() {
		if (state_ != State.CLOSED) {
			state_ = State.DISCONNECTED;
		}
	}

	@Override
	synchronized public void close() {
		if (state_ == State.CLOSED) {
			throw new IllegalStateException("Trying to use a closed resouce");
		} else if (state_ == State.DISCONNECTED) {
			state_ = State.CLOSED;
			return;
		}
		state_ = State.CLOSED;
		ioio_.removeDisconnectListener(this);
	}
	
	protected synchronized void checkState() throws ConnectionLostException {
		if (state_ == State.CLOSED) {
			throw new IllegalStateException("Trying to use a closed resouce");
		} else if (state_ == State.DISCONNECTED) {
			throw new ConnectionLostException();
		}
	}
}
