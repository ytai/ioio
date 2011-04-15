package ioio.lib.impl;

import java.io.IOException;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

public class FlowControlledPacketSender {
	interface Packet {
		int getSize();
	}
	
	interface Sender {
		void send(Packet packet);
	}

	private Sender sender_;
	private BlockingQueue<Packet> queue_ = new ArrayBlockingQueue<Packet>(
			Constants.PACKET_BUFFER_SIZE);
	FlushThread thread_ = new FlushThread();
	private int readyToSend_ = 0;
	private boolean closed_ = false;

	public FlowControlledPacketSender(Sender sender) {
		sender_ = sender;
		thread_.start();
	}

	synchronized public void flush() throws IOException {
		try {
			while (!queue_.isEmpty()) {
				wait();
			}
		} catch (InterruptedException e) {
		}
	}

	synchronized public void write(Packet packet) {
		if (closed_) {
			throw new IllegalStateException("Stream has been closed");
		}
		queue_.add(packet);
		notifyAll();
	}

	synchronized public void readyToSend(int numBytes) {
		assert(numBytes >= readyToSend_);
		readyToSend_ = numBytes;
		notifyAll();
	}
	
	synchronized public void close() {
		closed_ = true;
		thread_.interrupt();
	}
	
	synchronized public void kill() {
		thread_.interrupt();
	}

	class FlushThread extends Thread {
		@Override
		public void run() {
			super.run();
			try {
				while (true) {
					synchronized (FlowControlledPacketSender.this) {
						while (queue_.isEmpty() || readyToSend_ < queue_.peek().getSize()) {
							FlowControlledPacketSender.this.wait();
						}
						FlowControlledPacketSender.this.notifyAll();
						readyToSend_ -= queue_.peek().getSize();
					}
					sender_.send(queue_.remove());
				}
			} catch (InterruptedException e) {
			}
		}
	}
}
