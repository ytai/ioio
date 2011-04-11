package ioio.lib.new_impl;

import ioio.lib.api.DigitalInputSpec;
import ioio.lib.api.DigitalOutputSpec;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.graphics.Path.Direction;
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
		Log.i("IOIOProtocol", "sending: 0x" + Integer.toHexString(b));
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
			int dataBytes, int totalBytes, int responseBytes) throws IOException { assert(false); }

	synchronized public void i2cWriteRead(int i2cNum, boolean tenBitAddr, int address,
			int writeSize, int readSize, byte[] writeData) throws IOException { assert(false); }


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

	synchronized public void registerPeriodicDigitalSampling(int pin, int freqScale) throws IOException { assert(false); }

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
			boolean twoStopBits, int parity) throws IOException {
		writeByte(UART_CONFIG);
		writeByte((uartNum << 6) | (speed4x ? 0x08 : 0x00)
				| (twoStopBits ? 0x04 : 0x00) | parity);
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
			boolean sampleAtEnd, boolean clkEdge, boolean clkPol) throws IOException { assert(false); }

	synchronized public void setPinSpi(int pin, int mode, boolean enable, int spiNum) throws IOException { assert(false); }

	synchronized public void i2cConfigureMaster(int i2cNum, int rate, boolean smbusLevels) throws IOException { assert(false); }
	
	public interface IncomingHandler {
		public void handleConnectionLost();
		
		public void handleSoftReset();

		public void handleSetPinDigitalOut(int pin, boolean value, boolean openDrain);

		public void handleSetPinDigitalIn(int pin, int pull);

		public void handleSetChangeNotify(int pin, boolean changeNotify);

		public void handleRegisterPeriodicDigitalSampling(int pin, int freqScale);

		public void handleSetPinAnalogIn(int pin);

		public void handleUartData(int uartNum, int numBytes, byte data[]);

		public void handleUartConfigure(int uartNum, int rate, boolean speed4x,
				boolean twoStopBits, int parity);

		public void handleSetPinUartRx(int pin, int uartNum, boolean enable);

		public void handleSetPinUartTx(int pin, int uartNum, boolean enable);

		public void handleSpiConfigureMaster(int spiNum, int scale, int div,
				boolean sampleAtEnd, boolean clkEdge, boolean clkPol);

		public void handleSetPinSpi(int pin, int mode, boolean enable, int spiNum);

		public void handleI2cConfigureMaster(int i2cNum, int rate, boolean smbusLevels);

		public void handleEstablishConnection(int hardwareId, int bootloaderId,
				int firmwareId);

		public void handleUartReportTxStatus(int uartNum, int bytesRemaining);

		public void handleSpiData(int spiNum, int ssPin, byte data[], int dataBytes);

		public void handleReportDigitalInStatus(int pin, boolean level);

		public void handleReportPeriodicDigitalInStatus(int frameNum,
				boolean values[]);

		public void handleReportAnalogInStatus(int pins[], int values[]);

		public void handleSpiReportTxStatus(int spiNum, int bytesRemaining);

		public void handleI2cResult(int i2cNum, int size, byte[] data);
	}
	

	class IncomingThread extends Thread {
		private int[] analogFramePins_;
		
		private int readByte() throws IOException {
			int b = in_.read();
			if (b == -1) throw new IOException("Unexpected stream closure");
			Log.i("IOIOProtocol", "received: 0x" + Integer.toHexString(b));
			return b;
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
			byte[] data = new byte[64];
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
						tmp = readByte();
						handler_.handleSetPinDigitalOut(tmp >> 2, ((tmp >> 1) & 0x01) == 1, (tmp & 0x01) == 1);
						break;
						
					case REPORT_DIGITAL_IN_STATUS:
						tmp = readByte();
						handler_.handleReportDigitalInStatus(tmp >> 2, (tmp & 0x01) == 1);
						break;
						
					case SET_PIN_DIGITAL_IN:
						tmp = readByte();
						handler_.handleSetPinDigitalIn(tmp >> 2, tmp & 0x03);
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
						analogFramePins_ = new int[numPins];
						for (int i = 0; i < numPins; ++i) {
							analogFramePins_[i] = readByte();
						}
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
						handler_.handleSetPinAnalogIn(readByte());
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
						b = readByte();
						tmp = readByte();
						handler_.handleSetPinUartRx(b, tmp & 0x03, (tmp & 0x80) != 0);
						break;
						
					case SET_PIN_UART_TX:
						b = readByte();
						tmp = readByte();
						handler_.handleSetPinUartTx(b, tmp & 0x03, (tmp & 0x80) != 0);
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
						// TODO: implement
						break;
						
					case I2C_RESULT:
						// TODO: implement
						break;
						
					case I2C_REPORT_TX_STATUS:
						// TODO: implement
						break;
						
					default:
						in_.close();
						throw new IOException("Received unexpected command: 0x" + Integer.toHexString(b));
					}
				}
			} catch (IOException e) {
				Log.i("IOIOProtocol", e.getMessage());
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
	
	public void close() {
		Log.i("IOIOProtocol", "Client initiated close");
		try {
			in_.close();
		} catch (IOException e) {
		}
		try {
			out_.close();
		} catch (IOException e) {
		}
		try {
			thread_.join();
		} catch (InterruptedException e) {
		}
	}
}
