package ioio.lib.new_impl;

import java.io.IOException;
import java.io.OutputStream;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

import android.util.Log;

public class FlowControlledOutputStream extends OutputStream {
	interface Sender {
		void send(byte[] data, int size);
	}

	private Sender sender_;
	private BlockingQueue<Byte> queue_ = new ArrayBlockingQueue<Byte>(
			Constants.BUFFER_SIZE);
	FlushThread thread_ = new FlushThread();
	private int readyToSend_ = 0;
	private final int maxPacket_;
	private final byte[] packet_;
	private boolean closed_ = false;

	public FlowControlledOutputStream(Sender sender, int maxPacket) {
		sender_ = sender;
		maxPacket_ = maxPacket;
		packet_ = new byte[maxPacket];
		thread_.start();
	}

	@Override
	synchronized public void flush() throws IOException {
		try {
			while (!queue_.isEmpty()) {
				wait();
			}
		} catch (InterruptedException e) {
		}
	}

	@Override
	synchronized public void write(int oneByte) throws IOException {
		if (closed_) {
			throw new IOException("Stream has been closed");
		}
		queue_.add((byte) oneByte);
		notifyAll();
	}

	synchronized public void readyToSend(int numBytes) {
		assert(numBytes >= readyToSend_);
		readyToSend_ = numBytes;
		notifyAll();
	}
	
	@Override
	synchronized public void close() {
		closed_ = true;
		thread_.interrupt();
	}

	class FlushThread extends Thread {
		@Override
		public void run() {
			super.run();
			try {
				while (true) {
					int toSend;
					synchronized (FlowControlledOutputStream.this) {
						while (readyToSend_ == 0 || queue_.isEmpty()) {
							FlowControlledOutputStream.this.wait();
						}
						toSend = Math.min(maxPacket_, Math.min(readyToSend_, queue_.size()));
						for (int i = 0; i < toSend; ++i) {
							packet_[i] = queue_.remove();
						}
						readyToSend_ -= toSend;
						FlowControlledOutputStream.this.notifyAll();
					}
					sender_.send(packet_, toSend);
				}
			} catch (InterruptedException e) {
			}
		}
	}
}
