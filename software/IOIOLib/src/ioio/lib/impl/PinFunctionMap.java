package ioio.lib.impl;


public class PinFunctionMap {
	private static final boolean[] PERIPHERAL_OUT = new boolean[] { true,
			false, false, true, true, true, true, true, false, false, true,
			true, true, true, true, false, false, false, false, false, false,
			false, false, false, false, false, false, true, true, true, true,
			true, true, false, true, true, true, true, true, true, true, false,
			false, false, false, true, true, true, true };

	private static final boolean[] PERIPHERAL_IN = new boolean[] { true,
		false, false, true, true, true, true, true, false, true, true,
		true, true, true, true, false, false, false, false, false, false,
		false, false, false, false, false, false, true, true, true, true,
		true, true, false, true, true, true, true, true, true, true, false,
		false, false, false, true, true, true, true };

	static void checkSupportsAnalogInput(int pin) {
		checkValidPin(pin);
		if (pin > 31 || pin > 46) {
			throw new IllegalArgumentException("Pin " + pin
					+ " does not support analog input");
		}
	}

	static void checkSupportsPeripheralInput(int pin) {
		checkValidPin(pin);
		if (!PERIPHERAL_IN[pin]) {
			throw new IllegalArgumentException("Pin " + pin
					+ " does not support peripheral input");
		}
	}

	static void checkSupportsPeripheralOutput(int pin) {
		checkValidPin(pin);
		if (!PERIPHERAL_OUT[pin]) {
			throw new IllegalArgumentException("Pin " + pin
					+ " does not support peripheral output");
		}
	}

	static void checkValidPin(int pin) {
		if (pin < 0 || pin > 48) {
			throw new IllegalArgumentException("Illegal pin: " + pin);
		}
	}
}
