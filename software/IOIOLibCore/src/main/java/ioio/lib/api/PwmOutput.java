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
 * A pin used for PWM (Pulse-Width Modulation) output.
 * <p>
 * A PWM pin produces a logic-level PWM signal. These signals are typically used
 * for simulating analog outputs for controlling the intensity of LEDs, the
 * rotation speed of motors, etc. They are also frequently used for controlling
 * hobby servo motors. PwmOutput instances are obtained by calling
 * {@link IOIO#openPwmOutput(ioio.lib.api.DigitalOutput.Spec, int)}.
 * <p>
 * When used for motors and LEDs, a frequency of several KHz is typically used,
 * where there is a trade-off between switching power-loses and smoothness of
 * operation. The pulse width is typically set by specifying the duty cycle,
 * with the {@link #setDutyCycle(float)} method. A duty cycle of 0 is "off", a
 * duty cycle of 1 is "on", and every intermediate value produces an
 * intermediate intensity. Please note that any devices consuming more than 20mA
 * of current (e.g. motors) should not by directly connected the the IOIO pins,
 * but rather through an amplification circuit suited for the specific load.
 * <p>
 * When used for hobby servos, the PWM signal is rather used for encoding of the
 * desired angle the motor should go to. By standard, a 100Hz signal is used and
 * the pulse width is varied between 1ms and 2ms (corresponding to both extremes
 * of the shaft angle), using {@link #setPulseWidth(int)}.
 * <p>
 * The instance is alive since its creation. If the connection with the IOIO
 * drops at any point, the instance transitions to a disconnected state, in
 * which every attempt to use the pin (except {@link #close()}) will throw a
 * {@link ConnectionLostException}. Whenever {@link #close()} is invoked the
 * instance may no longer be used. Any resources associated with it are freed
 * and can be reused.
 * <p>
 * Typical usage (fading LED):
 * 
 * <pre>
 * PwmOutput servo = ioio.openPwmOutput(12, 1000);  // LED anode on pin 12
 * ...
 * servo.setDutyCycle(0.0f);  // LED off
 * ... 
 * servo.setDutyCycle(0.5f);  // 50% intensity
 * ... 
 * servo.setDutyCycle(1.0f);  // 100% intensity
 * ... 
 * servo.close();  // pin 12 can now be used for something else.
 * </pre>
 * <p>
 * Typical usage (servo):
 * 
 * <pre>
 * PwmOutput servo = ioio.openPwmOutput(12, 100);
 * ...
 * servo.setPulseWidth(1000);  // 1000us = 1ms = one extreme
 * ... 
 * servo.setPulseWidth(1500);  // 1500us = 1.5ms = center
 * ... 
 * servo.setPulseWidth(2000);  // 2000us = 2ms = other extreme
 * ... 
 * servo.close();  // pin 12 can now be used for something else.
 * </pre>
 * 
 * @see IOIO#openPwmOutput(ioio.lib.api.DigitalOutput.Spec, int)
 */
public interface PwmOutput extends Closeable {
	/**
	 * Sets the duty cycle of the PWM output. The duty cycle is defined to be
	 * the pulse width divided by the total cycle period. For absolute control
	 * of the pulse with, consider using {@link #setPulseWidth(int)}.
	 * 
	 * @param dutyCycle
	 *            The duty cycle, as a real value from 0.0 to 1.0.
	 * @throws ConnectionLostException
	 *             The connection to the IOIO has been lost.
	 * @see #setPulseWidth(int)
	 */
	public void setDutyCycle(float dutyCycle) throws ConnectionLostException;

	/**
	 * Sets the pulse width of the PWM output. The pulse width is duration of
	 * the high-time within a single period of the signal. For relative control
	 * of the pulse with, consider using {@link #setDutyCycle(float)}.
	 * 
	 * @param pulseWidthUs
	 *            The pulse width, in microsecond units.
	 * @throws ConnectionLostException
	 *             The connection to the IOIO has been lost.
	 * @see #setDutyCycle(float)
	 */
	public void setPulseWidth(int pulseWidthUs) throws ConnectionLostException;

	/**
	 * The same as {@link #setPulseWidth(int)}, but with sub-microsecond
	 * precision.
	 */
	public void setPulseWidth(float pulseWidthUs)
			throws ConnectionLostException;
}
