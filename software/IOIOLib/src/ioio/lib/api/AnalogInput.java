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

import java.io.Closeable;

import ioio.lib.api.exception.ConnectionLostException;

/**
 * A pin used for analog input.
 * <p>
 * An analog input pin can be used to measure voltage. AnalogInput instances are
 * obtained by calling {@link IOIO#openAnalogInput(int)}.
 * <p>
 * Floating-point values scaled from 0 to 1 can be obtained by calling
 * {@link #read()}. Absolute voltage levels can be obtained by calling
 * {@link #getVoltage()}.
 * <p>
 * The instance is alive since its creation. The first {@link #read()} call
 * block for a few milliseconds until the initial value is updated. If the
 * connection with the IOIO drops at any point, the instance transitions to a
 * disconnected state, in which every attempt to use the pin (except
 * {@link #close()}) will throw a {@link ConnectionLostException}. Whenever
 * {@link #close()} is invoked the instance may no longer be used. Any resources
 * associated with it are freed and can be reused.
 * <p>
 * Typical usage:
 * 
 * <pre>
 * AnalogInput potentiometer = ioio.openAnalogInput(40);
 * float value = potentiometer.read();
 * ...
 * potentiometer.close();  // pin 40 can now be used for something else.
 * </pre>
 * 
 * @see IOIO#openAnalogInput(int)
 */
public interface AnalogInput extends Closeable {
	/**
	 * Gets the analog input reading, as an absolute voltage in Volt units.
	 * <p>
	 * It typically takes a few milliseconds between when the instance is
	 * created and until the first value can be read. In this case, the method
	 * may block shortly. If this is a problem, the calling thread can be
	 * interrupted.
	 * <p>
	 * If a scaled value is desired, consider using {@link #read()}. 
	 * 
	 * @return The voltage, in Volt units.
	 * @throws InterruptedException The calling thread has been interrupted.
	 * @throws ConnectionLostException The connection with the IOIO is lost.
	 * @see #read()
	 */
	public float getVoltage() throws InterruptedException,
			ConnectionLostException;

	/**
	 * Gets the maximum value against which {@link #read()} values are scaled.
	 * @return The voltage, in Volts.
	 */
	public float getReference();

	/**
	 * Gets the analog input reading, as a scaled real value between 0 and 1.
	 * <p>
	 * It typically takes a few milliseconds between when the instance is
	 * created and until the first value can be read. In this case, the method
	 * may block shortly. If this is a problem, the calling thread can be
	 * interrupted.
	 * <p>
	 * If an absolute value is desired, consider using {@link #getVoltage()}.
	 * 
	 * @return The voltage, in scaled units.
	 * @throws InterruptedException The calling thread has been interrupted.
	 * @throws ConnectionLostException The connection with the IOIO is lost.
	 * @see #getVoltage()
	 */
	public float read() throws InterruptedException, ConnectionLostException;
}
