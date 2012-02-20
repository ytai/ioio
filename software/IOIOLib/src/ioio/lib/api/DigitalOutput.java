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
package ioio.lib.api;

import ioio.lib.api.exception.ConnectionLostException;

/**
 * A pin used for digital output.
 * <p>
 * A digital input pin can be used to generate logic-level signals.
 * DigitalOutput instances are obtained by calling
 * {@link IOIO#openDigitalOutput(DigitalOutput.Spec, boolean)}.
 * <p>
 * The value of the pin is set by calling {@link #write(boolean)}.
 * <p>
 * The instance is alive since its creation. If the connection with the IOIO
 * drops at any point, the instance transitions to a disconnected state, in
 * which every attempt to use the pin (except {@link #close()}) will throw a
 * {@link ConnectionLostException}. Whenever {@link #close()} is invoked the
 * instance may no longer be used. Any resources associated with it are freed
 * and can be reused.
 * <p>
 * Typical usage:
 * 
 * <pre>
 * DigitalOutput led = ioio.openDigitalInput(2);  // LED anode on pin 2.
 * led.write(true);  // turn LED on.
 * ...
 * led.close();  // pin 2 can now be used for something else.
 * </pre>
 */
public interface DigitalOutput extends Closeable {
	/**
	 * A digital output pin specification, used when opening digital outputs.
	 */
	public static class Spec {
		/** Output pin mode. */
		public enum Mode {
			/**
			 * Pin operates in push-pull mode, i.e. a logical "HIGH" is
			 * represented by a voltage of Vdd on the pin and a logical "LOW" by
			 * a voltage of 0 (ground).
			 */
			NORMAL,
			/**
			 * Pin operates in open-drain mode, i.e. a logical "HIGH" is
			 * represented by a high impedance on the pin (as if it is
			 * disconnected) and a logical "LOW" by a voltage of 0 (ground).
			 * This mode is most commonly used for generating 5V logical signal
			 * on a 3.3V pin: 5V tolerant pins must be used; a pull-up resistor
			 * is connected between the pin and 5V, and the pin is used in open-
			 * drain mode.
			 */
			OPEN_DRAIN,
		}

		/** The pin number, as labeled on the board. */
		public int pin;
		/** The pin mode. */
		public Mode mode;

		/**
		 * Constructor.
		 * 
		 * @param pin
		 *            Pin number, as labeled on the board.
		 * @param mode
		 *            Pin mode.
		 */
		public Spec(int pin, Mode mode) {
			this.pin = pin;
			this.mode = mode;
		}

		/**
		 * Shorthand for Spec(pin, Mode.NORMAL).
		 * 
		 * @see DigitalOutput.Spec#Spec(int, DigitalOutput.Spec.Mode)
		 */
		public Spec(int pin) {
			this(pin, Mode.NORMAL);
		}
	}

	/**
	 * Set the output of the pin.
	 * 
	 * @param val
	 *            The output. true is logical "HIGH", false is logical "LOW".
	 * @throws ConnectionLostException
	 *             The connection with the IOIO has been lost.
	 */
	public void write(boolean val) throws ConnectionLostException;
}
