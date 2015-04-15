/*
 * Copyright 2013 Ytai Ben-Tsvi. All rights reserved.
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
package ioio.lib.android.device;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;

/**
 * Wrappers around USB bulk endpoints, which expose them as I/O streams.
 * 
 * @author Misha Seltzer
 * @author Nadir Izrael
 * @author Ytai Ben-Tsvi
 */
public class Streams {
	// It seems that for bulkTransfer, giving timeout of 0 does what we need.
	// The documentation on the Android side of things is not clear about what
	// we should choose, so as long as it works - great.
	static final int TRANSFER_TIMEOUT_MILLIS = 0;

	public static class DeviceInputStream extends InputStream {
		private final byte[] buffer_ = new byte[1024];
		private final UsbDeviceConnection connection_;
		private final UsbEndpoint ep_;

		DeviceInputStream(UsbDeviceConnection connection, UsbEndpoint ep) {
			this.connection_ = connection;
			this.ep_ = ep;
		}

		@Override
		public int read() throws IOException {
			return read(buffer_, 0, 1) == 1 ? buffer_[0] : -1;
		}

		@Override
		public int read(byte[] buffer) throws IOException {
			return read(buffer, 0, buffer.length);
		}

		@Override
		public synchronized int read(byte[] buffer, int offset, int length) throws IOException {
			if (offset == 0) {
				return connection_.bulkTransfer(ep_, buffer, length, TRANSFER_TIMEOUT_MILLIS);
			}
			// We have to go through an intermediate buffer and copy, since the API won't let us
			// write to a non-0
			// offset.
			int readAmount = connection_.bulkTransfer(ep_, buffer_,
					Math.min(length, buffer_.length), TRANSFER_TIMEOUT_MILLIS);
			System.arraycopy(buffer_, 0, buffer, offset, readAmount);
			return readAmount;
		}
	}

	public static class DeviceOutputStream extends OutputStream {
		private final UsbDeviceConnection connection_;
		private final UsbEndpoint ep_;
		private final byte[] buffer_ = new byte[1024];

		DeviceOutputStream(UsbDeviceConnection connection, UsbEndpoint ep) {
			this.connection_ = connection;
			this.ep_ = ep;
		}

		@Override
		public synchronized void write(int oneByte) throws IOException {
			buffer_[0] = (byte) oneByte;
			write(buffer_, 0, 1);
		}

		@Override
		public void write(byte[] buffer) throws IOException {
			write(buffer, 0, buffer.length);
		}

		@Override
		public synchronized void write(byte[] buffer, int offset, int count) throws IOException {
			if (count > (buffer.length - offset)) {
				throw new IOException("Count is too big");
			}
			while (count > 0) {
				if (offset == 0) {
					// This is an optimization: when the offset is 0, we can pass the original
					// buffer and avoid a copy.
					offset = connection_.bulkTransfer(ep_, buffer, count, TRANSFER_TIMEOUT_MILLIS);
					if (offset < 0) {
						throw new IOException("Couldn't write to USB");
					}
					count -= offset;
				} else {
					// We cannot write directly from the buffer, since the API won't let us use a
					// non-0 offset.
					// So we have to use an intermediate buffer and copy.
					int copied = Math.min(count, buffer_.length);
					System.arraycopy(buffer, offset, buffer_, 0, copied);
					int written = connection_.bulkTransfer(ep_, buffer_, copied,
							TRANSFER_TIMEOUT_MILLIS);
					if (written < 0) {
						throw new IOException("Couldn't write to USB");
					}
					offset += written;
					count -= written;
				}
			}
		}
	}
}
