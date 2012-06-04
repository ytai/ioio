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

import ioio.lib.spi.Log;

import java.io.IOException;
import java.io.InputStream;
import java.util.Queue;
import java.util.concurrent.ArrayBlockingQueue;

class QueueInputStream extends InputStream {
	private enum State {
		OPEN, CLOSED, KILLED
	};

	private final Queue<Byte> queue_ = new ArrayBlockingQueue<Byte>(
			Constants.BUFFER_SIZE);
	private State state_ = State.OPEN;

	@Override
	synchronized public int read() throws IOException {
		try {
			while (state_ == State.OPEN && queue_.isEmpty()) {
				wait();
			}
			if (state_ == State.KILLED) {
				throw new IOException("Stream has been closed");
			}
			if (state_ == State.CLOSED && queue_.isEmpty()) {
				return -1;
			}
			return ((int) queue_.remove()) & 0xFF;
		} catch (InterruptedException e) {
			throw new IOException("Interrupted");
		}
	}

	@Override
	synchronized public int read(byte[] b, int off, int len) throws IOException {
		if (len == 0) {
			return 0;
		}
		try {
			while (state_ == State.OPEN && queue_.isEmpty()) {
				wait();
			}
			if (state_ == State.KILLED) {
				throw new IOException("Stream has been closed");
			}
			if (state_ == State.CLOSED && queue_.isEmpty()) {
				return -1;
			}
			if (len > queue_.size()) {
				len = queue_.size();
			}
			for (int i = 0; i < len; ++i) {
				b[off++] = queue_.remove();
			}
			return len;
		} catch (InterruptedException e) {
			throw new IOException("Interrupted");
		}
	}

	synchronized public void write(byte[] data, int size) {
		for (int i = 0; i < size; ++i) {
			if (queue_.size() == Constants.BUFFER_SIZE) {
				Log.e("QueueInputStream", "Buffer overflow, discarding data");
				break;
			}
			queue_.add(data[i]);
		}
		notifyAll();
	}

	@Override
	synchronized public int available() throws IOException {
		return queue_.size();
	}

	@Override
	synchronized public void close() {
		if (state_ != State.OPEN) {
			return;
		}
		state_ = State.CLOSED;
		notifyAll();
	}

	synchronized public void kill() {
		if (state_ != State.OPEN) {
			return;
		}
		state_ = State.KILLED;
		notifyAll();
	}

}
