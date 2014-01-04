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
package ioio.dude;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public class IOIODudeMain {
	private static final int ESTABLISH_CONNECTION = 0x00;
	private static final int CHECK_INTERFACE = 0x01;
	private static final int CHECK_INTERFACE_RESPONSE = 0x01;
	private static final int READ_FINGERPRINT = 0x02;
	private static final int FINGERPRINT = 0x02;
	private static final int WRITE_FINGERPRINT = 0x03;
	private static final int WRITE_IMAGE = 0x04;
	private static final int CHECKSUM = 0x04;

	private static enum Protocol {
		PROTOCOL_IOIO, PROTOCOL_BOOTLOADER
	}

	private static enum Command {
		VERSIONS, FINGERPRINT, WRITE
	}

	private static SerialPortIOIOConnection connection_;
	private static OutputStream out_;
	private static InputStream in_;
	private static final byte[] buffer_ = new byte[16];
	private static final int PROGRESS_SIZE = 40;

	private static String hardwareVersion_;
	private static String bootloaderVersion_;
	private static String applicationVersion_;
	private static String platformVersion_;
	private static Protocol whatIsConnected_;

	private static String portName_;
	private static Command command_;
	private static String fileName_;
	private static boolean reset_ = false;
	private static boolean force_ = false;

	private static void printUsage() {
		System.err.println("IOIODude V1.1");
		System.err.println();
		System.err.println("Usage:");
		System.err.println("ioiodude <options> versions");
		System.err.println("ioiodude <options> fingerprint");
		System.err.println("ioiodude <options> write <ioioapp>");
		System.err.println();
		System.err.println("Valid options are:");
		System.err
				.println("--port=<name> The serial port where the IOIO is connected.");
		System.err
				.println("--reset Reset the IOIO out of bootloader mode when done.");
		System.err
				.println("--force Bypass fingerprint matching and force writing.");
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if (args.length == 0) {
			printUsage();
			System.exit(1);
		}

		try {
			parseArgs(args);
		} catch (BadArgumentsException e) {
			System.err.println("Invalid command line.");
			System.err.println(e.getMessage());
			printUsage();
			System.exit(2);
		}

		try {
			connect(portName_);
			switch (command_) {
			case VERSIONS:
				versionsCommand();
				break;

			case FINGERPRINT:
				fingerprintCommand();
				break;

			case WRITE:
				writeCommand();
				break;
			}
			if (reset_) {
				hardReset();
			}
		} catch (IOException e) {
			System.err.println("Caught IOException. Exiting.");
			System.exit(3);
		} catch (ProtocolException e) {
			System.err.println("Protocol error:");
			System.err.println(e.getMessage());
			System.err.println("Exiting.");
			System.exit(4);
		} catch (NoSuchAlgorithmException e) {
			System.err.println("System cannot calculate MD5. Cannot proceed.");
			System.exit(5);
		}
	}

	private static void hardReset() throws IOException {
		out_.write(new byte[] { 0x00, 'I', 'O', 'I', 'O' });
	}

	private static void writeCommand() throws IOException, ProtocolException,
			NoSuchAlgorithmException {
		checkBootloaderProtocol();
		File file = new File(fileName_);
		ZipFile zip = new ZipFile(file, ZipFile.OPEN_READ);
		try {
			ZipEntry entry = zip.getEntry(platformVersion_ + ".ioio");
			if (entry == null) {
				System.err
						.println("Application bundle does not include an image for the platform "
								+ platformVersion_);
				return;
			}

			byte[] fileFp = calculateFingerprint(zip.getInputStream(entry));

			if (!force_) {
				System.err.println("Comparing fingerprints...");
				byte[] currentFp = readFingerprint();

				if (Arrays.equals(currentFp, fileFp)) {
					System.err.println("Fingerprint match - skipping write.");
					return;
				} else {
					System.err.println("Fingerprint mismatch.");
				}
			}

			System.err.println("Writing image...");
			short checksum = writeImage(zip.getInputStream(entry),
					(int) entry.getSize());
			if (readChecksum() != checksum) {
				throw new ProtocolException(
						"Bad checksum. IOIO image is possibly corrupt.");
			}
			System.err.println("Writing fingerprint...");
			writeFingerprint(fileFp);
			System.err.println("Done.");
		} finally {
			zip.close();
		}
	}

	private static short writeImage(InputStream in, int length)
			throws IOException {
		out_.write(WRITE_IMAGE);

		out_.write((int) ((length >> 0) & 0xff));
		out_.write((int) ((length >> 8) & 0xff));
		out_.write((int) ((length >> 16) & 0xff));
		out_.write((int) (length >> 24) & 0xff);

		short checksum = 0;
		int written = 0;
		int progress = -1;
		int i;
		byte[] buffer = new byte[1024];
		while (-1 != (i = in.read(buffer))) {
			for (int j = 0; j < i; ++j) {
				checksum += ((int) buffer[j]) & 0xFF;
			}
			out_.write(buffer, 0, i);
			written += i;
			if (written * PROGRESS_SIZE / length != progress) {
				progress = written * PROGRESS_SIZE / length;
				printProgress(progress);
			}
		}
		System.err.println();
		return checksum;
	}

	private static void printProgress(int progress) {
		System.err.print('[');
		for (int i = 0; i < PROGRESS_SIZE; ++i) {
			if (i < progress) {
				System.err.print('#');
			} else {
				System.err.print(' ');
			}
		}
		System.err.print(']');
		System.err.print('\r');
	}

	private static short readChecksum() throws IOException, ProtocolException {
		if (in_.read() != CHECKSUM) {
			throw new ProtocolException("Unexpected response.");
		}
		readExactly(2);
		final int b0 = ((int) buffer_[0]) & 0xff;
		final int b1 = ((int) buffer_[1]) & 0xff;
		return (short) (b0 | b1 << 8);
	}

	private static byte[] calculateFingerprint(InputStream in) throws IOException,
			NoSuchAlgorithmException {
		MessageDigest digester = MessageDigest.getInstance("MD5");
		byte[] bytes = new byte[1024];
		int byteCount;
		while ((byteCount = in.read(bytes)) > 0) {
			digester.update(bytes, 0, byteCount);
		}
		return digester.digest();
	}

	private static void writeFingerprint(byte[] fingerprint) throws IOException {
		assert fingerprint.length == 16;
		out_.write(WRITE_FINGERPRINT);
		out_.write(fingerprint);
	}

	private static void fingerprintCommand() throws IOException,
			ProtocolException {
		checkBootloaderProtocol();
		byte[] fingerprint = readFingerprint();
		for (int i = 0; i < 16; ++i) {
			System.out
					.print(Integer.toHexString(((int) fingerprint[i]) & 0xff));
		}
		System.out.println();
	}

	private static byte[] readFingerprint() throws ProtocolException,
			IOException {
		out_.write(READ_FINGERPRINT);
		if (in_.read() != FINGERPRINT) {
			throw new ProtocolException("Unexpected response.");
		}
		readExactly(16);
		byte[] fingerprint = new byte[16];
		System.arraycopy(buffer_, 0, fingerprint, 0, 16);
		return fingerprint;
	}

	private static void checkBootloaderProtocol() throws IOException,
			ProtocolException {
		if (whatIsConnected_ != Protocol.PROTOCOL_BOOTLOADER) {
			throw new ProtocolException(
					"The IOIO is not in bootloader mode!\n"
							+ "Enter bootloader mode by:\n"
							+ "- Power off the IOIO.\n"
							+ "- Connect the 'boot' pin to 'GND'.\n"
							+ "- Power on the IOIO.\n"
							+ "- The stat LED should be on constantly.\n"
							+ "- Disconnect 'boot' from 'GND'. The stat LED should blink a few times.\n"
							+ "Now, try again.");
		}
		out_.write(CHECK_INTERFACE);
		out_.write("BOOT0001".getBytes());
		readExactly(2);
		if (buffer_[0] != CHECK_INTERFACE_RESPONSE) {
			throw new ProtocolException("Unexpected response.");
		}
		if ((buffer_[1] & 0x01) == 0) {
			throw new ProtocolException(
					"Bootloader does not support protocol BOOT0001.");
		}
	}

	private static void versionsCommand() {
		switch (whatIsConnected_) {
		case PROTOCOL_IOIO:
			System.err.println("IOIO Application detected.");
			break;

		case PROTOCOL_BOOTLOADER:
			System.err.println("IOIO Bootloader detected.");
			break;
		}
		System.err.println();
		System.err.println("Hardware version: " + hardwareVersion_);
		System.err.println("Bootloader version: " + bootloaderVersion_);
		switch (whatIsConnected_) {
		case PROTOCOL_IOIO:
			System.err.println("Application version: " + applicationVersion_);
			break;

		case PROTOCOL_BOOTLOADER:
			System.err.println("Platform version: " + platformVersion_);
			break;
		}
	}

	private static void parseArgs(String[] args) throws BadArgumentsException {
		int nonOptionArgNum = 0;
		for (String arg : args) {
			if (arg.startsWith("--")) {
				parseOption(arg);
			} else {
				if (nonOptionArgNum == 0) {
					parseCommand(arg);
				} else {
					switch (command_) {
					case VERSIONS:
					case FINGERPRINT:
						throw new BadArgumentsException("Unexpected argument: "
								+ arg);

					case WRITE:
						if (nonOptionArgNum == 1) {
							fileName_ = arg;
						} else {
							throw new BadArgumentsException(
									"Unexpected argument: " + arg);
						}
						break;
					}
				}
				++nonOptionArgNum;
			}
		}
		if (command_ == null) {
			throw new BadArgumentsException("Expected command.");
		}
		if (portName_ == null) {
			throw new BadArgumentsException(
					"Expected port. Please use the --port option.");
		}
		if (command_ == Command.WRITE && fileName_ == null) {
			throw new BadArgumentsException("Expected file name.");
		}
	}

	private static void parseCommand(String arg) throws BadArgumentsException {
		try {
			command_ = Command.valueOf(arg.toUpperCase());
		} catch (IllegalArgumentException e) {
			throw new BadArgumentsException("Unrecognized command: " + arg);
		}
	}

	private static void parseOption(String arg) throws BadArgumentsException {
		if (arg.startsWith("--port=")) {
			portName_ = arg.substring(7);
		} else if (arg.equals("--reset")) {
			reset_ = true;
		} else if (arg.equals("--force")) {
			force_ = true;
		} else {
			throw new BadArgumentsException("Unexpected option: " + arg);
		}
	}

	private static void connect(String port) throws IOException,
			ProtocolException {
		connection_ = new SerialPortIOIOConnection(port);
		connection_.waitForConnect();
		out_ = connection_.getOutputStream();
		in_ = connection_.getInputStream();

		if (in_.read() != ESTABLISH_CONNECTION) {
			throw new ProtocolException("Got and unexpected input.");
		}
		readExactly(4);
		if (bufferIsIOIO()) {
			whatIsConnected_ = Protocol.PROTOCOL_IOIO;
			readIOIOVersions();
		} else if (bufferIsBoot()) {
			whatIsConnected_ = Protocol.PROTOCOL_BOOTLOADER;
			readBootVersions();
		} else {
			throw new ProtocolException(
					"Device is neighter a standard IOIO application nor a IOIO bootloader.");
		}
	}

	private static void readIOIOVersions() throws ProtocolException,
			IOException {
		byte[] ver = new byte[8];
		readExactly(ver, 0, 8);
		hardwareVersion_ = new String(ver);
		readExactly(ver, 0, 8);
		bootloaderVersion_ = new String(ver);
		readExactly(ver, 0, 8);
		applicationVersion_ = new String(ver);
	}

	private static void readBootVersions() throws ProtocolException,
			IOException {
		byte[] ver = new byte[8];
		readExactly(ver, 0, 8);
		hardwareVersion_ = new String(ver);
		readExactly(ver, 0, 8);
		bootloaderVersion_ = new String(ver);
		readExactly(ver, 0, 8);
		platformVersion_ = new String(ver);
	}

	private static boolean bufferIsBoot() {
		return buffer_[0] == 'B' && buffer_[1] == 'O' && buffer_[2] == 'O'
				&& buffer_[3] == 'T';
	}

	private static boolean bufferIsIOIO() {
		return buffer_[0] == 'I' && buffer_[1] == 'O' && buffer_[2] == 'I'
				&& buffer_[3] == 'O';
	}

	private static void readExactly(int size) throws ProtocolException,
			IOException {
		readExactly(buffer_, 0, size);
	}

	private static void readExactly(byte[] buffer, int offset, int size)
			throws ProtocolException, IOException {
		while (size > 0) {
			int num = in_.read(buffer, offset, size);
			if (num < 0) {
				throw new ProtocolException("Unexpected connection closure.");
			}
			size -= num;
			offset += num;
		}
	}

	private static class ProtocolException extends Exception {
		private static final long serialVersionUID = 4332923296318917793L;

		public ProtocolException(String message) {
			super(message);
		}
	}

	private static class BadArgumentsException extends Exception {
		private static final long serialVersionUID = -5730905669013719779L;

		public BadArgumentsException(String message) {
			super(message);
		}
	}

}
