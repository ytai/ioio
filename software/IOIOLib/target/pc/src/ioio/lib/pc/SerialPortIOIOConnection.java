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
package ioio.lib.pc;

import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;
import ioio.lib.api.IOIOConnection;
import ioio.lib.api.exception.ConnectionLostException;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class SerialPortIOIOConnection implements IOIOConnection {
	// private static final String TAG = "SerialPortIOIOConnection";
	private final CommPortIdentifier identifier_;
	private SerialPort serialPort_;
	private InputStream inputStream_;
	private OutputStream outputStream_;
	boolean disconnect_ = false;

	public SerialPortIOIOConnection(CommPortIdentifier identifier) {
		identifier_ = identifier;
	}

	// RXTX has an annoying bug:
	// Calling close() on a port while another thread is blocking on read()
	// won't unblock the other thread and will block the thread calling close(),
	// thus causing a deadlock.
	// The (ugly) workaround is to set a short timeout on read operations and
	// whenever it expires check whether or not we should keep trying to read.
	// Apparently, there is not way to differentiate a timeout from a real EOF
	// when using read(), but in our case, we're only using
	// read(byte[], int, int)
	class InputStreamWrapper extends InputStream {
		private final InputStream in_;

		InputStreamWrapper(InputStream in) {
			in_ = in;
		}

		@Override
		public int read() throws IOException {
			while (true) {
				int i;
				if ((i = in_.read()) != -1 || disconnect_) {
					return i;
				}
			}
		}

		@Override
		public int read(byte[] b, int off, int len) throws IOException {
			while (true) {
				int i;
				if ((i = in_.read(b, off, len)) != 0 || disconnect_) {
					return i;
				}
			}
		}
	}

	@Override
	public void waitForConnect() throws ConnectionLostException {
		try {
			synchronized (this) {
				CommPort commPort = identifier_.open(this.getClass().getName(),
						10000);
				serialPort_ = (SerialPort) commPort;
				serialPort_.enableReceiveTimeout(1000);
				serialPort_.enableReceiveThreshold(1);
				inputStream_ = new InputStreamWrapper(
						serialPort_.getInputStream());
				outputStream_ = serialPort_.getOutputStream();
			}
		} catch (Exception e) {
			if (serialPort_ != null) {
				serialPort_.close();
			}
			throw new ConnectionLostException(e);
		}
	}

	@Override
	synchronized public void disconnect() {
		disconnect_ = true;
		if (serialPort_ != null) {
			serialPort_.close();
		}
	}

	@Override
	public InputStream getInputStream() throws ConnectionLostException {
		return inputStream_;
	}

	@Override
	public OutputStream getOutputStream() throws ConnectionLostException {
		return outputStream_;
	}

	@Override
	public boolean canClose() {
		return true;
	}
}
