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


class PinFunctionMap {
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
		if (pin < 31 || pin > 46) {
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
