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

import ioio.lib.api.AnalogInput;
import ioio.lib.api.CapSense;
import ioio.lib.api.DigitalInput;
import ioio.lib.api.DigitalInput.Spec;
import ioio.lib.api.DigitalInput.Spec.Mode;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.IOIOConnection;
import ioio.lib.api.IcspMaster;
import ioio.lib.api.PulseInput;
import ioio.lib.api.PulseInput.ClockRate;
import ioio.lib.api.PulseInput.PulseMode;
import ioio.lib.api.PwmOutput;
import ioio.lib.api.Sequencer;
import ioio.lib.api.SpiMaster;
import ioio.lib.api.TwiMaster;
import ioio.lib.api.TwiMaster.Rate;
import ioio.lib.api.Uart;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
import ioio.lib.impl.IOIOProtocol.PwmScale;
import ioio.lib.impl.IncomingState.DisconnectListener;
import ioio.lib.impl.ResourceManager.Resource;
import ioio.lib.impl.ResourceManager.ResourceType;
import ioio.lib.spi.Log;

import java.io.IOException;

public class IOIOImpl implements IOIO, DisconnectListener {
	private static class SyncListener implements IncomingState.SyncListener, DisconnectListener {
		enum State { WAITING, SIGNALED, DISCONNECTED };
		private State state_ = State.WAITING;

		@Override
		public synchronized void sync() {
			state_ = State.SIGNALED;
			notifyAll();
		}

		public synchronized void waitSync() throws InterruptedException, ConnectionLostException {
			while (state_ == State.WAITING) {
				wait();
			}
			if (state_ == State.DISCONNECTED) {
				throw new ConnectionLostException();
			}
		}

		@Override
		public synchronized void disconnected() {
			state_ = State.DISCONNECTED;
			notifyAll();
		}
	}

	private static final String TAG = "IOIOImpl";
	private boolean disconnect_ = false;

	private static final byte[] REQUIRED_INTERFACE_ID = new byte[] { 'I', 'O',
			'I', 'O', '0', '0', '0', '5' };

	IOIOProtocol protocol_;
	ResourceManager resourceManager_;
	IncomingState incomingState_ = new IncomingState();
	Board.Hardware hardware_;
	private IOIOConnection connection_;
	private State state_ = State.INIT;

	public IOIOImpl(IOIOConnection con) {
		connection_ = con;
	}

	@Override
	public void waitForConnect() throws ConnectionLostException,
			IncompatibilityException {
		if (state_ == State.CONNECTED) {
			return;
		}
		if (state_ == State.DEAD) {
			throw new ConnectionLostException();
		}
		addDisconnectListener(this);
		Log.d(TAG, "Waiting for IOIO connection");
		try {
			try {
				Log.v(TAG, "Waiting for underlying connection");
				connection_.waitForConnect();
				synchronized (this) {
					if (disconnect_) {
						throw new ConnectionLostException();
					}
					protocol_ = new IOIOProtocol(connection_.getInputStream(),
							connection_.getOutputStream(), incomingState_);
					// Once this block exits, a disconnect will also involve
					// softClose().
				}
			} catch (ConnectionLostException e) {
				incomingState_.handleConnectionLost();
				throw e;
			}
			Log.v(TAG, "Waiting for handshake");
			incomingState_.waitConnectionEstablished();
			initBoard();
			Log.v(TAG, "Querying for required interface ID");
			checkInterfaceVersion();
			Log.v(TAG, "Required interface ID is supported");
			state_ = State.CONNECTED;
			Log.i(TAG, "IOIO connection established");
		} catch (ConnectionLostException e) {
			Log.d(TAG, "Connection lost / aborted");
			state_ = State.DEAD;
			throw e;
		} catch (IncompatibilityException e) {
			throw e;
		} catch (InterruptedException e) {
			Log.e(TAG, "Unexpected exception", e);
		}
	}

	@Override
	public synchronized void disconnect() {
		Log.d(TAG, "Client requested disconnect.");
		if (disconnect_) {
			return;
		}
		disconnect_ = true;
		try {
			if (protocol_ != null && !connection_.canClose()) {
				protocol_.softClose();
			}
		} catch (IOException e) {
			Log.e(TAG, "Soft close failed", e);
		}
		connection_.disconnect();
	}

	@Override
	public synchronized void disconnected() {
		state_ = State.DEAD;
		if (disconnect_) {
			return;
		}
		Log.d(TAG, "Physical disconnect.");
		disconnect_ = true;
		// The IOIOConnection doesn't necessarily know about the disconnect
		connection_.disconnect();
	}

	@Override
	public void waitForDisconnect() throws InterruptedException {
		incomingState_.waitDisconnect();
	}

	@Override
	public State getState() {
		return state_;
	}

	private void initBoard() throws IncompatibilityException {
		if (incomingState_.board_ == null) {
			throw new IncompatibilityException("Unknown board: "
					+ incomingState_.hardwareId_);
		}
		hardware_ = incomingState_.board_.hardware;
		resourceManager_ = new ResourceManager(hardware_);
	}

	private void checkInterfaceVersion() throws IncompatibilityException,
			ConnectionLostException, InterruptedException {
		try {
			protocol_.checkInterface(REQUIRED_INTERFACE_ID);
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
		if (!incomingState_.waitForInterfaceSupport()) {
			state_ = State.INCOMPATIBLE;
			Log.e(TAG, "Required interface ID is not supported");
			throw new IncompatibilityException(
					"IOIO firmware does not support required firmware: "
							+ new String(REQUIRED_INTERFACE_ID));
		}
	}

	synchronized void removeDisconnectListener(DisconnectListener listener) {
		incomingState_.removeDisconnectListener(listener);
	}

	synchronized void addDisconnectListener(DisconnectListener listener)
			throws ConnectionLostException {
		incomingState_.addDisconnectListener(listener);
	}

	synchronized void closePin(ResourceManager.Resource pin) {
		try {
			protocol_.setPinDigitalIn(pin.id, DigitalInput.Spec.Mode.FLOATING);
			resourceManager_.free(pin);
		} catch (IOException e) {
		}
	}

	@Override
	synchronized public void softReset() throws ConnectionLostException {
		checkState();
		try {
			protocol_.softReset();
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	synchronized public void hardReset() throws ConnectionLostException {
		checkState();
		try {
			protocol_.hardReset();
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	public String getImplVersion(VersionType v) throws ConnectionLostException {
		if (state_ == State.INIT) {
			throw new IllegalStateException(
					"Connection has not yet been established");
		}
		switch (v) {
		case HARDWARE_VER:
			return incomingState_.hardwareId_;
		case BOOTLOADER_VER:
			return incomingState_.bootloaderId_;
		case APP_FIRMWARE_VER:
			return incomingState_.firmwareId_;
		case IOIOLIB_VER:
			return "IOIO0503";
		}
		return null;
	}

	@Override
	public DigitalInput openDigitalInput(int pin)
			throws ConnectionLostException {
		return openDigitalInput(new DigitalInput.Spec(pin));
	}

	@Override
	public DigitalInput openDigitalInput(int pin, Mode mode)
			throws ConnectionLostException {
		return openDigitalInput(new DigitalInput.Spec(pin, mode));
	}

	@Override
	synchronized public DigitalInput openDigitalInput(DigitalInput.Spec spec)
			throws ConnectionLostException {
		checkState();
		Resource pin = new Resource(ResourceType.PIN, spec.pin);
		resourceManager_.alloc(pin);
		DigitalInputImpl result = new DigitalInputImpl(this, pin);
		addDisconnectListener(result);
		incomingState_.addInputPinListener(spec.pin, result);
		try {
			protocol_.setPinDigitalIn(spec.pin, spec.mode);
			protocol_.setChangeNotify(spec.pin, true);
		} catch (IOException e) {
			result.close();
			throw new ConnectionLostException(e);
		}
		return result;
	}

	@Override
	public DigitalOutput openDigitalOutput(int pin,
			ioio.lib.api.DigitalOutput.Spec.Mode mode, boolean startValue)
			throws ConnectionLostException {
		return openDigitalOutput(new DigitalOutput.Spec(pin, mode), startValue);
	}

	@Override
	synchronized public DigitalOutput openDigitalOutput(
			DigitalOutput.Spec spec, boolean startValue)
			throws ConnectionLostException {
		checkState();
		Resource pin = new Resource(ResourceType.PIN, spec.pin);
		resourceManager_.alloc(pin);

		DigitalOutputImpl result = new DigitalOutputImpl(this, pin, startValue);
		addDisconnectListener(result);
		try {
			protocol_.setPinDigitalOut(spec.pin, startValue, spec.mode);
		} catch (IOException e) {
			result.close();
			throw new ConnectionLostException(e);
		}
		return result;
	}

	@Override
	public DigitalOutput openDigitalOutput(int pin, boolean startValue)
			throws ConnectionLostException {
		return openDigitalOutput(new DigitalOutput.Spec(pin), startValue);
	}

	@Override
	public DigitalOutput openDigitalOutput(int pin)
			throws ConnectionLostException {
		return openDigitalOutput(new DigitalOutput.Spec(pin), false);
	}

	@Override
	synchronized public AnalogInput openAnalogInput(int pinNum)
			throws ConnectionLostException {
		checkState();
		hardware_.checkSupportsAnalogInput(pinNum);
		Resource pin = new Resource(ResourceType.PIN, pinNum);
		resourceManager_.alloc(pin);
		AnalogInputImpl result = new AnalogInputImpl(this, pin);
		addDisconnectListener(result);
		incomingState_.addInputPinListener(pinNum, result);
		try {
			protocol_.setPinAnalogIn(pinNum);
			protocol_.setAnalogInSampling(pinNum, true);
		} catch (IOException e) {
			result.close();
			throw new ConnectionLostException(e);
		}
		return result;
	}

	@Override
	public CapSense openCapSense(int pin) throws ConnectionLostException {
		return openCapSense(pin, CapSense.DEFAULT_COEF);
	}

	@Override
	public synchronized CapSense openCapSense(int pinNum, float filterCoef)
			throws ConnectionLostException {
		checkState();
		hardware_.checkSupportsCapSense(pinNum);
		Resource pin = new Resource(ResourceType.PIN, pinNum);
		resourceManager_.alloc(pin);
		CapSenseImpl result = new CapSenseImpl(this, pin, filterCoef);
		addDisconnectListener(result);
		incomingState_.addInputPinListener(pinNum, result);
		try {
			protocol_.setPinCapSense(pinNum);
			protocol_.setCapSenseSampling(pinNum, true);
		} catch (IOException e) {
			result.close();
			throw new ConnectionLostException(e);
		}
		return result;
	}

	@Override
	public PwmOutput openPwmOutput(int pin, int freqHz)
			throws ConnectionLostException {
		return openPwmOutput(new DigitalOutput.Spec(pin), freqHz);
	}

	@Override
	synchronized public PwmOutput openPwmOutput(DigitalOutput.Spec spec,
			int freqHz) throws ConnectionLostException {
		checkState();
		hardware_.checkSupportsPeripheralOutput(spec.pin);

		Resource pin = new Resource(ResourceType.PIN, spec.pin);
		Resource oc = new Resource(ResourceType.OUTCOMPARE);

		resourceManager_.alloc(pin, oc);
		int scale = 0;
		float baseUs;
		int period;
		while (true) {
			final int clk = 16000000 / IOIOProtocol.PwmScale.values()[scale].scale;
			period = clk / freqHz;
			if (period <= 65536) {
				baseUs = 1000000.0f / clk;
				break;
			}
			if (++scale >= PwmScale.values().length) {
				throw new IllegalArgumentException("Frequency too low: "
						+ freqHz);
			}
		}

		PwmImpl pwm = new PwmImpl(this, pin, oc, period, baseUs);
		addDisconnectListener(pwm);
		try {
			protocol_.setPinDigitalOut(spec.pin, false, spec.mode);
			protocol_.setPinPwm(spec.pin, oc.id, true);
			protocol_.setPwmPeriod(oc.id, period - 1,
					IOIOProtocol.PwmScale.values()[scale]);
		} catch (IOException e) {
			pwm.close();
			throw new ConnectionLostException(e);
		}
		return pwm;
	}

	@Override
	public Uart openUart(int rx, int tx, int baud, Uart.Parity parity,
			Uart.StopBits stopbits) throws ConnectionLostException {
		return openUart(rx == INVALID_PIN ? null : new DigitalInput.Spec(rx),
				tx == INVALID_PIN ? null : new DigitalOutput.Spec(tx), baud,
				parity, stopbits);
	}

	@Override
	synchronized public Uart openUart(DigitalInput.Spec rx,
			DigitalOutput.Spec tx, int baud, Uart.Parity parity,
			Uart.StopBits stopbits) throws ConnectionLostException {
		checkState();
		if (rx != null) {
			hardware_.checkSupportsPeripheralInput(rx.pin);
		}
		if (tx != null) {
			hardware_.checkSupportsPeripheralOutput(tx.pin);
		}
		Resource rxPin = rx != null ? new Resource(ResourceType.PIN, rx.pin)
				: null;
		Resource txPin = tx != null ? new Resource(ResourceType.PIN, tx.pin)
				: null;
		Resource uart = new Resource(ResourceType.UART);
		resourceManager_.alloc(rxPin, txPin, uart);

		UartImpl result = new UartImpl(this, txPin, rxPin, uart);
		addDisconnectListener(result);
		incomingState_.addUartListener(uart.id, result);
		try {
			if (rx != null) {
				protocol_.setPinDigitalIn(rx.pin, rx.mode);
				protocol_.setPinUart(rx.pin, uart.id, false, true);
			}
			if (tx != null) {
				protocol_.setPinDigitalOut(tx.pin, true, tx.mode);
				protocol_.setPinUart(tx.pin, uart.id, true, true);
			}
			boolean speed4x = true;
			int rate = Math.round(4000000.0f / baud) - 1;
			if (rate > 65535) {
				speed4x = false;
				rate = Math.round(1000000.0f / baud) - 1;
			}
			protocol_.uartConfigure(uart.id, rate, speed4x, stopbits, parity);
		} catch (IOException e) {
			result.close();
			throw new ConnectionLostException(e);
		}
		return result;
	}

	@Override
	synchronized public TwiMaster openTwiMaster(int twiNum, Rate rate,
			boolean smbus) throws ConnectionLostException {
		checkState();

		final int[][] twiPins = hardware_.twiPins();

		Resource twi = new Resource(ResourceType.TWI, twiNum);
		Resource[] pins = new Resource[] {
				new Resource(ResourceType.PIN, twiPins[twiNum][0]),
				new Resource(ResourceType.PIN, twiPins[twiNum][1]) };

		resourceManager_.alloc(twi, pins);

		TwiMasterImpl result = new TwiMasterImpl(this, twi, pins);
		addDisconnectListener(result);
		incomingState_.addTwiListener(twiNum, result);
		try {
			protocol_.i2cConfigureMaster(twiNum, rate, smbus);
		} catch (IOException e) {
			result.close();
			throw new ConnectionLostException(e);
		}
		return result;
	}

	@Override
	synchronized public IcspMaster openIcspMaster()
			throws ConnectionLostException {
		checkState();

		final int[] icspPins = hardware_.icspPins();
		Resource icsp = new Resource(ResourceType.ICSP);
		Resource[] pins = new Resource[] {
				new Resource(ResourceType.PIN, icspPins[0]),
				new Resource(ResourceType.PIN, icspPins[1]),
				new Resource(ResourceType.PIN, icspPins[2]) };

		resourceManager_.alloc(icsp, pins);

		IcspMasterImpl result = new IcspMasterImpl(this, icsp, pins);
		addDisconnectListener(result);
		incomingState_.addIcspListener(result);
		try {
			protocol_.icspOpen();
		} catch (IOException e) {
			result.close();
			throw new ConnectionLostException(e);
		}
		return result;
	}

	@Override
	public SpiMaster openSpiMaster(int miso, int mosi, int clk,
			int slaveSelect, SpiMaster.Rate rate)
			throws ConnectionLostException {
		return openSpiMaster(miso, mosi, clk, new int[] { slaveSelect }, rate);
	}

	@Override
	public SpiMaster openSpiMaster(int miso, int mosi, int clk,
			int[] slaveSelect, SpiMaster.Rate rate)
			throws ConnectionLostException {
		DigitalOutput.Spec[] slaveSpecs = new DigitalOutput.Spec[slaveSelect.length];
		for (int i = 0; i < slaveSelect.length; ++i) {
			slaveSpecs[i] = new DigitalOutput.Spec(slaveSelect[i]);
		}
		return openSpiMaster(new DigitalInput.Spec(miso, Mode.PULL_UP),
				new DigitalOutput.Spec(mosi), new DigitalOutput.Spec(clk),
				slaveSpecs, new SpiMaster.Config(rate));
	}

	@Override
	synchronized public SpiMaster openSpiMaster(DigitalInput.Spec miso,
			DigitalOutput.Spec mosi, DigitalOutput.Spec clk,
			DigitalOutput.Spec[] slaveSelect, SpiMaster.Config config)
			throws ConnectionLostException {
		checkState();

		hardware_.checkSupportsPeripheralInput(miso.pin);
		hardware_.checkSupportsPeripheralOutput(mosi.pin);
		hardware_.checkSupportsPeripheralOutput(clk.pin);

		Resource ssPins[] = new Resource[slaveSelect.length];
		Resource misoPin = new Resource(ResourceType.PIN, miso.pin);
		Resource mosiPin = new Resource(ResourceType.PIN, mosi.pin);
		Resource clkPin = new Resource(ResourceType.PIN, clk.pin);
		for (int i = 0; i < slaveSelect.length; ++i) {
			ssPins[i] = new Resource(ResourceType.PIN, slaveSelect[i].pin);
		}
		Resource spi = new Resource(ResourceType.SPI);

		resourceManager_.alloc(ssPins, misoPin, mosiPin, clkPin, spi);

		SpiMasterImpl result = new SpiMasterImpl(this, spi, mosiPin, misoPin,
				clkPin, ssPins);
		addDisconnectListener(result);

		incomingState_.addSpiListener(spi.id, result);
		try {
			protocol_.setPinDigitalIn(miso.pin, miso.mode);
			protocol_.setPinSpi(miso.pin, 1, true, spi.id);
			protocol_.setPinDigitalOut(mosi.pin, true, mosi.mode);
			protocol_.setPinSpi(mosi.pin, 0, true, spi.id);
			protocol_.setPinDigitalOut(clk.pin, config.invertClk, clk.mode);
			protocol_.setPinSpi(clk.pin, 2, true, spi.id);
			for (DigitalOutput.Spec spec : slaveSelect) {
				protocol_.setPinDigitalOut(spec.pin, true, spec.mode);
			}
			protocol_.spiConfigureMaster(spi.id, config);
		} catch (IOException e) {
			result.close();
			throw new ConnectionLostException(e);
		}
		return result;
	}

	@Override
	public PulseInput openPulseInput(Spec spec, ClockRate rate, PulseMode mode,
			boolean doublePrecision) throws ConnectionLostException {
		checkState();
		hardware_.checkSupportsPeripheralInput(spec.pin);
		Resource pin = new Resource(ResourceType.PIN, spec.pin);
		Resource incap = new Resource(
				doublePrecision ? ResourceType.INCAP_DOUBLE
						: ResourceType.INCAP_SINGLE);
		resourceManager_.alloc(pin, incap);

		IncapImpl result = new IncapImpl(this, mode, incap, pin, rate.hertz,
				mode.scaling, doublePrecision);
		addDisconnectListener(result);
		incomingState_.addIncapListener(incap.id, result);
		try {
			protocol_.setPinDigitalIn(spec.pin, spec.mode);
			protocol_.setPinIncap(spec.pin, incap.id, true);
			protocol_.incapConfigure(incap.id, doublePrecision,
					mode.ordinal() + 1, rate.ordinal());
		} catch (IOException e) {
			result.close();
			throw new ConnectionLostException(e);
		}
		return result;
	}

	@Override
	public PulseInput openPulseInput(int pin, PulseMode mode)
			throws ConnectionLostException {
		return openPulseInput(new DigitalInput.Spec(pin), ClockRate.RATE_16MHz,
				mode, true);
	}

	@Override
	public Sequencer openSequencer(Sequencer.ChannelConfig config[])
			throws ConnectionLostException {
		return new SequencerImpl(this, config);
	}

	private void checkState() throws ConnectionLostException {
		if (state_ == State.DEAD) {
			throw new ConnectionLostException();
		}
		if (state_ == State.INCOMPATIBLE) {
			throw new IllegalStateException(
					"Incompatibility has been reported - IOIO cannot be used");
		}
		if (state_ != State.CONNECTED) {
			throw new IllegalStateException(
					"Connection has not yet been established");
		}
	}

	@Override
	public synchronized void beginBatch() throws ConnectionLostException {
		checkState();
		protocol_.beginBatch();
	}

	@Override
	public synchronized void endBatch() throws ConnectionLostException {
		checkState();
		try {
			protocol_.endBatch();
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	public void sync() throws ConnectionLostException, InterruptedException {
		SyncListener listener = new SyncListener();
		synchronized (this) {
			checkState();
			incomingState_.addSyncListener(listener);
			incomingState_.addDisconnectListener(listener);
			try {
				protocol_.sync();
			} catch (IOException e) {
				throw new ConnectionLostException(e);
			}
		}
		listener.waitSync();
	}
}
