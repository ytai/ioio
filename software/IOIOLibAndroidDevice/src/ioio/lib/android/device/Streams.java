package ioio.lib.android.device;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;

public class Streams {
	// It seems that for bulkTransfer, giving timeout of 0 does what we need.
	// The documentation on the Android side of things is not clear about what
	// we should choose, so as long as it works - great.
	static final int TRANSFER_TIMEOUT_MILLIS = 0;

	public static class DeviceInputStream extends InputStream {
		private final byte[] oneByteBuffer_ = new byte[1];
		private final UsbDeviceConnection connection_;
		private final UsbEndpoint ep_;

		DeviceInputStream(UsbDeviceConnection connection, UsbEndpoint ep) {
			this.connection_ = connection;
			this.ep_ = ep;
		}

		@Override
		public int read() throws IOException {
			return read(oneByteBuffer_) == 1 ? oneByteBuffer_[0] : -1;
		}

		@Override
		public int read(byte[] buffer) throws IOException {
			return read(buffer, 0, buffer.length);
		}

		@Override
		public synchronized int read(byte[] buffer, int offset, int length) throws IOException {
			if (offset == 0) {
				return connection_.bulkTransfer(ep_, buffer, buffer.length, TRANSFER_TIMEOUT_MILLIS);
			}
			// This next code is not very efficient. It allocates more memory, and does a copy of it.
			// It seems that there's no much better way to do that without compromising memory
			// footprint (by having a member buffer).
			// BUT: This code never gets called from IOIOLibAndroid - so it shouldn't hurt us.
			byte[] tmp = new byte[length];
			int readAmount = connection_.bulkTransfer(ep_, tmp, buffer.length,
					TRANSFER_TIMEOUT_MILLIS);
			System.arraycopy(tmp, 0, buffer, offset, Math.min(length, readAmount));
			return readAmount;
		}
	}

	public static class DeviceOutputStream extends OutputStream {
		private final UsbDeviceConnection connection_;
		private final UsbEndpoint ep_;
		private final byte[] oneByteBuffer_ = new byte[1];

		DeviceOutputStream(UsbDeviceConnection connection, UsbEndpoint ep) {
			this.connection_ = connection;
			this.ep_ = ep;
		}

		@Override
		public synchronized void write(int oneByte) throws IOException {
			oneByteBuffer_[0] = (byte) oneByte;
			write(oneByteBuffer_);
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
					offset = connection_.bulkTransfer(ep_, buffer, count, TRANSFER_TIMEOUT_MILLIS);
					if (offset < 0) {
						throw new IOException("Couldn't write to USB");
					}
					count -= offset;
					continue;
				}
				// Not sure if we want to allocate this only once.
				// On one hand - this will do less allocations,
				// on the other hand, this code should almost never run, thus
				// allocating once
				// will enlarge the memory footprint of the application.
				byte[] tmp = new byte[count];
				System.arraycopy(buffer, offset, tmp, 0, count);
				int written = connection_.bulkTransfer(ep_, tmp, count, TRANSFER_TIMEOUT_MILLIS);
				if (written < 0) {
					throw new IOException("Couldn't write to USB");
				}
				offset += written;
				count -= written;
			}
		}
	}
}
