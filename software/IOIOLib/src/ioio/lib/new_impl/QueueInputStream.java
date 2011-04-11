package ioio.lib.new_impl;

import java.io.IOException;
import java.io.InputStream;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

import android.util.Log;

public class QueueInputStream extends InputStream {
	private BlockingQueue<Byte> queue_ = new ArrayBlockingQueue<Byte>(
			Constants.BUFFER_SIZE);
	private boolean closed_ = false;

	@Override
	public int read() throws IOException {
		if (closed_) {
			throw new IOException("Stream has been closed");
		}
		try {
			return queue_.take();
		} catch (InterruptedException e) {
			return -1;
		}
	}
	
	public void write(byte[] data, int size) {
		for (int i = 0; i < size; ++i) {
			queue_.add(data[i]);
		}
	}

	@Override
	public int available() throws IOException {
		return queue_.size();
	}

	@Override
	public void close() {
		closed_ = true;
	}

}
