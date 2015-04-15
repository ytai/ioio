package ioio.lib.impl;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Similar to a {@link BufferedInputStream}, but guarantees that all reads from the source stream
 * are exactly the specified size of the buffer.
 * It turns out, {@link BufferedInputStream} does not actually have such a guarantee.
 */
public class FixedReadBufferedInputStream extends InputStream {
	private int bufferIndex_ = 0;
	private int validData_ = 0;
	private final byte[] buffer_;
	private final InputStream source_;

	public FixedReadBufferedInputStream(InputStream source, int size) {
		buffer_ = new byte[size];
		source_ = source;
	}

	@Override
	public int available() throws IOException {
		return validData_ - bufferIndex_;
	}

	@Override
	public void close() throws IOException {
		source_.close();
	}

	@Override
	public int read(byte[] buffer, int offset, int length) throws IOException {
		fillIfEmpty();
		if (validData_ == -1) {
			return -1;
		}
		length = Math.min(length, validData_ - bufferIndex_);
		System.arraycopy(buffer_, bufferIndex_, buffer, offset, length);
		bufferIndex_ += length;
		return length;
	}

	@Override
	public int read(byte[] buffer) throws IOException {
		return read(buffer, 0, buffer.length);
	}

	@Override
	public int read() throws IOException {
		fillIfEmpty();
		if (validData_ == -1) {
			return -1;
		}
		return buffer_[bufferIndex_++] & 0xFF;
	}

	@Override
	public long skip(long byteCount) throws IOException {
		long skipped = 0;
		while (byteCount > 0) {
			fillIfEmpty();
			if (validData_ == -1) {
				return skipped;
			}
			int count = (int) Math.min(available(), byteCount);
			byteCount -= count;
			bufferIndex_ += count;
			skipped += count;
		}
		return skipped;
	}

	private void fillIfEmpty() throws IOException {
		while (available() == 0 && validData_ != -1) {
			fill();
		}
	}

	private void fill() throws IOException {
		bufferIndex_ = 0;
		validData_ = source_.read(buffer_);
	}
}
