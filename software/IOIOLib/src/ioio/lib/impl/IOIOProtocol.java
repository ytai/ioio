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
package ioio.lib.impl;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.SpiMaster;
import ioio.lib.api.TwiMaster.Rate;
import ioio.lib.api.Uart;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

import android.util.Log;

public class IOIOProtocol {
	static final int HARD_RESET                          = 0x00;
	static final int ESTABLISH_CONNECTION                = 0x00;
	static final int SOFT_RESET                          = 0x01;
	static final int CHECK_INTERFACE                     = 0x02;
	static final int CHECK_INTERFACE_RESPONSE            = 0x02;
	static final int SET_PIN_DIGITAL_OUT                 = 0x03;
	static final int SET_DIGITAL_OUT_LEVEL               = 0x04;
	static final int REPORT_DIGITAL_IN_STATUS            = 0x04;
	static final int SET_PIN_DIGITAL_IN                  = 0x05;
	static final int REPORT_PERIODIC_DIGITAL_IN_STATUS   = 0x05;
	static final int SET_CHANGE_NOTIFY                   = 0x06;
	static final int REGISTER_PERIODIC_DIGITAL_SAMPLING  = 0x07;
	static final int SET_PIN_PWM                         = 0x08;
	static final int SET_PWM_DUTY_CYCLE                  = 0x09;
	static final int SET_PWM_PERIOD                      = 0x0A;
	static final int SET_PIN_ANALOG_IN                   = 0x0B;
	static final int REPORT_ANALOG_IN_STATUS             = 0x0B;
	static final int SET_ANALOG_IN_SAMPLING              = 0x0C;
	static final int REPORT_ANALOG_IN_FORMAT             = 0x0C;
	static final int UART_CONFIG                         = 0x0D;
	static final int UART_STATUS                         = 0x0D;
	static final int UART_DATA                           = 0x0E;
	static final int SET_PIN_UART                        = 0x0F;
	static final int UART_REPORT_TX_STATUS               = 0x0F;
	static final int SPI_CONFIGURE_MASTER                = 0x10;
	static final int SPI_STATUS                          = 0x10;
	static final int SPI_MASTER_REQUEST                  = 0x11;
	static final int SPI_DATA                            = 0x11;
	static final int SET_PIN_SPI                         = 0x12;
	static final int SPI_REPORT_TX_STATUS                = 0x12;
	static final int I2C_CONFIGURE_MASTER                = 0x13;
	static final int I2C_STATUS                          = 0x13;
	static final int I2C_WRITE_READ                      = 0x14;
	static final int I2C_RESULT                          = 0x14;
	static final int I2C_REPORT_TX_STATUS                = 0x15;
	
	static final int[] SCALE_DIV = new int[] {
		0x1F,  // 31.25
		0x1E,  // 35.714
		0x1D,  // 41.667
		0x1C,  // 50
		0x1B,  // 62.5
		0x1A,  // 83.333
		0x17,  // 125
		0x16,  // 142.857
		0x15,  // 166.667
		0x14,  // 200
		0x13,  // 250
		0x12,  // 333.333
		0x0F,  // 500
		0x0E,  // 571.429
		0x0D,  // 666.667
		0x0C,  // 800
		0x0B,  // 1000
		0x0A,  // 1333.333
		0x07,  // 2000
		0x06,  // 2285.714
		0x05,  // 2666.667
		0x04,  // 3200
		0x03,  // 4000
		0x02,  // 5333.333
		0x01   // 8000
	};

	private byte[] buf_ = new byte[128];
	private int pos_ = 0;

	private void writeByte(int b) {
		assert(b >= 0 && b < 256);
//		Log.v("IOIOProtocol", "sending: 0x" + Integer.toHexString(b));
		buf_[pos_++] = (byte) b;
	}

	private void flush() throws IOException {
		try {
			out_.write(buf_, 0, pos_);
		} finally {
			pos_ = 0;
		}
	}

	private void writeTwoBytes(int i) throws IOException {
		writeByte(i & 0xFF);
		writeByte(i >> 8);
	}

	synchronized public void hardReset() throws IOException {
		writeByte(HARD_RESET);
		writeByte('I');
		writeByte('O');
		writeByte('I');
		writeByte('O');
		flush();
	}

	synchronized public void softReset() throws IOException {
		writeByte(SOFT_RESET);
		flush();
	}
	
	synchronized public void checkInterface(byte[] interfaceId)
			throws IOException {
		if (interfaceId.length != 8) {
			throw new IllegalArgumentException(
					"interface ID must be exactly 8 bytes long");
		}
		writeByte(CHECK_INTERFACE);
		for (int i = 0; i < 8; ++i) {
			writeByte(interfaceId[i]);
		}
		flush();
	}

	synchronized public void setDigitalOutLevel(int pin, boolean level)
			throws IOException {
		writeByte(SET_DIGITAL_OUT_LEVEL);
		writeByte(pin << 2 | (level ? 1 : 0));
		flush();
	}

	synchronized public void setPinPwm(int pin, int pwmNum, boolean enable)
			throws IOException {
		writeByte(SET_PIN_PWM);
		writeByte(pin & 0x3F);
		writeByte((enable ? 0x80 : 0x00) | (pwmNum & 0x0F));
		flush();
	}

	synchronized public void setPwmDutyCycle(int pwmNum, int dutyCycle,
			int fraction) throws IOException {
		writeByte(SET_PWM_DUTY_CYCLE);
		writeByte(pwmNum << 2 | fraction);
		writeTwoBytes(dutyCycle);
		flush();
	}

	synchronized public void setPwmPeriod(int pwmNum, int period,
			boolean scale256) throws IOException {
		writeByte(SET_PWM_PERIOD);
		writeByte((pwmNum << 1) | (scale256 ? 1 : 0));
		writeTwoBytes(period);
		flush();
	}

	synchronized public void i2cWriteRead(int i2cNum, boolean tenBitAddr,
			int address, int writeSize, int readSize, byte[] writeData)
			throws IOException {
		writeByte(I2C_WRITE_READ);
		writeByte(((address >> 8) << 6) | (tenBitAddr ? 0x20 : 0x00) | i2cNum);
		writeByte(address & 0xFF);
		writeByte(writeSize);
		writeByte(readSize);
		for (int i = 0; i < writeSize; ++i) {
			writeByte(writeData[i]);
		}
		flush();
	}

	synchronized public void setPinDigitalOut(int pin, boolean value,
			DigitalOutput.Spec.Mode mode) throws IOException {
		writeByte(SET_PIN_DIGITAL_OUT);
		writeByte((pin << 2)
				| (mode == DigitalOutput.Spec.Mode.OPEN_DRAIN ? 0x01 : 0x00)
				| (value ? 0x02 : 0x00));
		flush();
	}

	synchronized public void setPinDigitalIn(int pin,
			DigitalInput.Spec.Mode mode) throws IOException {
		int pull = 0;
		if (mode == DigitalInput.Spec.Mode.PULL_UP) {
			pull = 1;
		} else if (mode == DigitalInput.Spec.Mode.PULL_DOWN) {
			pull = 2;
		}
		writeByte(SET_PIN_DIGITAL_IN);
		writeByte((pin << 2) | pull);
		flush();
	}

	synchronized public void setChangeNotify(int pin, boolean changeNotify)
			throws IOException {
		writeByte(SET_CHANGE_NOTIFY);
		writeByte((pin << 2) | (changeNotify ? 0x01 : 0x00));
		flush();
	}

	synchronized public void registerPeriodicDigitalSampling(int pin,
			int freqScale) throws IOException {
		// TODO: implement
	}

	synchronized public void setPinAnalogIn(int pin) throws IOException {
		writeByte(SET_PIN_ANALOG_IN);
		writeByte(pin);
		flush();
	}

	synchronized public void setAnalogInSampling(int pin, boolean enable) throws IOException {
		writeByte(SET_ANALOG_IN_SAMPLING);
		writeByte((enable ? 0x80 : 0x00) | (pin & 0x3F));
		flush();
	}

	synchronized public void uartData(int uartNum, int numBytes, byte data[])
			throws IOException {
		if (numBytes > 64) {
			throw new IllegalArgumentException(
					"A maximum of 64 bytes can be sent in one uartData message. Got: "
							+ numBytes);
		}
		writeByte(UART_DATA);
		writeByte((numBytes - 1) | uartNum << 6);
		for (int i = 0; i < numBytes; ++i) {
			writeByte(data[i]);
		}
		flush();
	}

	synchronized public void uartConfigure(int uartNum, int rate,
			boolean speed4x, Uart.StopBits stopbits, Uart.Parity parity)
			throws IOException {
		int parbits = parity == Uart.Parity.EVEN ? 1
				: (parity == Uart.Parity.ODD ? 2 : 0);
		writeByte(UART_CONFIG);
		writeByte((uartNum << 6) | (speed4x ? 0x08 : 0x00)
				| (stopbits == Uart.StopBits.TWO ? 0x04 : 0x00) | parbits);
		writeTwoBytes(rate);
		flush();
	}

	synchronized public void uartClose(int uartNum) throws IOException {
		writeByte(UART_CONFIG);
		writeByte(uartNum << 6);
		writeTwoBytes(0);
		flush();
	}

	synchronized public void setPinUart(int pin, int uartNum, boolean tx, boolean enable)
			throws IOException {
		writeByte(SET_PIN_UART);
		writeByte(pin);
		writeByte((enable ? 0x80 : 0x00) | (tx ? 0x40 : 0x00) | uartNum);
		flush();
	}

	synchronized public void spiConfigureMaster(int spiNum,
			SpiMaster.Config config) throws IOException {
		writeByte(SPI_CONFIGURE_MASTER);
		writeByte((spiNum << 5) | SCALE_DIV[config.rate.ordinal()]);
		writeByte((config.sampleOnTrailing ? 0x00 : 0x02)
				| (config.invertClk ? 0x01 : 0x00));
		flush();
	}

	synchronized public void spiClose(int spiNum) throws IOException {
		writeByte(SPI_CONFIGURE_MASTER);
		writeByte(spiNum << 5);
		writeByte(0x00);
		flush();
	}

	synchronized public void setPinSpi(int pin, int mode, boolean enable,
			int spiNum) throws IOException {
		writeByte(SET_PIN_SPI);
		writeByte(pin);
		writeByte((1 << 4) | (mode << 2) | spiNum);
		flush();
	}

	synchronized public void spiMasterRequest(int spiNum, int ssPin,
			byte data[], int dataBytes, int totalBytes, int responseBytes)
			throws IOException {
		final boolean dataNeqTotal = (dataBytes != totalBytes);
		final boolean resNeqTotal = (responseBytes != totalBytes);
		writeByte(SPI_MASTER_REQUEST);
		writeByte((spiNum << 6) | ssPin);
		writeByte((dataNeqTotal ? 0x80 : 0x00) | (resNeqTotal ? 0x40 : 0x00)
				| totalBytes - 1);
		if (dataNeqTotal) {
			writeByte(dataBytes);
		}
		if (resNeqTotal) {
			writeByte(responseBytes);
		}
		for (int i = 0; i < dataBytes; ++i) {
			writeByte(data[i]);
		}
		flush();
	}

	synchronized public void i2cConfigureMaster(int i2cNum, Rate rate,
			boolean smbusLevels) throws IOException {
		int rateBits = (rate == Rate.RATE_1MHz ? 3
				: (rate == Rate.RATE_400KHz ? 2 : 1));
		writeByte(I2C_CONFIGURE_MASTER);
		writeByte((smbusLevels ? 0x80 : 0) | (rateBits << 5) | i2cNum);
		flush();
	}

	synchronized public void i2cClose(int i2cNum) throws IOException {
		writeByte(I2C_CONFIGURE_MASTER);
		writeByte(i2cNum);
		flush();
	}

	public interface IncomingHandler {
		public void handleEstablishConnection(int hardwareId, int bootloaderId,
				int firmwareId);

		public void handleConnectionLost();

		public void handleSoftReset();
		
		public void handleCheckInterfaceResponse(boolean supported);

		public void handleSetChangeNotify(int pin, boolean changeNotify);

		public void handleReportDigitalInStatus(int pin, boolean level);

		public void handleRegisterPeriodicDigitalSampling(int pin, int freqScale);

		public void handleReportPeriodicDigitalInStatus(int frameNum,
				boolean values[]);

		public void handleAnalogPinNotify(int pin, boolean open);

		public void handleReportAnalogInStatus(int pins[], int values[]);

		public void handleUartOpen(int uartNum);

		public void handleUartClose(int uartNum);

		public void handleUartData(int uartNum, int numBytes, byte data[]);

		public void handleUartReportTxStatus(int uartNum, int bytesRemaining);

		public void handleSpiOpen(int spiNum);

		public void handleSpiClose(int spiNum);

		public void handleSpiData(int spiNum, int ssPin, byte data[],
				int dataBytes);

		public void handleSpiReportTxStatus(int spiNum, int bytesRemaining);

		public void handleI2cOpen(int i2cNum);

		public void handleI2cClose(int i2cNum);

		public void handleI2cResult(int i2cNum, int size, byte[] data);

		public void handleI2cReportTxStatus(int spiNum, int bytesRemaining);

	}

	class IncomingThread extends Thread {
		private int[] analogFramePins_ = new int[0];
		private Set<Integer> removedPins_ = new HashSet<Integer>(
				Constants.NUM_ANALOG_PINS);
		private Set<Integer> addedPins_ = new HashSet<Integer>(
				Constants.NUM_ANALOG_PINS);

		private void findDelta(int[] newPins) {
			removedPins_.clear();
			addedPins_.clear();
			for (int i : analogFramePins_) {
				removedPins_.add(i);
			}
			for (int i : newPins) {
				addedPins_.add(i);
			}
			for (Iterator<Integer> it = removedPins_.iterator(); it.hasNext();) {
				Integer current = it.next();
				if (addedPins_.contains(current)) {
					it.remove();
					addedPins_.remove(current);
				}
			}
		}

		private int readByte() throws IOException {
			try {
				int b = in_.read();
				if (b == -1)
					throw new IOException("Unexpected stream closure");
				// Log.v("IOIOProtocol", "received: 0x" +
				// Integer.toHexString(b));
				return b;
			} catch (IOException e) {
				Log.i("IOIOProtocol", "IOIO disconnected");
				throw e;
			}
		}

		private int readTwoBytes() throws IOException {
			return readByte() | (readByte() << 8);
		}

		private int readThreeBytes() throws IOException {
			return readByte() | (readByte() << 8) | (readByte() << 16);
		}

		@Override
		public void run() {
			super.run();
			setPriority(MAX_PRIORITY);
			int arg1;
			int arg2;
			int numPins;
			byte[] data = new byte[256];
			try {
				while (true) {
					switch (arg1 = readByte()) {
					case ESTABLISH_CONNECTION:
						if (readByte() != 'I' || readByte() != 'O'
								|| readByte() != 'I' || readByte() != 'O') {
							throw new IOException(
									"Bad establish connection magic");
						}
						int hardwareId = readThreeBytes();
						int bootloaderId = readThreeBytes();
						int firmwareId = readThreeBytes();

						handler_.handleEstablishConnection(hardwareId,
								bootloaderId, firmwareId);
						break;

					case SOFT_RESET:
						handler_.handleSoftReset();
						break;

					case REPORT_DIGITAL_IN_STATUS:
						arg1 = readByte();
						handler_.handleReportDigitalInStatus(arg1 >> 2,
								(arg1 & 0x01) == 1);
						break;

					case SET_CHANGE_NOTIFY:
						arg1 = readByte();
						handler_.handleSetChangeNotify(arg1 >> 2,
								(arg1 & 0x01) == 1);
						break;

					case REGISTER_PERIODIC_DIGITAL_SAMPLING:
						// TODO: implement
						break;

					case REPORT_PERIODIC_DIGITAL_IN_STATUS:
						// TODO: implement
						break;

					case REPORT_ANALOG_IN_FORMAT:
						numPins = readByte();
						int[] newFormat = new int[numPins];
						for (int i = 0; i < numPins; ++i) {
							newFormat[i] = readByte();
						}
						findDelta(newFormat);
						for (Integer i : removedPins_) {
							handler_.handleAnalogPinNotify(i, false);
						}
						for (Integer i : addedPins_) {
							handler_.handleAnalogPinNotify(i, true);
						}
						analogFramePins_ = newFormat;
						break;

					case REPORT_ANALOG_IN_STATUS:
						numPins = analogFramePins_.length;
						int header = 0;
						int[] values = new int[numPins];
						for (int i = 0; i < numPins; ++i) {
							if (i % 4 == 0) {
								header = readByte();
							}
							values[i] = (readByte() << 2) | (header & 0x03);
							header >>= 2;
						}
						handler_.handleReportAnalogInStatus(analogFramePins_,
								values);
						break;

					case UART_REPORT_TX_STATUS:
						arg1 = readByte();
						arg2 = readByte();
						handler_.handleUartReportTxStatus(arg1 & 0x03,
								(arg1 >> 2) | (arg2 << 6));
						break;

					case UART_DATA:
						arg1 = readByte();
						for (int i = 0; i < (arg1 & 0x3F) + 1; ++i) {
							data[i] = (byte) readByte();
						}
						handler_.handleUartData(arg1 >> 6, (arg1 & 0x3F) + 1,
								data);
						break;

					case UART_STATUS:
						arg1 = readByte();
						if ((arg1 & 0x80) != 0) {
							handler_.handleUartOpen(arg1 & 0x03);
						} else {
							handler_.handleUartClose(arg1 & 0x03);
						}
						break;

					case SPI_DATA:
						arg1 = readByte();
						arg2 = readByte();
						for (int i = 0; i < (arg1 & 0x3F) + 1; ++i) {
							data[i] = (byte) readByte();
						}
						handler_.handleSpiData(arg1 >> 6, arg2 & 0x3F, data,
								(arg1 & 0x3F) + 1);
						break;

					case SPI_REPORT_TX_STATUS:
						arg1 = readByte();
						arg2 = readByte();
						handler_.handleSpiReportTxStatus(arg1 & 0x03,
								(arg1 >> 2) | (arg2 << 6));
						break;

					case SPI_STATUS:
						arg1 = readByte();
						if ((arg1 & 0x80) != 0) {
							handler_.handleSpiOpen(arg1 & 0x03);
						} else {
							handler_.handleSpiClose(arg1 & 0x03);
						}
						break;

					case I2C_STATUS:
						arg1 = readByte();
						if ((arg1 & 0x80) != 0) {
							handler_.handleI2cOpen(arg1 & 0x03);
						} else {
							handler_.handleI2cClose(arg1 & 0x03);
						}
						break;

					case I2C_RESULT:
						arg1 = readByte();
						arg2 = readByte();
						if (arg2 != 0xFF) {
							for (int i = 0; i < arg2; ++i) {
								data[i] = (byte) readByte();
							}
						}
						handler_.handleI2cResult(arg1 & 0x03, arg2, data);
						break;

					case I2C_REPORT_TX_STATUS:
						arg1 = readByte();
						arg2 = readByte();
						handler_.handleI2cReportTxStatus(arg1 & 0x03,
								(arg1 >> 2) | (arg2 << 6));
						break;
						
					case CHECK_INTERFACE_RESPONSE:
						arg1 = readByte();
						handler_.handleCheckInterfaceResponse((arg1 & 0x01) == 1);
						break;

					default:
						in_.close();
						IOException e = new IOException(
								"Received unexpected command: 0x"
										+ Integer.toHexString(arg1));
						Log.e("IOIOProtocol", "Protocol error", e);
						throw e;
					}
				}
			} catch (IOException e) {
				handler_.handleConnectionLost();
			}
		}
	}

	private final InputStream in_;
	private final OutputStream out_;
	private final IncomingHandler handler_;
	private final IncomingThread thread_ = new IncomingThread();

	public IOIOProtocol(InputStream in, OutputStream out,
			IncomingHandler handler) {
		in_ = in;
		out_ = out;
		handler_ = handler;
		thread_.start();
	}
}
