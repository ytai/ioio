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

import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IOIOProtocol.IncomingHandler;
import ioio.lib.spi.Log;

import java.util.HashSet;
import java.util.List;
import java.util.Queue;
import java.util.Set;
import java.util.concurrent.ConcurrentLinkedQueue;

class IncomingState implements IncomingHandler {
	enum ConnectionState {
		INIT, ESTABLISHED, CONNECTED, DISCONNECTED, UNSUPPORTED_IID
	}

	interface InputPinListener {
		void setValue(int value);
	}

	interface DisconnectListener {
		void disconnected();
	}

	interface DataModuleListener {
		void dataReceived(byte[] data, int size);

		void reportAdditionalBuffer(int bytesToAdd);
	}

	class InputPinState {
		private Queue<InputPinListener> listeners_ = new ConcurrentLinkedQueue<InputPinListener>();
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
			assert (!listeners_.isEmpty());
			if (!currentOpen_) {
				currentOpen_ = true;
			}
		}

		void setValue(int v) {
			assert (currentOpen_);
			listeners_.peek().setValue(v);
		}
	}

	class DataModuleState {
		private Queue<DataModuleListener> listeners_ = new ConcurrentLinkedQueue<IncomingState.DataModuleListener>();
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
			assert (!listeners_.isEmpty());
			if (!currentOpen_) {
				currentOpen_ = true;
			}
		}

		void dataReceived(byte[] data, int size) {
			assert (currentOpen_);
			listeners_.peek().dataReceived(data, size);
		}

		public void reportAdditionalBuffer(int bytesRemaining) {
			assert (currentOpen_);
			listeners_.peek().reportAdditionalBuffer(bytesRemaining);
		}
	}

	private final InputPinState[] intputPinStates_ = new InputPinState[Constants.NUM_PINS];
	private final DataModuleState[] uartStates_ = new DataModuleState[Constants.NUM_UART_MODULES];
	private final DataModuleState[] twiStates_ = new DataModuleState[Constants.NUM_TWI_MODULES];
	private final DataModuleState[] spiStates_ = new DataModuleState[Constants.NUM_SPI_MODULES];
	private final DataModuleState[] incapStates_ = new DataModuleState[2
			* Constants.INCAP_MODULES_DOUBLE.length
			+ Constants.INCAP_MODULES_SINGLE.length];
	private final DataModuleState icspState_ = new DataModuleState();
	private final Set<DisconnectListener> disconnectListeners_ = new HashSet<IncomingState.DisconnectListener>();
	private ConnectionState connection_ = ConnectionState.INIT;
	public String hardwareId_;
	public String bootloaderId_;
	public String firmwareId_;

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
		for (int i = 0; i < incapStates_.length; ++i) {
			incapStates_[i] = new DataModuleState();
		}
	}

	synchronized public void waitConnectionEstablished()
			throws InterruptedException, ConnectionLostException {
		while (connection_ == ConnectionState.INIT) {
			wait();
		}
		if (connection_ == ConnectionState.DISCONNECTED) {
			throw new ConnectionLostException();
		}
	}

	synchronized public boolean waitForInterfaceSupport()
			throws InterruptedException, ConnectionLostException {
		if (connection_ == ConnectionState.INIT) {
			throw new IllegalStateException(
					"Have to connect before waiting for interface support");
		}
		while (connection_ == ConnectionState.ESTABLISHED) {
			wait();
		}
		if (connection_ == ConnectionState.DISCONNECTED) {
			throw new ConnectionLostException();
		}
		return connection_ == ConnectionState.CONNECTED;
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

	public void addIncapListener(int incapNum, DataModuleListener listener) {
		incapStates_[incapNum].pushListener(listener);
	}

	public void addIcspListener(DataModuleListener listener) {
		icspState_.pushListener(listener);
	}

	public void addSpiListener(int spiNum, DataModuleListener listener) {
		spiStates_[spiNum].pushListener(listener);
	}

	synchronized public void addDisconnectListener(DisconnectListener listener)
			throws ConnectionLostException {
		checkNotDisconnected();
		disconnectListeners_.add(listener);
	}

	synchronized public void removeDisconnectListener(
			DisconnectListener listener) {
		if (connection_ != ConnectionState.DISCONNECTED) {
			disconnectListeners_.remove(listener);
		}
	}

	@Override
	public void handleConnectionLost() {
		// logMethod("handleConnectionLost");
		synchronized (this) {
			connection_ = ConnectionState.DISCONNECTED;
		}
		for (DisconnectListener listener : disconnectListeners_) {
			listener.disconnected();
		}
		disconnectListeners_.clear();
		synchronized (this) {
			notifyAll();
		}
	}

	@Override
	public void handleSoftReset() {
		// logMethod("handleSoftReset");
		for (InputPinState pinState : intputPinStates_) {
			pinState.closeCurrentListener();
		}
		for (DataModuleState uartState : uartStates_) {
			uartState.closeCurrentListener();
		}
		for (DataModuleState twiState : twiStates_) {
			twiState.closeCurrentListener();
		}
		for (DataModuleState spiState : spiStates_) {
			spiState.closeCurrentListener();
		}
		for (DataModuleState incapState : incapStates_) {
			incapState.closeCurrentListener();
		}
		icspState_.closeCurrentListener();
	}

	@Override
	synchronized public void handleCheckInterfaceResponse(boolean supported) {
		// logMethod("handleCheckInterfaceResponse", supported);
		connection_ = supported ? ConnectionState.CONNECTED
				: ConnectionState.UNSUPPORTED_IID;
		notifyAll();
	}

	@Override
	public void handleSetChangeNotify(int pin, boolean changeNotify) {
		// logMethod("handleSetChangeNotify", pin, changeNotify);
		if (changeNotify) {
			intputPinStates_[pin].openNextListener();
		} else {
			intputPinStates_[pin].closeCurrentListener();
		}
	}

	@Override
	public void handleRegisterPeriodicDigitalSampling(int pin, int freqScale) {
		// logMethod("handleRegisterPeriodicDigitalSampling", pin, freqScale);
		assert (false);
	}

	@Override
	public void handleAnalogPinStatus(int pin, boolean open) {
		// logMethod("handleAnalogPinStatus", pin, open);
		if (open) {
			intputPinStates_[pin].openNextListener();
		} else {
			intputPinStates_[pin].closeCurrentListener();
		}
	}

	@Override
	public void handleUartData(int uartNum, int numBytes, byte[] data) {
		// logMethod("handleUartData", uartNum, numBytes, data);
		uartStates_[uartNum].dataReceived(data, numBytes);
	}

	@Override
	public void handleUartOpen(int uartNum) {
		// logMethod("handleUartOpen", uartNum);
		uartStates_[uartNum].openNextListener();
	}

	@Override
	public void handleUartClose(int uartNum) {
		// logMethod("handleUartClose", uartNum);
		uartStates_[uartNum].closeCurrentListener();
	}

	@Override
	public void handleSpiOpen(int spiNum) {
		// logMethod("handleSpiOpen", spiNum);
		spiStates_[spiNum].openNextListener();
	}

	@Override
	public void handleSpiClose(int spiNum) {
		// logMethod("handleSpiClose", spiNum);
		spiStates_[spiNum].closeCurrentListener();
	}

	@Override
	public void handleI2cOpen(int i2cNum) {
		// logMethod("handleI2cOpen", i2cNum);
		twiStates_[i2cNum].openNextListener();
	}

	@Override
	public void handleI2cClose(int i2cNum) {
		// logMethod("handleI2cClose", i2cNum);
		twiStates_[i2cNum].closeCurrentListener();
	}

	@Override
	public void handleIcspOpen() {
		// logMethod("handleIcspOpen");
		icspState_.openNextListener();
	}

	@Override
	public void handleIcspClose() {
		// logMethod("handleIcspClose");
		icspState_.closeCurrentListener();
	}

	@Override
	public void handleEstablishConnection(byte[] hardwareId,
			byte[] bootloaderId, byte[] firmwareId) {
		hardwareId_ = new String(hardwareId);
		bootloaderId_ = new String(bootloaderId);
		firmwareId_ = new String(firmwareId);

		Log.i("IncomingState", "IOIO Connection established. Hardware ID: "
				+ hardwareId_ + " Bootloader ID: " + bootloaderId_
				+ " Firmware ID: " + firmwareId_);
		synchronized (this) {
			connection_ = ConnectionState.ESTABLISHED;
			notifyAll();
		}
	}

	@Override
	public void handleUartReportTxStatus(int uartNum, int bytesRemaining) {
		// logMethod("handleUartReportTxStatus", uartNum, bytesRemaining);
		uartStates_[uartNum].reportAdditionalBuffer(bytesRemaining);
	}

	@Override
	public void handleI2cReportTxStatus(int i2cNum, int bytesRemaining) {
		// logMethod("handleI2cReportTxStatus", i2cNum, bytesRemaining);
		twiStates_[i2cNum].reportAdditionalBuffer(bytesRemaining);
	}

	@Override
	public void handleSpiData(int spiNum, int ssPin, byte[] data, int dataBytes) {
		// logMethod("handleSpiData", spiNum, ssPin, data, dataBytes);
		spiStates_[spiNum].dataReceived(data, dataBytes);
	}

	@Override
	public void handleIcspReportRxStatus(int bytesRemaining) {
		// logMethod("handleIcspReportRxStatus", bytesRemaining);
		icspState_.reportAdditionalBuffer(bytesRemaining);
	}

	@Override
	public void handleReportDigitalInStatus(int pin, boolean level) {
		// logMethod("handleReportDigitalInStatus", pin, level);
		intputPinStates_[pin].setValue(level ? 1 : 0);
	}

	@Override
	public void handleReportPeriodicDigitalInStatus(int frameNum,
			boolean[] values) {
		// logMethod("handleReportPeriodicDigitalInStatus", frameNum, values);
	}

	@Override
	public void handleReportAnalogInStatus(List<Integer> pins,
			List<Integer> values) {
		// logMethod("handleReportAnalogInStatus", pins, values);
		for (int i = 0; i < pins.size(); ++i) {
			intputPinStates_[pins.get(i)].setValue(values.get(i));
		}
	}

	@Override
	public void handleSpiReportTxStatus(int spiNum, int bytesRemaining) {
		// logMethod("handleSpiReportTxStatus", spiNum, bytesRemaining);
		spiStates_[spiNum].reportAdditionalBuffer(bytesRemaining);
	}

	@Override
	public void handleI2cResult(int i2cNum, int size, byte[] data) {
		// logMethod("handleI2cResult", i2cNum, size, data);
		twiStates_[i2cNum].dataReceived(data, size);
	}

	@Override
	public void handleIncapReport(int incapNum, int size, byte[] data) {
		// logMethod("handleIncapReport", incapNum, size, data);
		incapStates_[incapNum].dataReceived(data, size);
	}

	@Override
	public void handleIncapClose(int incapNum) {
		// logMethod("handleIncapClose", incapNum);
		incapStates_[incapNum].closeCurrentListener();
	}

	@Override
	public void handleIncapOpen(int incapNum) {
		// logMethod("handleIncapOpen", incapNum);
		incapStates_[incapNum].openNextListener();
	}

	@Override
	public void handleIcspResult(int size, byte[] data) {
		// logMethod("handleIcspResult", size, data);
		icspState_.dataReceived(data, size);
	}

	private void checkNotDisconnected() throws ConnectionLostException {
		if (connection_ == ConnectionState.DISCONNECTED) {
			throw new ConnectionLostException();
		}
	}

//	private void logMethod(String name, Object... args) {
//		StringBuffer msg = new StringBuffer(name);
//		msg.append('(');
//		for (int i = 0; i < args.length; ++i) {
//			if (i != 0) {
//				msg.append(", ");
//			}
//			msg.append(args[i]);
//		}
//		msg.append(')');
//
//		Log.v("IncomingState", msg.toString());
//	}
}
