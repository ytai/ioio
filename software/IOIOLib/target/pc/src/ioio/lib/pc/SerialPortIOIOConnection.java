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

import ioio.lib.api.IOIOConnection;
import ioio.lib.api.exception.ConnectionLostException;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import purejavacomm.CommPort;
import purejavacomm.CommPortIdentifier;
import purejavacomm.NoSuchPortException;
import purejavacomm.SerialPort;

class SerialPortIOIOConnection implements IOIOConnection {
	// private static final String TAG = "SerialPortIOIOConnection";
	private boolean abort_ = false;
	private final String name_;
	private SerialPort serialPort_;
	private InputStream inputStream_;
	private OutputStream outputStream_;

	public SerialPortIOIOConnection(String name) {
		name_ = name;
	}

	@Override
	public void waitForConnect() throws ConnectionLostException {
		while (!abort_) {
			try {
				CommPortIdentifier identifier = CommPortIdentifier
						.getPortIdentifier(name_);
				CommPort commPort = identifier.open(this.getClass().getName(),
						1000);
				synchronized (this) {
					if (!abort_) {
						serialPort_ = (SerialPort) commPort;
						serialPort_.enableReceiveThreshold(1);
						serialPort_.enableReceiveTimeout(500);
						serialPort_.setDTR(true);
						Thread.sleep(100);
						inputStream_ = new GracefullyClosingInputStream(
								serialPort_.getInputStream());
						outputStream_ = serialPort_.getOutputStream();
						return;
					}
				}
			} catch (NoSuchPortException e) {
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e1) {
				}
			} catch (Exception e) {
				if (serialPort_ != null) {
					serialPort_.close();
				}
			}
		}
		throw new ConnectionLostException();
	}

	@Override
	synchronized public void disconnect() {
		abort_ = true;
		if (serialPort_ != null) {
			try {
				inputStream_.close();
			} catch (IOException e) {
			}
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

	// This is a hack:
	// On Linux and OSX, PJC will not unblock a blocked read() on an input
	// stream when closed from another thread.
	// The workaround is to set a timeout on the InputStream and to read in a
	// loop until something is actually read.
	// Since a timeout is indistinguishable from an end-of-stream when using 
	// the no-argument read(), we set a flag to designate that this is a real
	// close, prior to actually closing, causing the read loop to exit upon the
	// next timeout.
	private static class GracefullyClosingInputStream extends InputStream {
		private final InputStream underlying_;
		private boolean closed_ = false;

		public GracefullyClosingInputStream(InputStream is) {
			underlying_ = is;
		}

		@Override
		public int read(byte[] b) throws IOException {
			while (!closed_) {
				int i = underlying_.read(b);
				if (i > 0) {
					return i;
				}
			}
			;
			return -1;
		}

		@Override
		public int read(byte[] b, int off, int len) throws IOException {
			while (!closed_) {
				int i = underlying_.read(b, off, len);
				if (i > 0) {
					return i;
				}
			}
			;
			return -1;
		}

		@Override
		public long skip(long n) throws IOException {
			return underlying_.skip(n);
		}

		@Override
		public int available() throws IOException {
			return underlying_.available();
		}

		@Override
		public void close() throws IOException {
			closed_ = true;
			underlying_.close();
		}

		@Override
		public synchronized void mark(int readlimit) {
			underlying_.mark(readlimit);
		}

		@Override
		public synchronized void reset() throws IOException {
			underlying_.reset();
		}

		@Override
		public boolean markSupported() {
			return underlying_.markSupported();
		}

		@Override
		public int read() throws IOException {
			while (!closed_) {
				int i = underlying_.read();
				if (i >= 0) {
					return i;
				}
			}
			;
			return -1;
		}
	}
}
