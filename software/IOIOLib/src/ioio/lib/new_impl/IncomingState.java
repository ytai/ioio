package ioio.lib.new_impl;

import java.util.LinkedList;
import java.util.Queue;

import android.util.Log;

import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.new_impl.IOIOProtocol.IncomingHandler;

public class IncomingState implements IncomingHandler {
	enum PinMode {
		DIGITAL_IN,
		DIGITAL_OUT,
		ANALOG_IN,
		PWM,
		UART,
		SPI,
		I2C
	}
	
	enum ConnectionState {
		INIT,
		OPEN,
		CLOSED
	}
	
	interface Listener {
		void opened(PinMode mode);
		void closed();
		void lost();
		void setValue(int value);
	}
	
	class PinState {
		private Queue<Listener> listeners_ = new LinkedList<Listener>();
		private boolean currentOpen_ = false;
		
		void pushListener(Listener listener) {
			listeners_.add(listener);
		}
		
		void closeCurrentListener() {
			assert(!listeners_.isEmpty());
			if (currentOpen_) {
				currentOpen_ = false;
				Listener current = listeners_.remove();
				current.closed();
			}
		}
		
		void openNextListener(PinMode mode) {
			assert(!listeners_.isEmpty());
			if (!currentOpen_) {
				currentOpen_ = true;
				listeners_.peek().opened(mode);
			}
		}
		
		void forceCloseAll() {
			while (!listeners_.isEmpty()) {
				listeners_.remove().lost();
			}
		}
		
		void setValue(int v) {
			assert(currentOpen_);
			listeners_.peek().setValue(v);
		}
	}
	
	private PinState[] pinStates_ = new PinState[Constants.NUM_PINS];
	private ConnectionState connection_ = ConnectionState.INIT;
	
	public IncomingState() {
		for (int i = 0; i < pinStates_.length; ++i) {
			pinStates_[i] = new PinState();
		}
	}
	
	synchronized public void waitConnect() throws InterruptedException, ConnectionLostException {
		while (connection_ == ConnectionState.INIT) {
			wait();
		}
		if (connection_ == ConnectionState.CLOSED) {
			throw new ConnectionLostException("Failed to connect");
		}
	}
	
	synchronized public void waitDisconnect() throws InterruptedException {
		while (connection_ != ConnectionState.CLOSED) {
			wait();
		}
	}
	
	public void addPinListener(int pin, Listener listener) {
		pinStates_[pin].pushListener(listener);
	}
	
	@Override
	synchronized public void handleConnectionLost() {
		logMethod("handleConnectionLost");
		for (PinState pinState: pinStates_) {
			pinState.forceCloseAll();
		}
		connection_ = ConnectionState.CLOSED;
		notifyAll();
	}

	@Override
	public void handleSoftReset() {
		logMethod("handleSoftReset");
		for (PinState pinState: pinStates_) {
			pinState.closeCurrentListener();
		}
	}

	@Override
	public void handleSetPinDigitalOut(int pin, boolean value, boolean openDrain) {
		logMethod("handleSetPinDigitalOut", pin, value, openDrain);
		pinStates_[pin].openNextListener(PinMode.DIGITAL_OUT);
	}

	@Override
	public void handleSetPinDigitalIn(int pin, int pull) {
		logMethod("handleSetPinDigitalIn", pin, pull);
		pinStates_[pin].closeCurrentListener();
	}

	@Override
	public void handleSetChangeNotify(int pin, boolean changeNotify) {
		logMethod("handleSetChangeNotify", pin, changeNotify);
		if (changeNotify) {
			pinStates_[pin].openNextListener(PinMode.DIGITAL_IN);
		}
	}

	@Override
	public void handleRegisterPeriodicDigitalSampling(int pin, int freqScale) {
		logMethod("handleRegisterPeriodicDigitalSampling", pin, freqScale);
		assert(false);
	}

	@Override
	public void handleSetPinAnalogIn(int pin) {
		logMethod("handleSetPinAnalogIn", pin);
		pinStates_[pin].openNextListener(PinMode.ANALOG_IN);
	}

	@Override
	public void handleUartData(int uartNum, int numBytes, byte[] data) {
		logMethod("handleUartData", uartNum, numBytes, data);
		// TODO Auto-generated method stub
		
	}

	@Override
	public void handleUartConfigure(int uartNum, int rate, boolean speed4x,
			boolean twoStopBits, int parity) {
		logMethod("handleUartConfigure", uartNum, rate, speed4x, twoStopBits, parity);
		// TODO Auto-generated method stub
		
	}

	@Override
	public void handleSetPinUartRx(int pin, int uartNum, boolean enable) {
		logMethod("handleSetPinUartRx", pin, uartNum, enable);
		// TODO Auto-generated method stub
		
	}

	@Override
	public void handleSetPinUartTx(int pin, int uartNum, boolean enable) {
		logMethod("handleSetPinUartTx", pin, uartNum, enable);
		// TODO Auto-generated method stub
		
	}

	@Override
	public void handleSpiConfigureMaster(int spiNum, int scale, int div,
			boolean sampleAtEnd, boolean clkEdge, boolean clkPol) {
		logMethod("handleSpiConfigureMaster", spiNum, scale, div, sampleAtEnd, clkEdge, clkPol);
		// TODO Auto-generated method stub
		
	}

	@Override
	public void handleSetPinSpi(int pin, int mode, boolean enable, int spiNum) {
		logMethod("handleSetPinSpi", pin, mode, enable, spiNum);
		// TODO Auto-generated method stub
		
	}

	@Override
	public void handleI2cConfigureMaster(int i2cNum, int rate,
			boolean smbusLevels) {
		logMethod("handleI2cConfigureMaster", i2cNum, rate, smbusLevels);
		// TODO Auto-generated method stub
		
	}

	@Override
	public void handleEstablishConnection(int hardwareId, int bootloaderId,
			int firmwareId) {
		logMethod("handleEstablishConnection", hardwareId, bootloaderId, firmwareId);
		// TODO: check versions, close on failure
		synchronized(this) {
			connection_ = ConnectionState.OPEN;
			notifyAll();
		}
	}

	@Override
	public void handleUartReportTxStatus(int uartNum, int bytesRemaining) {
		logMethod("handleUartReportTxStatus", uartNum, bytesRemaining);
		// TODO Auto-generated method stub
	}

	@Override
	public void handleSpiData(int spiNum, int ssPin, byte[] data, int dataBytes) {
		logMethod("handleSpiData", spiNum, ssPin, data, dataBytes);
		// TODO Auto-generated method stub
	}

	@Override
	public void handleReportDigitalInStatus(int pin, boolean level) {
		logMethod("handleReportDigitalInStatus", pin, level);
		pinStates_[pin].setValue(level ? 1 : 0);
	}

	@Override
	public void handleReportPeriodicDigitalInStatus(int frameNum,
			boolean[] values) {
		logMethod("handleReportPeriodicDigitalInStatus", frameNum, values);
		// TODO Auto-generated method stub
	}

	@Override
	public void handleReportAnalogInFormat(int numPins, int[] pins) {
		logMethod("handleReportAnalogInFormat", numPins, pins);
		// TODO Auto-generated method stub
	}

	@Override
	public void handleReportAnalogInStatus(int[] values) {
		logMethod("handleReportAnalogInStatus", values);
		// TODO Auto-generated method stub
		
	}

	@Override
	public void handleSpiReportTxStatus(int spiNum, int bytesRemaining) {
		logMethod("handleSpiReportTxStatus", spiNum, bytesRemaining);
		// TODO Auto-generated method stub
		
	}

	@Override
	public void handleI2cResult(int i2cNum, int size, byte[] data) {
		logMethod("handleI2cResult", i2cNum, size, data);
		// TODO Auto-generated method stub
		
	}
	
	private void logMethod(String name, Object... args) {
		StringBuffer msg = new StringBuffer(name);
		msg.append('(');
		for (int i = 0; i < args.length; ++i) {
			if (i != 0) {
				msg.append(", ");
			}
			msg.append(args[i]);
		}
		msg.append(')');
		
		Log.i("IncomingState", msg.toString());
	}
}
