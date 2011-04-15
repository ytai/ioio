package ioio.lib.impl;

import java.util.HashSet;
import java.util.LinkedList;
import java.util.Queue;
import java.util.Set;

import android.util.Log;

import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IOIOProtocol.IncomingHandler;

public class IncomingState implements IncomingHandler {
	enum ConnectionState {
		INIT,
		CONNECTED,
		DISCONNECTED
	}
	
	interface InputPinListener {
		void setValue(int value);
	}
	
	interface DisconnectListener {
		void disconnected();
	}
	
	interface DataModuleListener {
		void dataReceived(byte[] data, int size);
		void reportBufferRemaining(int bytesRemaining);
	}
	
	class InputPinState {
		private Queue<InputPinListener> listeners_ = new LinkedList<InputPinListener>();
		private boolean currentOpen_ = false;
		
		void pushListener(InputPinListener listener) {
			listeners_.add(listener);
		}
		
		void closeCurrentListener() {
			if (currentOpen_) {
				currentOpen_ = false;
				listeners_.remove();
			}
		}
		
		void openNextListener() {
			assert(!listeners_.isEmpty());
			if (!currentOpen_) {
				currentOpen_ = true;
			}
		}
		
		void setValue(int v) {
			assert(currentOpen_);
			listeners_.peek().setValue(v);
		}
	}
	
	class DataModuleState {
		private Queue<DataModuleListener> listeners_ = new LinkedList<IncomingState.DataModuleListener>();
		private boolean currentOpen_ = false;
		
		void pushListener(DataModuleListener listener) {
			listeners_.add(listener);
		}
		
		void closeCurrentListener() {
			if (currentOpen_) {
				currentOpen_ = false;
				listeners_.remove();
			}
		}
		
		void openNextListener() {
			assert(!listeners_.isEmpty());
			if (!currentOpen_) {
				currentOpen_ = true;
			}
		}
		
		void dataReceived(byte[] data, int size) {
			assert(currentOpen_);
			listeners_.peek().dataReceived(data, size);
		}

		public void reportBufferRemaining(int bytesRemaining) {
			assert(currentOpen_);
			listeners_.peek().reportBufferRemaining(bytesRemaining);
		}
	}
	
	private InputPinState[] intputPinStates_ = new InputPinState[Constants.NUM_PINS];
	private DataModuleState[] uartStates_ = new DataModuleState[Constants.NUM_UART_MODULES];
	private DataModuleState[] twiStates_ = new DataModuleState[Constants.NUM_TWI_MODULES];
	private DataModuleState[] spiStates_ = new DataModuleState[Constants.NUM_SPI_MODULES];
	private ConnectionState connection_ = ConnectionState.INIT;
	private Set<DisconnectListener> disconnectListeners_ = new HashSet<IncomingState.DisconnectListener>();
	
	public IncomingState() {
		for (int i = 0; i < intputPinStates_.length; ++i) {
			intputPinStates_[i] = new InputPinState();
		}
		for (int i = 0; i < uartStates_.length; ++i) {
			uartStates_[i] = new DataModuleState();
		}
		for (int i = 0; i < twiStates_.length; ++i) {
			twiStates_[i] = new DataModuleState();
		}
		for (int i = 0; i < spiStates_.length; ++i) {
			spiStates_[i] = new DataModuleState();
		}
	}
	
	synchronized public void waitConnect() throws InterruptedException, ConnectionLostException {
		while (connection_ == ConnectionState.INIT) {
			wait();
		}
		if (connection_ == ConnectionState.DISCONNECTED) {
			throw new ConnectionLostException();
		}
	}
	
	synchronized public void waitDisconnect() throws InterruptedException {
		while (connection_ != ConnectionState.DISCONNECTED) {
			wait();
		}
	}
	
	public void addInputPinListener(int pin, InputPinListener listener) {
		intputPinStates_[pin].pushListener(listener);
	}
	
	public void addUartListener(int uartNum, DataModuleListener listener) {
		uartStates_[uartNum].pushListener(listener);
	}
	
	public void addTwiListener(int twiNum, DataModuleListener listener) {
		twiStates_[twiNum].pushListener(listener);
	}
	
	public void addSpiListener(int spiNum, DataModuleListener listener) {
		spiStates_[spiNum].pushListener(listener);
	}
	
	synchronized public void addDisconnectListener(DisconnectListener listener) throws ConnectionLostException {
		checkConnected();
		disconnectListeners_.add(listener);
	}
	
	synchronized public void removeDisconnectListener(DisconnectListener listener) {
		disconnectListeners_.remove(listener);
	}
	
	@Override
	synchronized public void handleConnectionLost() {
		logMethod("handleConnectionLost");
		for (DisconnectListener listener: disconnectListeners_) {
			listener.disconnected();
		}
		disconnectListeners_.clear();
		connection_ = ConnectionState.DISCONNECTED;
		notifyAll();
	}

	@Override
	public void handleSoftReset() {
		logMethod("handleSoftReset");
		for (InputPinState pinState: intputPinStates_) {
			pinState.closeCurrentListener();
		}
		for (DataModuleState uartState: uartStates_) {
			uartState.closeCurrentListener();
		}
		for (DataModuleState twiState: twiStates_) {
			twiState.closeCurrentListener();
		}
		for (DataModuleState spiState: spiStates_) {
			spiState.closeCurrentListener();
		}
	}

	@Override
	public void handleSetChangeNotify(int pin, boolean changeNotify) {
		logMethod("handleSetChangeNotify", pin, changeNotify);
		if (changeNotify) {
			intputPinStates_[pin].openNextListener();
		} else {
			intputPinStates_[pin].closeCurrentListener();
		}
	}

	@Override
	public void handleRegisterPeriodicDigitalSampling(int pin, int freqScale) {
		logMethod("handleRegisterPeriodicDigitalSampling", pin, freqScale);
		assert(false);
	}

	@Override
	public void handleAnalogPinNotify(int pin, boolean open) {
		logMethod("handleAnalogPinStatus", pin, open);
		if (open) {
			intputPinStates_[pin].openNextListener();
		} else {
			intputPinStates_[pin].closeCurrentListener();
		}
	}
	
	@Override
	public void handleUartData(int uartNum, int numBytes, byte[] data) {
		logMethod("handleUartData", uartNum, numBytes, data);
		uartStates_[uartNum].dataReceived(data, numBytes);
	}

	@Override
	public void handleUartOpen(int uartNum) {
		logMethod("handleUartOpen", uartNum);
		uartStates_[uartNum].openNextListener();
	}

	@Override
	public void handleUartClose(int uartNum) {
		logMethod("handleUartClose", uartNum);
		uartStates_[uartNum].closeCurrentListener();
	}

	@Override
	public void handleSpiOpen(int spiNum) {
		logMethod("handleSpiConfigureMaster", spiNum);
		spiStates_[spiNum].openNextListener();
	}
	

	@Override
	public void handleSpiClose(int spiNum) {
		logMethod("handleSpiClose", spiNum);
		spiStates_[spiNum].closeCurrentListener();
	}

	@Override
	public void handleI2cOpen(int i2cNum) {
		logMethod("handleI2cConfigureMaster", i2cNum);
		twiStates_[i2cNum].openNextListener();
	}

	@Override
	public void handleI2cClose(int i2cNum) {
		logMethod("handleI2cClose", i2cNum);
		twiStates_[i2cNum].closeCurrentListener();
	}

	@Override
	public void handleEstablishConnection(int hardwareId, int bootloaderId,
			int firmwareId) {
		logMethod("handleEstablishConnection", hardwareId, bootloaderId, firmwareId);
		// TODO: check versions, close on failure
		synchronized(this) {
			connection_ = ConnectionState.CONNECTED;
			notifyAll();
		}
	}

	@Override
	public void handleUartReportTxStatus(int uartNum, int bytesRemaining) {
		logMethod("handleUartReportTxStatus", uartNum, bytesRemaining);
		uartStates_[uartNum].reportBufferRemaining(bytesRemaining);
	}

	@Override
	public void handleI2cReportTxStatus(int i2cNum, int bytesRemaining) {
		logMethod("handleI2cReportTxStatus", i2cNum, bytesRemaining);
		twiStates_[i2cNum].reportBufferRemaining(bytesRemaining);
	}

	@Override
	public void handleSpiData(int spiNum, int ssPin, byte[] data, int dataBytes) {
		logMethod("handleSpiData", spiNum, ssPin, data, dataBytes);
		spiStates_[spiNum].dataReceived(data, dataBytes);
	}

	@Override
	public void handleReportDigitalInStatus(int pin, boolean level) {
		logMethod("handleReportDigitalInStatus", pin, level);
		intputPinStates_[pin].setValue(level ? 1 : 0);
	}

	@Override
	public void handleReportPeriodicDigitalInStatus(int frameNum,
			boolean[] values) {
		logMethod("handleReportPeriodicDigitalInStatus", frameNum, values);
		// TODO Auto-generated method stub
	}

	@Override
	public void handleReportAnalogInStatus(int pins[], int values[]) {
		logMethod("handleReportAnalogInStatus", pins, values);
		for (int i = 0; i < pins.length; ++i) {
			intputPinStates_[pins[i]].setValue(values[i]);
		}		
	}

	@Override
	public void handleSpiReportTxStatus(int spiNum, int bytesRemaining) {
		logMethod("handleSpiReportTxStatus", spiNum, bytesRemaining);
		spiStates_[spiNum].reportBufferRemaining(bytesRemaining);
	}

	@Override
	public void handleI2cResult(int i2cNum, int size, byte[] data) {
		logMethod("handleI2cResult", i2cNum, size, data);
		twiStates_[i2cNum].dataReceived(data, size);
	}
	
	private void checkConnected() throws ConnectionLostException {
		if (connection_ != ConnectionState.CONNECTED) {
			throw new ConnectionLostException();
		}
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
		
		Log.v("IncomingState", msg.toString());
	}
}
