package ioio.programmer;

import java.io.IOException;
import java.io.InputStream;

public class IOIOFileReader {
	int address_;
	byte[] buf_ = new byte[192];

	public class FormatException extends Exception {
		private static final long serialVersionUID = 7944061537571462938L;

		public FormatException(String msg) {
			super(msg);
		}

		public FormatException(Exception e) {
			super(e);
		}
	}

	private final InputStream in_;

	public IOIOFileReader(InputStream in) throws FormatException {
		in_ = in;
		mustRead(8);
		if (buf_[0] != 'I' || buf_[1] != 'O' || buf_[2] != 'I'
				|| buf_[3] != 'O') {
			throw new FormatException("Bad header, expected 'IOIO'");
		}
		int ver = readInt(4);
		if (ver != 1) {
			throw new FormatException(
					"Unsupported file format version, expected 1, got: " + ver);
		}
	}

	public boolean next() throws FormatException {
		try {
			int r = read(4);
			if (r == 0) {
				return false;
			}
			if (r != 4) {
				throw new FormatException("Unexpected EOF");
			}
		} catch (IOException e) {
			throw new FormatException(e);
		}
		address_ = readInt(0);
		mustRead(192);
		return true;
	}

	public byte[] currentBlock() {
		return buf_;
	}

	public int currentAddress() {
		return address_;
	}

	private int read(int size) throws IOException {
		int offset = 0;
		while (offset < size) {
			int r = in_.read(buf_, offset, size - offset);
			if (r == -1) {
				return offset;
			}
			offset += r;
		}
		return offset;
	}

	private void mustRead(int size) throws FormatException {
		try {
			if (read(size) != size) {
				throw new FormatException("Unexpected EOF");
			}
		} catch (IOException e) {
			throw new FormatException(e);
		}
	}

	private int readInt(int offset) {
		return (byteToInt(buf_[offset]) << 0)
				| (byteToInt(buf_[offset + 1]) << 8)
				| (byteToInt(buf_[offset + 2]) << 16)
				| (byteToInt(buf_[offset + 3]) << 24);
	}
	
	private static int byteToInt(byte b) {
		return ((int) b) & 0xFF;
	}
}
