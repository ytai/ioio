package ioio.lib.new_impl;

import ioio.lib.api.DigitalInputSpec;
import ioio.lib.api.DigitalOutputSpec;
import ioio.lib.api.Twi.Rate;
import ioio.lib.api.Uart;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

import android.util.Log;

public class IOIOProtocol {
	static final int HARD_RESET                        = 0x00;
	static final int ESTABLISH_CONNECTION              = 0x00;
	static final int SOFT_RESET                        = 0x01;
	static final int SET_PIN_DIGITAL_OUT               = 0x02;
	static final int SET_DIGITAL_OUT_LEVEL             = 0x03;
	static final int REPORT_DIGITAL_IN_STATUS          = 0x03;
	static final int SET_PIN_DIGITAL_IN                = 0x04;
	static final int SET_CHANGE_NOTIFY                 = 0x05;
	static final int REGISTER_PERIOD_DIGITAL_SAMPLING  = 0x06;
	static final int REPORT_PERIODIC_DIGITAL_IN_STATUS = 0x07;
	static final int SET_PIN_PWM                       = 0x08;
	static final int REPORT_ANALOG_IN_FORMAT           = 0x08;
	static final int SET_PWM_DUTY_CYCLE                = 0x09;
	static final int REPORT_ANALOG_IN_STATUS           = 0x09;
	static final int SET_PWM_PERIOD                    = 0x0A;
	static final int UART_REPORT_TX_STATUS             = 0x0A;
	static final int SET_PIN_ANALOG_IN                 = 0x0B;
	static final int UART_DATA                         = 0x0C;
	static final int UART_CONFIG                       = 0x0D;
	static final int SET_PIN_UART_RX                   = 0x0E;
	static final int SET_PIN_UART_TX                   = 0x0F;
	static final int SPI_MASTER_REQUEST                = 0x10;
	static final int SPI_DATA                          = 0x10;
	static final int SPI_REPORT_TX_STATUS              = 0x11;
	static final int SPI_CONFIGURE_MASTER              = 0x12;
	static final int SET_PIN_SPI                       = 0x13;
	static final int I2C_CONFIGURE_MASTER              = 0x14;
	static final int I2C_WRITE_READ                    = 0x15;
	static final int I2C_RESULT                        = 0x15;
	static final int I2C_REPORT_TX_STATUS              = 0x16;
	
	private void writeByte(int b) throws IOException {
		assert(b >= 0 && b < 256);
		Log.v("IOIOProtocol", "sending: 0x" + Integer.toHexString(b));
		out_.write(b);
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
	}

	synchronized public void softReset() throws IOException {
		writeByte(SOFT_RESET);
	}
	
	synchronized public void setDigitalOutLevel(int pin, boolean level) throws IOException  {
		writeByte(SET_DIGITAL_OUT_LEVEL);
		writeByte(pin << 2 | (level ? 1 : 0));
	}

	synchronized public void setPinPwm(int pin, int pwmNum) throws IOException { 
		writeByte(SET_PIN_PWM);
		writeByte(pin);
		writeByte(pwmNum);
	}

	synchronized public void setPwmDutyCycle(int pwmNum, int dutyCycle, int fraction) throws IOException { 
		writeByte(SET_PWM_DUTY_CYCLE);
		writeByte(pwmNum << 2 | fraction);
		writeTwoBytes(dutyCycle);
	}

	synchronized public void setPwmPeriod(int pwmNum, int period, boolean scale256) throws IOException {
		writeByte(SET_PWM_PERIOD);
		writeByte((pwmNum << 1) | (scale256 ? 1 : 0));
		writeTwoBytes(period);
	}

	synchronized public void spiMasterRequest(int spiNum, int ssPin, byte data[],
			int dataBytes, int totalBytes, int responseBytes) throws IOException {
		// TODO: implement
	}

	synchronized public void i2cWriteRead(int i2cNum, boolean tenBitAddr, int address,
			int writeSize, int readSize, byte[] writeData) throws IOException {
		writeByte(I2C_WRITE_READ);
		writeByte(((address >> 8) << 6) | (tenBitAddr ? 0x20 : 0x00) | i2cNum);
		writeByte(address & 0xFF);
		writeByte(writeSize);
		writeByte(readSize);
		for (int i = 0; i < writeSize; ++i) {
			writeByte(writeData[i]);
		}
	}

	synchronized public void setPinDigitalOut(int pin, boolean value, DigitalOutputSpec.Mode mode) throws IOException { 
		writeByte(SET_PIN_DIGITAL_OUT);
		writeByte((pin << 2)
				| (mode == DigitalOutputSpec.Mode.OPEN_DRAIN ? 0x01 : 0x00)
				| (value ? 0x02 : 0x00));
	}

	synchronized public void setPinDigitalIn(int pin, DigitalInputSpec.Mode mode) throws IOException {
		int pull = 0;
		if (mode == DigitalInputSpec.Mode.PULL_UP) {
			pull = 1;
		} else if (mode == DigitalInputSpec.Mode.PULL_DOWN) {
			pull = 2;
		}
		writeByte(SET_PIN_DIGITAL_IN);
		writeByte((pin << 2) | pull);
	}

	synchronized public void setChangeNotify(int pin, boolean changeNotify) throws IOException {
		writeByte(SET_CHANGE_NOTIFY);
		writeByte((pin << 2) | (changeNotify ? 0x01 : 0x00));
	}

	synchronized public void registerPeriodicDigitalSampling(int pin, int freqScale) throws IOException {
		// TODO: implement
	}

	synchronized public void setPinAnalogIn(int pin) throws IOException { 
		writeByte(SET_PIN_ANALOG_IN);
		writeByte(pin);
	}

	synchronized public void uartData(int uartNum, int numBytes, byte data[]) throws IOException {
		writeByte(UART_DATA);
		writeByte((numBytes - 1) | uartNum << 6);
		for (int i = 0; i < numBytes; ++i) {
			writeByte(data[i]);
		}
	}

	synchronized public void uartConfigure(int uartNum, int rate, boolean speed4x,
			Uart.StopBits stopbits, Uart.Parity parity) throws IOException {
		int parbits = parity == Uart.Parity.EVEN_PARITY ? 1
				: (parity == Uart.Parity.ODD_PARITY ? 2 : 0);
		writeByte(UART_CONFIG);
		writeByte((uartNum << 6) | (speed4x ? 0x08 : 0x00)
				| (stopbits == Uart.StopBits.TWO_STOP_BITS ? 0x04 : 0x00)
				| parbits);
		writeTwoBytes(rate);
	}

	synchronized public void setPinUartRx(int pin, int uartNum, boolean enable) throws IOException {
		writeByte(SET_PIN_UART_RX);
		writeByte(pin);
		writeByte((enable ? 0x80 : 0x00) | uartNum);
	}

	synchronized public void setPinUartTx(int pin, int uartNum, boolean enable) throws IOException {
		writeByte(SET_PIN_UART_TX);
		writeByte(pin);
		writeByte((enable ? 0x80 : 0x00) | uartNum);
	}

	synchronized public void spiConfigureMaster(int spiNum, int scale, int div,
			boolean sampleAtEnd, boolean clkEdge, boolean clkPol) throws IOException {
		// TODO: implement
	}

	synchronized public void setPinSpi(int pin, int mode, boolean enable, int spiNum) throws IOException {
		// TODO: implement
	}

	synchronized public void i2cConfigureMaster(int i2cNum, Rate rate, boolean smbusLevels) throws IOException {
		int rateBits = (rate == Rate.RATE_1MHz ? 3 : (rate == Rate.RATE_400KHz ? 2 : 1));
		writeByte(I2C_CONFIGURE_MASTER);
		writeByte((smbusLevels ? 0x80 : 0) | (rateBits << 5) | i2cNum);
	}
	
	synchronized public void i2cClose(int i2cNum) throws IOException {
		writeByte(I2C_CONFIGURE_MASTER);
		writeByte(i2cNum);
	}
	
	public interface IncomingHandler {
		public void handleConnectionLost();

		public void handleSoftReset();

		public void handleSetChangeNotify(int pin, boolean changeNotify);

		public void handleRegisterPeriodicDigitalSampling(int pin, int freqScale);

		public void handleUartData(int uartNum, int numBytes, byte data[]);

		public void handleUartConfigure(int uartNum, int rate, boolean speed4x,
				boolean twoStopBits, int parity);

		public void handleSpiConfigureMaster(int spiNum, int scale, int div,
				boolean sampleAtEnd, boolean clkEdge, boolean clkPol);

		public void handleI2cConfigureMaster(int i2cNum, int rate,
				boolean smbusLevels);

		public void handleI2cClose(int i2cNum);

		public void handleEstablishConnection(int hardwareId, int bootloaderId,
				int firmwareId);

		public void handleUartReportTxStatus(int uartNum, int bytesRemaining);

		public void handleSpiData(int spiNum, int ssPin, byte data[],
				int dataBytes);

		public void handleReportDigitalInStatus(int pin, boolean level);

		public void handleReportPeriodicDigitalInStatus(int frameNum,
				boolean values[]);

		public void handleReportAnalogInStatus(int pins[], int values[]);

		public void handleSpiReportTxStatus(int spiNum, int bytesRemaining);

		public void handleI2cReportTxStatus(int spiNum, int bytesRemaining);

		public void handleI2cResult(int i2cNum, int size, byte[] data);

		public void handleAnalogPinNotify(int pin, boolean open);
	}	

	class IncomingThread extends Thread {
		private int[] analogFramePins_ = new int[0];
		private Set<Integer> removedPins_ = new HashSet<Integer>(Constants.NUM_ANALOG_PINS);
		private Set<Integer> addedPins_ = new HashSet<Integer>(Constants.NUM_ANALOG_PINS);
		
		private void findDelta(int[] newPins) {
			removedPins_.clear();
			addedPins_.clear();
			for (int i: analogFramePins_) {
				removedPins_.add(i);
			}
			for (int i: newPins) {
				addedPins_.add(i);
			}
			for (Iterator<Integer> it = removedPins_.iterator(); it.hasNext(); ) {
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
				if (b == -1) throw new IOException("Unexpected stream closure");
				Log.v("IOIOProtocol", "received: 0x" + Integer.toHexString(b));
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
			int b;
			int tmp;
			byte[] data = new byte[256];
			try {
				while (true) {
					switch (b = readByte()) {
					case ESTABLISH_CONNECTION:
						if (readByte() != 'I'
							|| readByte() != 'O'
							|| readByte() != 'I'
							|| readByte() != 'O') {
							throw new IOException("Bad establish connection magic");
						}
						int hardwareId = readThreeBytes();
						int bootloaderId = readThreeBytes();
						int firmwareId = readThreeBytes();
						
						handler_.handleEstablishConnection(hardwareId, bootloaderId, firmwareId);
						break;
						
					case SOFT_RESET:
						handler_.handleSoftReset();
						break;
						
					case SET_PIN_DIGITAL_OUT:
						readByte();
						break;
						
					case REPORT_DIGITAL_IN_STATUS:
						tmp = readByte();
						handler_.handleReportDigitalInStatus(tmp >> 2, (tmp & 0x01) == 1);
						break;
						
					case SET_PIN_DIGITAL_IN:
						readByte();
						break;
						
					case SET_CHANGE_NOTIFY:
						tmp = readByte();
						handler_.handleSetChangeNotify(tmp >> 2, (tmp & 0x01) == 1);
						break;
						
					case REGISTER_PERIOD_DIGITAL_SAMPLING:
						// TODO: implement
						break;
						
					case REPORT_PERIODIC_DIGITAL_IN_STATUS:
						// TODO: implement
						break;
						
					case REPORT_ANALOG_IN_FORMAT:
						int numPins = readByte();
						int[] newFormat = new int[numPins];
						for (int i = 0; i < numPins; ++i) {
							newFormat[i] = readByte();
						}
						findDelta(newFormat);
						for (Integer i: removedPins_) {
							handler_.handleAnalogPinNotify(i, false);
						}
						for (Integer i: addedPins_) {
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
						handler_.handleReportAnalogInStatus(analogFramePins_, values);
						break;
						
					case UART_REPORT_TX_STATUS:
						b = readByte();
						tmp = readByte();
						handler_.handleUartReportTxStatus(b & 0x03, (b >> 2) | (tmp << 8));
						break;
						
					case SET_PIN_ANALOG_IN:
						readByte();
						break;
						
					case UART_DATA:
						tmp = readByte();
						for (int i = 0; i < (tmp & 0x3F) + 1; ++i) {
							data[i] = (byte) readByte();
						}
						handler_.handleUartData(tmp >> 6, (tmp & 0x3F) + 1, data);
						break;
						
					case UART_CONFIG:
						tmp = readByte();
						handler_.handleUartConfigure(tmp >> 6, readTwoBytes(),
								(tmp & 0x08) != 0, (tmp & 0x04) != 0,
								tmp & 0x03);
						break;
						
					case SET_PIN_UART_RX:
						readTwoBytes();
						break;
						
					case SET_PIN_UART_TX:
						readTwoBytes();
						break;
						
					case SPI_DATA:
						// TODO: implement
						break;

					case SPI_REPORT_TX_STATUS:
						// TODO: implement
						break;
						
					case SPI_CONFIGURE_MASTER:
						// TODO: implement
						break;
						
					case SET_PIN_SPI:
						// TODO: implement
						break;
						
					case I2C_CONFIGURE_MASTER:
						b = readByte();
						if (((b >> 5) & 0x03) != 0) {
							handler_.handleI2cConfigureMaster(b & 0x03, (b >> 5) & 0x03, (b >> 7) == 1);
						} else {
							handler_.handleI2cClose(b & 0x03);
						}
						break;
						
					case I2C_RESULT:
						b = readByte();
						tmp = readByte();
						if (tmp != 0xFF) {
							for (int i = 0; i < tmp; ++i) {
								data[i] = (byte) readByte();
							}
						}
						handler_.handleI2cResult(b & 0x03, tmp, data);
						break;
						
					case I2C_REPORT_TX_STATUS:
						b = readByte();
						tmp = readByte();
						handler_.handleI2cReportTxStatus(b & 0x03, (b >> 2) | (tmp << 8));
						break;
						
					default:
						in_.close();
						IOException e = new IOException("Received unexpected command: 0x" + Integer.toHexString(b)); 
						Log.e("IOIOProtocol", "Protocol error", e);
						throw e;
					}
				}
			} catch (IOException e) {
				handler_.handleConnectionLost();
			}
		}
	}

	private InputStream in_;
	private OutputStream out_;
	private IncomingHandler handler_;
	private IncomingThread thread_ = new IncomingThread();
	
	public IOIOProtocol(InputStream in, OutputStream out, IncomingHandler handler) {
		in_ = in;
		out_ = out;
		handler_ = handler;
		thread_.start();
	}
}
