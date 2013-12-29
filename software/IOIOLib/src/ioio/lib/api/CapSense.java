/*
 * Copyright 2013 Ytai Ben-Tsvi. All rights reserved.
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
 * A pin used for capacitive sensing.
 * <p>
 * A cap-sense input pin can be used to measure capacitance, most frequently for touch sensing.
 * CapSense instances are obtained by calling {@link IOIO#openCapSense(int)}.
 * <p>
 * Capacitance is measured by pushing a known amount of charge into the circuit, and measuring the
 * increase in voltage. As capacitance gets bigger, this increase becomes smaller, and thus less
 * accurate. As capacitance gets smaller, the increase may become fast enough so it can saturate,
 * i.e. reach the maximum voltage. The system has been tuned to effectively sense capacitance values
 * typical of the human body, for touch sensors. The lowest possible capacitance is about 27pF, and
 * at about 2700pF the precision will be around 10% (with high noise level unless filtered). The
 * internal capacitance of the system with some parasitic capacitance will normally be 30pF or more.
 * Human-body capacitance will typically be about 150pF without grounding, and greater with
 * grounding or when touching a large metallic surface.
 * <p>
 * Floating-point values in pico-Farade units can be obtained by calling {@link #read()}.
 * <p>
 * For better noise immunity, some low-pass filtering is recommended. This module provides a simple,
 * single-pole IIR filtering, with configurable time- constant. There is a trade-off when selecting
 * the time-constant: a longer time constant will provide better noise filtering, but at the cost of
 * a slower response. In other words, it will take more time for the measured value to reach the
 * actual. The default value of {@link #DEFAULT_COEF}( {@value #DEFAULT_COEF}pF) is a reasonable one
 * in many cases. To change it, call {@link #setFilterCoef(float)}, or use the overload the open
 * method, which gets a filter coefficient argument: {@link IOIO#openCapSense(int, float)}.
 * <p>
 * The instance is alive since its creation. The first {@link #read()} call block for a few
 * milliseconds until the initial value is updated. If the connection with the IOIO drops at any
 * point, the instance transitions to a disconnected state, in which every attempt to use the pin
 * (except {@link #close()}) will throw a {@link ConnectionLostException}. Whenever {@link #close()}
 * is invoked the instance may no longer be used. Any resources associated with it are freed and can
 * be reused.
 * <p>
 * Typical usage:
 *
 * <pre>
 * CapSense touchSensor = ioio.openCapSense(40);
 * if (touchSensor.read() > 50) {
 *   // Clicked!
 *   ...
 * }
 *
 * ...
 * touchSensor.close();  // optional. pin 40 can now be used for something else.
 * </pre>
 *
 * @see IOIO#openCapSense(int)
 * @see IOIO#openCapSense(int, float)
 */
public interface CapSense extends Closeable {
	public static final float DEFAULT_COEF = 25.f;

	/**
	 * Gets the capacitance reading.
	 * <p>
	 * It typically takes a few milliseconds between when the instance is created and until the
	 * first value can be read. In this case, the method may block shortly. If this is a problem,
	 * the calling thread can be interrupted.
	 * <p>
	 * This value is computed using a filtered signal, which is configured via
	 * {@link #setFilterCoef(float)}
	 *
	 * @return The capacitance, in pico-Farade units.
	 * @throws InterruptedException
	 *             The calling thread has been interrupted.
	 * @throws ConnectionLostException
	 *             The connection with the IOIO is lost.
	 */
	public float read() throws InterruptedException, ConnectionLostException;

	/**
	 * This is very similar to {@link #read()}, but will wait for a new sample to arrive before
	 * returning. This is useful in conjunction with {@link IOIO#sync()}, in cases when we want to
	 * guarantee the we are looking at a sample that has been captured strictly after certain other
	 * commands have been executed.
	 *
	 * @return The capacitance, in pico-Farade units.
	 * @throws InterruptedException
	 *             The calling thread has been interrupted.
	 * @throws ConnectionLostException
	 *             The connection with the IOIO is lost.
	 * @see #read()
	 */
	public float readSync() throws InterruptedException, ConnectionLostException;

	/**
	 * Sets the low-pass filter coefficient.
	 *
	 * This coefficient is the typical time constant of the system, which gives us an order of
	 * magnitude of its response time. Slower response time typically provides better noise immunity
	 * at the expense of higher latency.
	 *
	 * @param t
	 *            The time constant, in milliseconds.
	 * @throws ConnectionLostException
	 *             The connection with the IOIO is lost.
	 */
	public void setFilterCoef(float t) throws ConnectionLostException;

	/**
	 * Block until sensed capacitance becomes greater than a given threshold.
	 *
	 * For using a touch surface as a digital button, a threshold of 50pF is normally useful, with
	 * some hysteresis recommended.
	 *
	 * @param threshold
	 *            The threshold value, in pF units.
	 * @throws ConnectionLostException
	 *             The connection with the IOIO is lost.
	 * @throws InterruptedException
	 *             The calling thread has been interrupted.
	 */
	public void waitOver(float threshold) throws ConnectionLostException, InterruptedException;

	/**
	 * This is very similar to {@link #waitOver()}, but will wait for a new sample to arrive before
	 * returning. This is useful in conjunction with {@link IOIO#sync()}, in cases when we want to
	 * guarantee the we are looking at a sample that has been captured strictly after certain other
	 * commands have been executed.
	 *
	 * @return The capacitance, in pico-Farade units.
	 * @throws InterruptedException
	 *             The calling thread has been interrupted.
	 * @throws ConnectionLostException
	 *             The connection with the IOIO is lost.
	 * @see #waitOver()
	 */
	public void waitOverSync(float threshold) throws ConnectionLostException, InterruptedException;

	/**
	 * Block until sensed capacitance becomes less than a given threshold.
	 *
	 * For using a touch surface as a digital button, a threshold of 50pF is normally useful, with
	 * some hysteresis recommended.
	 *
	 * @param threshold
	 *            The threshold value, in pF units.
	 * @throws ConnectionLostException
	 *             The connection with the IOIO is lost.
	 * @throws InterruptedException
	 *             The calling thread has been interrupted.
	 */
	public void waitUnder(float threshold) throws ConnectionLostException, InterruptedException;

	/**
	 * This is very similar to {@link #waitUnder()}, but will wait for a new sample to arrive before
	 * returning. This is useful in conjunction with {@link IOIO#sync()}, in cases when we want to
	 * guarantee the we are looking at a sample that has been captured strictly after certain other
	 * commands have been executed.
	 *
	 * @return The capacitance, in pico-Farade units.
	 * @throws InterruptedException
	 *             The calling thread has been interrupted.
	 * @throws ConnectionLostException
	 *             The connection with the IOIO is lost.
	 * @see #waitUnder()
	 */
	public void waitUnderSync(float threshold) throws ConnectionLostException, InterruptedException;
}
