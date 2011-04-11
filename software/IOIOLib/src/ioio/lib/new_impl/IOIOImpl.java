package ioio.lib.new_impl;

import ioio.lib.api.AnalogInput;
import ioio.lib.api.DigitalInput;
import ioio.lib.api.DigitalInputSpec;
import ioio.lib.api.DigitalInputSpec.Mode;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.DigitalOutputSpec;
import ioio.lib.api.IOIO;
import ioio.lib.api.PwmOutput;
import ioio.lib.api.Uart;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.InvalidOperationException;
import ioio.lib.api.exception.OutOfResourceException;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class IOIOImpl implements IOIO {
	IOIOProtocol protocol_;
	private boolean connected_ = true;
	private IncomingState incomingState_ = new IncomingState();
	private boolean openPins_[] = new boolean[Constants.NUM_PINS];
	private ModuleAllocator pwmAllocator_ = new ModuleAllocator(Constants.NUM_PWM_MODULES);
	
	public IOIOImpl(InputStream in, OutputStream out) throws InterruptedException, ConnectionLostException {
		protocol_ = new IOIOProtocol(in, out, incomingState_);
		incomingState_.waitConnect();
	}
	
	synchronized void closePin(int pin) {
		if (openPins_[pin]) {
			try {
				protocol_.setPinDigitalIn(pin, 0);
			} catch (IOException e) {
			}
			openPins_[pin] = false;
		}
	}
	
	synchronized void closePwm(int pwmNum) {
		pwmAllocator_.releaseModule(pwmNum);
		try {
			protocol_.setPwmPeriod(pwmNum, 0, false);
		} catch (IOException e) {
		}
	}

	@Override
	synchronized public void disconnect() throws InterruptedException {
		protocol_.close();
		incomingState_.waitDisconnect();
	}

	@Override
	public boolean isConnected() {
		return connected_;
	}

	@Override
	synchronized public void softReset() throws ConnectionLostException {
		try {
			protocol_.softReset();
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	synchronized public void hardReset() throws ConnectionLostException {
		try {
			protocol_.hardReset();
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	public DigitalInput openDigitalInput(int pin)
			throws ConnectionLostException, InvalidOperationException {
		return openDigitalInput(new DigitalInputSpec(pin));
	}

	@Override
	public DigitalInput openDigitalInput(int pin, Mode mode)
			throws ConnectionLostException, InvalidOperationException {
		return openDigitalInput(new DigitalInputSpec(pin, mode));
	}

	@Override
	public DigitalInput openDigitalInput(DigitalInputSpec spec)
			throws ConnectionLostException, InvalidOperationException {
		if (openPins_[spec.pin]) {
			throw new InvalidOperationException("Pin number " + spec.pin + " is already open.");
		}
		DigitalInputImpl result = new DigitalInputImpl(this, spec.pin);
		openPins_[spec.pin] = true;
		incomingState_.addPinListener(spec.pin, result);
		try {
			int pull = 0;
			if (spec.mode == DigitalInputSpec.Mode.PULL_UP) {
				pull = 1;
			} else if (spec.mode == DigitalInputSpec.Mode.PULL_DOWN) {
				pull = 2;
			}
			protocol_.setPinDigitalIn(spec.pin, pull);
			protocol_.setChangeNotify(spec.pin, true);
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
		return result;
	}

	@Override
	synchronized public DigitalOutput openDigitalOutput(int pin,
			ioio.lib.api.DigitalOutputSpec.Mode mode, boolean startValue)
			throws ConnectionLostException, InvalidOperationException {
		return openDigitalOutput(new DigitalOutputSpec(pin, mode), startValue);
	}

	@Override
	synchronized public DigitalOutput openDigitalOutput(DigitalOutputSpec spec,
			boolean startValue) throws ConnectionLostException,
			InvalidOperationException {
		if (openPins_[spec.pin]) {
			throw new InvalidOperationException("Pin number " + spec.pin + " is already open.");
		}
		DigitalOutputImpl result = new DigitalOutputImpl(this, spec.pin);
		openPins_[spec.pin] = true;
		incomingState_.addPinListener(spec.pin, result);
		try {
			protocol_.setPinDigitalOut(spec.pin, startValue, spec.mode == DigitalOutputSpec.Mode.OPEN_DRAIN);
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
		return result;
	}

	@Override
	public DigitalOutput openDigitalOutput(int pin, boolean startValue)
			throws ConnectionLostException, InvalidOperationException {
		return openDigitalOutput(new DigitalOutputSpec(pin), startValue);
	}

	@Override
	public DigitalOutput openDigitalOutput(int pin)
			throws ConnectionLostException, InvalidOperationException {
		return openDigitalOutput(new DigitalOutputSpec(pin), false);
	}

	@Override
	synchronized public AnalogInput openAnalogInput(int pin) throws ConnectionLostException,
			InvalidOperationException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public PwmOutput openPwmOutput(int pin, int freqHz)
			throws OutOfResourceException, ConnectionLostException,
			InvalidOperationException {
		return openPwmOutput(new DigitalOutputSpec(pin), freqHz);
	}

	@Override
	synchronized public PwmOutput openPwmOutput(DigitalOutputSpec spec, int freqHz)
			throws OutOfResourceException, ConnectionLostException,
			InvalidOperationException {
		if (openPins_[spec.pin]) {
			throw new InvalidOperationException("Pin number " + spec.pin + " is already open.");
		}
		Integer pwmNum = pwmAllocator_.allocateModule();
		if (pwmNum == null) {
			throw new OutOfResourceException("No more available PWM modules");
		}
		int period = 16000000 / freqHz - 1;
		boolean scale256 = false;
		int effectivePeriodMicroSec;
		if (period > 65535) {
			period = 16000000 / freqHz / 256 - 1;
			scale256 = true;
			effectivePeriodMicroSec = (period + 1) * 16;
		} else {
			effectivePeriodMicroSec = (period + 1) / 16;
		}
 		PwmImpl pwm = new PwmImpl(this, spec.pin, pwmNum, period, effectivePeriodMicroSec);
		openPins_[spec.pin] = true;
		incomingState_.addPinListener(spec.pin, pwm);
		try {
			protocol_.setPinDigitalOut(spec.pin, false, spec.mode == DigitalOutputSpec.Mode.OPEN_DRAIN);
			protocol_.setPinPwm(spec.pin, pwmNum);
			protocol_.setPwmPeriod(pwmNum, period, scale256);
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
		return pwm;
		
	}

	@Override
	public Uart openUart(int rx, int tx, int baud, int parity, int stopbits)
			throws ConnectionLostException, InvalidOperationException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	synchronized public Uart openUart(DigitalInputSpec rx, DigitalOutputSpec tx, int baud,
			int parity, int stopbits) throws ConnectionLostException,
			InvalidOperationException {
		// TODO Auto-generated method stub
		return null;
	}

//	@Override
//	public IOIOSpi openSpi(int miso, int mosi, int clk, int select, int speed)
//			throws ConnectionLostException, InvalidOperationException {
//		// TODO Auto-generated method stub
//		return null;
//	}
//
//	@Override
//	synchronized public IOIOSpi openSpi(DigitalInputSpec miso, DigitalOutputSpec mosi,
//			DigitalOutputSpec clk, DigitalOutputSpec select, int speed)
//			throws ConnectionLostException, InvalidOperationException {
//		// TODO Auto-generated method stub
//		return null;
//	}
//
//	@Override
//	synchronized public IOIOTwi openTwi(int twiNum, int speed) throws ConnectionLostException,
//			InvalidOperationException {
//		// TODO Auto-generated method stub
//		return null;
//	}
}
