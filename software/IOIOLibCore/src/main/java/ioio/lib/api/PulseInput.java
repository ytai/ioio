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
 * An interface for pulse width and frequency measurements of digital signals.
 * <p>
 * PulseInput (commonly known as "input capture") is a versatile module which enables extraction of
 * various timing information from a digital signal. There are two main use cases: pulse duration
 * measurement and frequency measurement. In pulse width measurement we measure the duration of a
 * positive ("high") or negative ("low") pulse, i.e. the elapsed time between a rise and a fall or
 * vice versa. This mode is useful, for example, for decoding a PWM signal or measuring the delay of
 * a sonar return signal. In frequency measurement we measure the duration between a rising edge to
 * the following rising edge. This gives us a momentary reading of a signal's frequency or period.
 * This is commonly used, for example, in conjunction with an optical or magnetic sensor for
 * measuring a turning shaft's speed.
 * <p>
 * {@link PulseInput} instances are obtained by calling
 * {@link IOIO#openPulseInput(ioio.lib.api.DigitalInput.Spec, ioio.lib.api.PulseInput.ClockRate, ioio.lib.api.PulseInput.PulseMode, boolean)}
 * . Once created, the client would typically use {@link #getDuration()} for getting the most recent
 * duration reading (either pulse width or period), or {@link #getFrequency()} for getting the most
 * recent frequency reading (available only in frequency mode).
 * <p>
 * When creating the instance, some important configuration decisions have to be made: the precision
 * (single or double), the clock rate and the mode of operation. Modes are straightforward:
 * {@link PulseMode#POSITIVE} is used for measuring a positive pulse, {@link PulseMode#NEGATIVE} a
 * negative pulse, and {@link PulseMode#FREQ} / {@link PulseMode#FREQ_SCALE_4} /
 * {@link PulseMode#FREQ_SCALE_16} are used for measuring frequency. The difference between the
 * three scaling modes is that without scaling, the frequency is determined by measurement of a
 * single (rising-edge-to-rising-edge) period. In x4 scaling, 4 consecutive periods are measured and
 * the time is divided by 4, providing some smoothing as well as better resolution. Similarly for
 * x16 scaling. Note that scaling affects the range of signals to be measured, as discussed below.
 * <p>
 * The choice of single vs. double-precision is important to understand: IOIO internally uses either
 * 16-bit counters or 32-bit counters for the timing. 16- counters force us to either limit the
 * maximum duration (and the minimum frequency) or compromise accuracy as compared to 32-bit
 * counters. However, if you need many concurrent pulse measurements in your application, you may
 * have no choice but to use single-precision.
 * <p>
 * The clock rate selection is important (and even critical when working in single-precision) and
 * requires the user to make some assumptions about the nature of the measured signal. The higher
 * the clock rate, the more precise the measurement, but the longest pulse that can be measured
 * decreases (or lowest frequency that can be measured increases). Using the scaling option when
 * operating in frequency mode also affects these sizes. combinations. It is always recommended to
 * choose the most precise mode, which exceeds the maximum expected pulse width (or inverse
 * frequency). If a pulse is received whom duration exceeds the longest allowed pulse, it will be
 * "folded" into the valid range and product garbage readings.
 * <p>
 * The following table (sorted by longest pulse) summarizes all possible clock / mode combinations.
 * The table applies for <b>single-precision</b> operation. For double-precision, simply multiply
 * the longest pulse by 65536 and divide the lowest frequency by the same amount. Interestingly, the
 * number written in [ms] units in the longest pulse column, roughly corresponds to the same number
 * in minutes when working with double precsion, since 1[min] = 60000[ms].
 * <table border="1">
 * <tr>
 * <th>Clock</th>
 * <th>Scaling</th>
 * <th>Resolution</th>
 * <th>Longest pulse</th>
 * <th>Lowest frequency</th>
 * </tr>
 * <tr>
 * <td>62.5KHz</td>
 * <td>1</td>
 * <td>16us</td>
 * <td>1.048s</td>
 * <td>0.95Hz</td>
 * </tr>
 * <tr>
 * <td>250KHz</td>
 * <td>1</td>
 * <td>4us</td>
 * <td>262.1ms</td>
 * <td>3.81Hz</td>
 * </tr>
 * <tr>
 * <td>62.5KHz</td>
 * <td>4</td>
 * <td>4us</td>
 * <td>262.1ms</td>
 * <td>3.81Hz</td>
 * </tr>
 * <tr>
 * <td>250KHz</td>
 * <td>4</td>
 * <td>1us</td>
 * <td>65.54ms</td>
 * <td>15.26Hz</td>
 * </tr>
 * <tr>
 * <td>62.5KHz</td>
 * <td>16</td>
 * <td>1us</td>
 * <td>65.54ms</td>
 * <td>15.26Hz</td>
 * </tr>
 * <tr>
 * <td>2MHz</td>
 * <td>1</td>
 * <td>500ns</td>
 * <td>32.77ms</td>
 * <td>30.52Hz</td>
 * </tr>
 * <tr>
 * <td>250KHz</td>
 * <td>16</td>
 * <td>250us</td>
 * <td>16.38ms</td>
 * <td>61.0Hz</td>
 * </tr>
 * <tr>
 * <td>2MHz</td>
 * <td>4</td>
 * <td>125ns</td>
 * <td>8.192ms</td>
 * <td>122.1Hz</td>
 * </tr>
 * <tr>
 * <td>16MHz</td>
 * <td>1</td>
 * <td>62.5ns</td>
 * <td>4.096ms</td>
 * <td>244.1Hz</td>
 * </tr>
 * <tr>
 * <td>2MHz</td>
 * <td>16</td>
 * <td>31.25ns</td>
 * <td>2.048ms</td>
 * <td>488.3Hz</td>
 * </tr>
 * <tr>
 * <td>16MHz</td>
 * <td>4</td>
 * <td>15.6ns</td>
 * <td>1.024ms</td>
 * <td>976.6Hz</td>
 * </tr>
 * <tr>
 * <td>16MHz</td>
 * <td>16</td>
 * <td>3.9ns</td>
 * <td>256us</td>
 * <td>3.906KHz</td>
 * </tr>
 * </table>
 *
 * <p>
 * In some applications it is desirable to measure every incoming pulse rather than repetitively
 * query the result of the last measurement. For that purpose the {@link #getDurationBuffered()}
 * method exists: every incoming pulse width is pushed into a small internal queue from which it can
 * be read. The client waits for data to be available, then reads it and data that comes in in the
 * meanwhile is stored. The queue has limited size, so it is important to read quickly if no pulses
 * are to be lost. Note that once a pulse is detected, the next one must have its leading edge at
 * least 5ms after the leading edge of the current one, or else it will be skipped. This throttling
 * has been introduced on purpose, in order to prevent saturation the communication channel when the
 * input signal is very high frequency. Effectively, this means that the maximum sample rate is
 * 200Hz. This rate has been chosen as it enables measuring R/C servo signals without missing
 * pulses.
 * <p>
 * In other circumstances, the client may want to guarantee that a certain sample has been observed
 * strictly <i>after</i> certain other data has been received from the IOIO. For that purpose the
 * {@link #getDurationSync()} and {@link #getFrequencySync()} variants exist. They are can be used
 * to impose strict ordering between samples captured by different inputs, or relative to setting of
 * outputs, using {@link IOIO#sync()}.
 * <p>
 * Typical usage (servo signal pulse width measurement):
 *
 * <pre>
 * {@code
 * // Open pulse input at 16MHz, double-precision
 * PulseInput in = ioio.openPulseInput(3, PulseMode.POSITIVE);
 * ...
 * float widthSec = in.getDuration();
 * OR:
 * float widthSec = in.getDurationBuffered();
 * ...
 * in.close();  // pin 3 can now be used for something else.
 * }
 * </pre>
 *
 * <p>
 * Typical usage (frequency measurement):
 *
 * <pre>
 * {@code
 * // Signal is known to be slightly over 150Hz. Single precision can be used.
 * PulseInput in = ioio.openPulseInput(3,
 *                                     ClockRate.RATE_2MHz,
 *                                     PulseMode.FREQ_SCALE_4,
 *                                     false);
 * ...
 * float freqHz = in.getFrequency();
 * ...
 * in.close();  // pin 3 can now be used for something else.
 * }
 * </pre>
 */
public interface PulseInput extends Closeable {
	/** An enumeration for describing the module's operating mode. */
	public enum PulseMode {
		/** Positive pulse measurement (rising-edge-to-falling-edge). */
		POSITIVE(1),
		/** Negative pulse measurement (falling-edge-to-rising-edge). */
		NEGATIVE(1),
		/** Frequency measurement (rising-edge-to-rising-edge). */
		FREQ(1),
		/** Frequency measurement (rising-edge-to-rising-edge) with 4x scaling. */
		FREQ_SCALE_4(4),
		/** Frequency measurement (rising-edge-to-rising-edge) with 16x scaling. */
		FREQ_SCALE_16(16);

		/** The scaling factor as an integer. */
		public final int scaling;

		private PulseMode(int s) {
			scaling = s;
		}
	}

	/** Supported clock rate enum. */
	public enum ClockRate {
		/** 16MHz */
		RATE_16MHz(16000000),
		/** 2MHz */
		RATE_2MHz(2000000),
		/** 250KHz */
		RATE_250KHz(250000),
		/** 62.5KHz */
		RATE_62KHz(62500);

		/** The value in Hertz units. */
		public final int hertz;

		private ClockRate(int h) {
			hertz = h;
		}
	}

	/**
	 * Gets the pulse duration in case of pulse measurement mode, or the period in case of frequency
	 * mode. When scaling is used, this is compensated for here, so the duration of a single cycle
	 * will be returned.
	 * <p>
	 * The first call to this method may block shortly until the first data update arrives. The
	 * client may interrupt the calling thread.
	 *
	 * @return The duration, in seconds.
	 * @throws InterruptedException
	 *             The calling thread has been interrupted.
	 * @throws ConnectionLostException
	 *             The connection with the IOIO has been lost.
	 */
	public float getDuration() throws InterruptedException, ConnectionLostException;

	/**
	 * This is very similar to {@link #getDuration()}, but will wait for a new sample to arrive
	 * before returning. This is useful in conjunction with {@link IOIO#sync()}, in cases when we
	 * want to guarantee the we are looking at a sample that has been captured strictly after
	 * certain other commands have been executed.
	 *
	 * @return The duration, in seconds.
	 * @throws InterruptedException
	 *             The calling thread has been interrupted.
	 * @throws ConnectionLostException
	 *             The connection with the IOIO is lost.
	 * @see #getDuration()
	 */
	public float getDurationSync() throws InterruptedException, ConnectionLostException;

	/**
	 * Reads a single measurement from the queue. If the queue is empty, will block until more data
	 * arrives. The calling thread may be interrupted in order to abort the call. See interface
	 * documentation for further explanation regarding the read queue.
	 * <p>
	 * This method may not be used if the interface has was opened in frequency mode.
	 *
	 * @return The duration, in seconds.
	 * @throws InterruptedException
	 *             The calling thread has been interrupted.
	 * @throws ConnectionLostException
	 *             The connection with the IOIO has been lost.
	 */
	public float getDurationBuffered() throws InterruptedException, ConnectionLostException;

	/**
	 * @deprecated Please use {@link #getDurationBuffered()} instead.
	 */
	public float waitPulseGetDuration() throws InterruptedException, ConnectionLostException;

	/**
	 * Gets the momentary frequency of the measured signal. When scaling is used, this is
	 * compensated for here, so the true frequency of the signal will be returned.
	 * <p>
	 * The first call to this method may block shortly until the first data update arrives. The
	 * client may interrupt the calling thread. - *
	 * <p>
	 * This method may only be used if the interface has been opened in frequency mode.
	 *
	 * @return The frequency, in Hz.
	 * @throws InterruptedException
	 *             The calling thread has been interrupted.
	 * @throws ConnectionLostException
	 *             The connection with the IOIO has been lost.
	 */
	public float getFrequency() throws InterruptedException, ConnectionLostException;

	/**
	 * This is very similar to {@link #getFrequency()}, but will wait for a new sample to arrive
	 * before returning. This is useful in conjunction with {@link IOIO#sync()}, in cases when we
	 * want to guarantee the we are looking at a sample that has been captured strictly after
	 * certain other commands have been executed.
	 *
	 * @return The frequency, in Hz.
	 * @throws InterruptedException
	 *             The calling thread has been interrupted.
	 * @throws ConnectionLostException
	 *             The connection with the IOIO is lost.
	 * @see #getFrequency()
	 */
	public float getFrequencySync() throws InterruptedException, ConnectionLostException;
}
