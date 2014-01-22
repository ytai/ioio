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
 * A waveform sequencer.
 * <p>
 * The {@link Sequencer} interface enables generation of very precisely-timed digital waveforms,
 * primarily targeted for different kinds of actuators, such as different kinds of motors, solenoids
 * and LEDs. The output comprises one or more channels, each of which can have one of several
 * different types. The output channels type and their assignments to pins are determined when the
 * {@link Sequencer} instance is created and cannot be changed during its lifetime. A
 * {@link Sequencer} instance is obtained via {@link IOIO#openSequencer(ChannelConfig[])}.
 * <p>
 * Each channel is configured with one of the {@link ChannelConfig} subclasses. For example,
 * {@link ChannelCueBinary} specified a binary output, useful for controlling an LED or a solenoid.
 * This configuration includes, among other things, the pin specification for this channel. See the
 * individual subclasses documentation for more information.
 * <p>
 * Once instantiated, the {@link Sequencer} gets fed with a stream of &lt;cue, duration&gt; pairs. A
 * cue is a set of instructions telling each of the channels what to do, or more precisely, one
 * {@link ChannelCue} per channel. The concrete type of the {@link ChannelCue} depends on the
 * channel type, for example, a channel that has been configured with {@link ChannelConfigBinary},
 * expects a {@link ChannelCueBinary} for its cue. In this example, the cue will specify whether
 * this output should be high or low. When a cue is combined with a duration, it has the semantics
 * of "for this much time, generate this waveform". A stream of such pairs can represent a complex
 * multi-channel waveform.
 * <p>
 * The above &lt;cue, duration&gt; pairs are pushed into the {@link Sequencer} via the
 * {@link #push(ChannelCue[], int)} method. They will not yet be executed, but rather queue up.
 * There is a limited number of cues that can be queued. Attempting to push more will block until a
 * cue has been executed. In order to avoid blocking (especially during the initial fill of the
 * queue - see below), the {@link #available()} method returns the number of pushes that can be done
 * without blocking. Once a few of them have been queued up, execution may start by calling
 * {@link #start()} . During execution, more cues can be pushed, and ideally fast enough so that
 * execution never needs to stall. During execution, {@link #pause()} will suspend execution as soon
 * as the currently executed cue is done, without clearing the queue of pending cues. Operation can
 * then be resumed by calling {@link #start()} again. Calling {@link #stop()} will immediately stop
 * execution and clear the queue. Execution progress can be tracked at any time by calling
 * {@link #numCuesStarted()}, which will increment every time actual execution of a cue has begun.
 * It will be reset back to 0 following a {@link #stop()}.
 *
 * <h4>Pre-fill</h4>
 * In order to avoid stalls immediately after {@link #start()}, it is recommended to pre-fill the
 * cue FIFO priorly. The recommended sequence of operations is:
 *
 * <pre>
 * // Open the sequencer.
 * Sequencer s = ioio_.openSequencer(...);
 * // At this point, the FIFO might still be at zero capacity, wait until opening is complete.
 * s.waitEventType(Sequencer.Event.Type.STOPPED);
 * // Now our FIFO is empty and at full capacity. Fill it entirely.
 * while (s.available() > 0) {
 *   s.push(...);
 * }
 * // Now we can start!
 * s.start();
 * </pre>
 *
 * <h4>Manual Operation</h4>
 * In some cases it is useful to be able to execute some cues while the {@link Sequencer} is paused
 * or stopped without having to clear the queue. For this purpose, the
 * {@link #manualStart(ChannelCue[])} command can be used. Calling it will immediately begin
 * execution of a given cue, without changing the queue previously given. Unlike cues scheduled via
 * {@link #push(ChannelCue[], int)}, these cues have no duration, and will keep executing until
 * {@link #manualStop()} is called. After that, normal operation can be resumed by calling
 * {@link #start()} again.
 *
 * <h4>States</h4>
 * This table summarizes the different states the {@link Sequencer} can be in, and the methods which
 * can be called to change it:
 * <table border="1">
 * <tr>
 * <th>Current State</th>
 * <th>Method</th>
 * <th>Next State</th>
 * </tr>
 * <tr>
 * <td rowspan="4">Idle</td>
 * <td>{@link #start()}</td>
 * <td>Running</td>
 * </tr>
 * <tr>
 * <td>{@link #manualStart(ChannelCue[])}</td>
 * <td>Manual</td>
 * </tr>
 * <tr>
 * <td>{@link #stop()}</td>
 * <td>Idle (and clear the queue)</td>
 * </tr>
 * <tr>
 * <td>{@link #manualStop()}</td>
 * <td>Idle (no-op)</td>
 * </tr>
 * <tr>
 * <td rowspan="2">Running</td>
 * <td>{@link #pause()}</td>
 * <td>Idle</td>
 * </tr>
 * <tr>
 * <td>{@link #stop()}</td>
 * <td>Idle (and clear the queue)</td>
 * </tr>
 * <tr>
 * <td rowspan="3">Manual</td>
 * <td>{@link #manualStop()}</td>
 * <td>Idle</td>
 * </tr>
 * <tr>
 * <td>{@link #stop()}</td>
 * <td>Idle (and clear the queue)</td>
 * </tr>
 * <tr>
 * <td>{@link #manualStart()}</td>
 * <td>Manual (new cue)</td>
 * </tr>
 * </table>
 *
 * <h4>Clocking</h4>
 * All the timing information provided to a single channel is based on a clock which implies a
 * time-base. For some channel types, the clock is determined upon instantiation; for others it can
 * be set on a per-cue basis. Once a clock rate is set, all timing values used for that channel are
 * in units of this time-base. Using a faster clock rate enables faster waveforms to be generated as
 * well as have better timing resolution for everything. However, since most timing values used in
 * this API have a limited range, using a faster clock would also limit the maximum duration of some
 * signals. As a rule of thumb, choosing the highest possible clock rate that can satisfy the
 * duration requirements is the right choice.
 *
 * <h4>Stalls</h4>
 * It is possible that the client does not push cues fast enough to keep up with execution. In that
 * case, the queue will drain and execution will stall. All channel types have well-defined
 * behaviors in case of a stall, for example, a binary channel allows the user to explicitly
 * determine whether it should be go back to its initial value or retain the current value when
 * stalled. Once stalled, as soon as a new cue gets pushed, operation will resume immediately
 * without having to call {@link #start()}.
 *
 * <h4>Keeping track of execution</h4>
 * Since execution of queued events is asynchronous, it is sometimes required to track their
 * execution progress, for example, for keeping a user interface synchronized with the actual state
 * of a multi-axis machine.
 * <p>
 * The client can poll for the last event that occured via {@link #getLastEvent()}, or block until
 * the next one arrives using {@link #waitEvent()}. The latter features a limited-size queue, so
 * events are not lost as long as the client reads continuously. The queue is initially 32-events
 * long, but could be changed using {@link #setEventQueueSize(int)}. The sequencer will report the
 * following kinds of events, via the {@link Event} type:
 * <dl>
 * <dt>STOPPED</dt>
 * <dd>The sequencer has been stopped (and the cue FIFO is now empty). This is also the event that
 * is returned when calling {@link #getLastEvent()} before any event has arrived.</dd>
 * <dt>CUE_STARTED</dt>
 * <dd>A new cue has just started execution.</dd>
 * <dt>PAUSED</dt>
 * <dd>A cue has just finished execution and progress has been paused as result of an explicit pause
 * request.</dd>
 * <dt>STALLED</dt>
 * <dd>A cue has just finished execution and progress has been paused as result of the queue running
 * empty. In this case, the state of the sequencer does not change (i.e. it is still Running), and
 * pushing addition events will immediately resume execution.</dd>
 * <dt>CLOSED</dt>
 * <dd>The sequencer has been closed. This is mostly intended for gracefully exiting a thread which
 * is constantly blocking on {@link #waitEvent()}</dd>
 * </dl>
 * <p>
 *
 */
public interface Sequencer extends Closeable {
	/**
	 * A clock rate selection, which implies a time-base.
	 */
	public enum Clock {
		/** 16 MHz (62.5ns time-base). */
		CLK_16M,
		/** 2 MHz (0.5us time-base). */
		CLK_2M,
		/** 250 KHz (4us time-base). */
		CLK_250K,
		/** 62.5 KHz (16us time-base). */
		CLK_62K5
	}

	/**
	 * A sequencer event. Used for tracking execution progress.
	 */
	public class Event {
		/**
		 * Event type.
		 */
		public enum Type {
			/**
			 * The sequencer has been stopped or never started. This will always be accompanied by a
			 * numCuesStarted of 0. This event will also be the first event appearing on the event
			 * queue to designate the the sequencer has been opened and the cue FIFO is at its full
			 * capacity for pushing messages. This is useful if the client wants to pre-fill the
			 * FIFO in order to avoid stalls.
			 */
			STOPPED,
			/**
			 * A new cue has started executing.
			 */
			CUE_STARTED,
			/**
			 * A cue has ended execution and the sequencer is idle as result of a pause request.
			 */
			PAUSED,
			/**
			 * A cue has ended execution and the sequencer is idle as result of the queue becoming
			 * empty.
			 */
			STALLED,
			/**
			 * This event type is only sent once, when the sequencer has been closed. It is mostly
			 * intended to release a client blocking on {@link Sequencer#waitEvent()}. It is also
			 * used if {@link Sequencer#getLastEvent()} is called before any event has been sent.
			 */
			CLOSED
		}

		/**
		 * The event type.
		 */
		public final Type type;

		/**
		 * This gives a counter of the number cues started execution until now, which is
		 * synchronized with the events. When the first cue begins execution, the event will be
		 * CUE_STARTED and numCuesStarted will be 1.
		 */
		public final int numCuesStarted;

		/**
		 * Constructor.
		 */
		public Event(Type t, int n) {
			type = t;
			numCuesStarted = n;
		}
	}

	/**
	 * A marker interface for channel configurations. A concrete instance of this describes the
	 * configuration of a single channel.
	 */
	public static interface ChannelConfig {
	}

	/**
	 * Configuration for a channel of type PWM Position.
	 * <p>
	 * PWM position channels are channels in which a PWM signal is generated, and the pulse width
	 * controls the position of the actuator. A good example is a hobby servo motor. The main
	 * difference from a PWM speed channel is that the position channel will retain its pulse width
	 * during a stall event.
	 */
	public static class ChannelConfigPwmPosition implements ChannelConfig {
		/**
		 * Specification of the output pin(s) for this channel.
		 */
		public final DigitalOutput.Spec[] pinSpec;

		/**
		 * The clock rate for this channel (cannot be changed on a per-cue basis).
		 */
		public final Clock clk;

		/**
		 * The PWM period, in time-base units, determined by {@link #clk}. Valid values are
		 * [2..65536].
		 */
		public final int period;

		/**
		 * The initial pulse width (before any cue is executed), in time-base units, determined by
		 * {@link #clk}. Valid values are 0 or [2..65536].
		 */
		public final int initialPulseWidth;

		/**
		 * Constructor.
		 * <p>
		 *
		 * @param clk
		 *            See {@link #clk}.
		 * @param period
		 *            See {@link #period}.
		 * @param initialPulseWidth
		 *            See {@link #initialPulseWidth}.
		 * @param pinSpec
		 *            See {@link #pinSpec}.
		 */
		public ChannelConfigPwmPosition(Clock clk, int period, int initialPulseWidth,
				DigitalOutput.Spec... pinSpec) {
			if (period < 2 || period > (1 << 16)) {
				throw new IllegalArgumentException("Period width must be between [2..65536]");
			}
			if (initialPulseWidth != 0 && (initialPulseWidth < 2 || initialPulseWidth > (1 << 16))) {
				throw new IllegalArgumentException(
						"Initial pulse width must be 0 or between [2..65536]");
			}
			this.pinSpec = pinSpec;
			this.clk = clk;
			this.period = period;
			this.initialPulseWidth = initialPulseWidth;
		}
	}

	/**
	 * Configuration for a channel of type PWM speed.
	 * <p>
	 * PWM speed channels are channels in which a PWM signal is generated, and the pulse width
	 * controls the speed of the actuator. A good example is a DC motor. The main difference from a
	 * PWM position channel is that the position channel will go back to its initial pulse width
	 * during a stall event.
	 */
	public static class ChannelConfigPwmSpeed implements ChannelConfig {
		/**
		 * Specification of the output pin(s) for this channel.
		 */
		public final DigitalOutput.Spec[] pinSpec;

		/**
		 * The clock rate for this channel (cannot be changed on a per-cue basis).
		 */
		public final Clock clk;

		/**
		 * The PWM period, in time-base units, determined by {@link #clk}. Valid values are
		 * [2..65536].
		 */
		public final int period;

		/**
		 * The initial pulse width (before any cue is executed), in time-base units, determined by
		 * {@link #clk}. Valid values are 0 or [2..65536]. Also used in the event of a stall.
		 */
		public final int initialPulseWidth;

		/**
		 * Constructor.
		 * <p>
		 *
		 * @param clk
		 *            See {@link #clk}.
		 * @param period
		 *            See {@link #period}.
		 * @param initialPulseWidth
		 *            See {@link #initialPulseWidth}.
		 * @param pinSpec
		 *            See {@link #pinSpec}.
		 */
		public ChannelConfigPwmSpeed(Clock clk, int period, int initialPulseWidth,
				DigitalOutput.Spec... pinSpec) {
			if (period < 2 || period > (1 << 16)) {
				throw new IllegalArgumentException("Period width must be between [2..65536]");
			}
			if (initialPulseWidth != 0 && (initialPulseWidth < 2 || initialPulseWidth > (1 << 16))) {
				throw new IllegalArgumentException(
						"Initial pulse width must be 0 or between [2..65536]");
			}
			this.pinSpec = pinSpec;
			this.clk = clk;
			this.period = period;
			this.initialPulseWidth = initialPulseWidth;
		}
	}

	/**
	 * Configuration for a channel of type FM speed.
	 * <p>
	 * FM speed channels are channels in which fixed-width pulses are generated with varying
	 * frequency, which corresponds to the actuator speed. A good example is a stepper motor in an
	 * application which requires speed control and not position control (for the latter see
	 * {@link ChannelConfigSteps}). An FM speed channel will idle (not produce any pulses) during a
	 * stall event.
	 */
	public static class ChannelConfigFmSpeed implements ChannelConfig {
		/**
		 * Specification of the output pin(s) for this channel.
		 */
		public final DigitalOutput.Spec[] pinSpec;

		/**
		 * The clock rate for this channel (cannot be changed on a per-cue basis).
		 */
		public final Clock clk;

		/**
		 * The width (duration) of each pulse, in time-base units, determined by {@link #clk}. Valid
		 * values are [2..65536].
		 */
		public final int pulseWidth;

		/**
		 * Constructor.
		 * <p>
		 *
		 * @param clk
		 *            See {@link #clk}.
		 * @param pulseWidth
		 *            See {@link #pulseWidth}.
		 * @param pinSpec
		 *            See {@link #pinSpec}.
		 */
		public ChannelConfigFmSpeed(Clock clk, int pulseWidth, DigitalOutput.Spec... pinSpec) {
			if (pulseWidth < 2 || pulseWidth > (1 << 16)) {
				throw new IllegalArgumentException("Pulse width must be between [2..65536]");
			}
			this.pinSpec = pinSpec;
			this.clk = clk;
			this.pulseWidth = pulseWidth;
		}
	}

	/**
	 * Configuration for a channel of type steps.
	 * <p>
	 * Steps channels are channels in which fixed-width pulses are generated with varying frequency,
	 * which corresponds to the actuator speed. A good example is a stepper motor in an application
	 * which requires position control. Unlike the FM speed channel, steps channels will generate a
	 * deterministic number of steps in a given duration, as well as allow changing the time-base on
	 * every cue. See {@link ChannelCueSteps} for a discussion on number of steps calculation. A
	 * steps channel will idle (not produce any pulses) during a stall event.
	 */
	public static class ChannelConfigSteps implements ChannelConfig {
		/**
		 * Specification of the output pin(s) for this channel.
		 */
		public final DigitalOutput.Spec[] pinSpec;

		/**
		 * Constructor.
		 * <p>
		 *
		 * @param pinSpec
		 *            See {@link #pinSpec}.
		 */
		public ChannelConfigSteps(DigitalOutput.Spec... pinSpec) {
			this.pinSpec = pinSpec;
		}
	}

	/**
	 * Configuration for a binary channel.
	 * <p>
	 * A binary channel is a simple digital output, which is driven in synchronization with the
	 * sequence. Solenoids, DC motors running at full speed (no PWM) or LED are all examples for
	 * actuators that can be controlled by a binary channel. During a stall event, the channel can
	 * be configured to either retain its last state, or go to its initial state.
	 */
	public static class ChannelConfigBinary implements ChannelConfig {
		/**
		 * Specification of the output pin(s) for this channel.
		 */
		public final DigitalOutput.Spec pinSpec;

		/**
		 * Initial value for this channel (true = HIGH, false = LOW).
		 */
		public final boolean initialValue;

		/**
		 * When true, channel will go to initial state when stalled or stopped. Otherwise, channel
		 * will retain its last state.
		 */
		public final boolean initWhenIdle;

		/**
		 * Constructor.
		 * <p>
		 *
		 * @param initialValue
		 *            See {@link #initialValue}.
		 * @param initWhenIdle
		 *            See {@link #initWhenIdle}.
		 * @param pinSpec
		 *            See {@link #pinSpec}.
		 */
		public ChannelConfigBinary(boolean initialValue, boolean initWhenIdle,
				DigitalOutput.Spec pinSpec) {
			this.pinSpec = pinSpec;
			this.initialValue = initialValue;
			this.initWhenIdle = initWhenIdle;
		}
	}

	/**
	 * A marker interface for channel cues. A concrete instance of this describes a single cue of a
	 * single channel.
	 */
	public static interface ChannelCue {
	}

	/**
	 * A cue for a PWM position channel.
	 * <p>
	 * Determines what is going to be the pulse width will while cue is executing.
	 */
	public static class ChannelCuePwmPosition implements ChannelCue {
		/**
		 * The pulse-width, in time-base units, as determined for this channel in its configuration.
		 * Valid values are 0 or [2..65536].
		 */
		public int pulseWidth;
	}

	/**
	 * A cue for a PWM speed channel.
	 * <p>
	 * Determines what is going to be the pulse width while this cue is executing.
	 */
	public static class ChannelCuePwmSpeed implements ChannelCue {
		/**
		 * The pulse-width, in time-base units, as determined for this channel in its configuration.
		 * Valid values are 0 or [2..65536].
		 */
		public int pulseWidth;
	}

	/**
	 * A cue for a FM speed channel.
	 * <p>
	 * Determines what is going to be the period duration (frequency) while this cue is executing.
	 */
	public static class ChannelCueFmSpeed implements ChannelCue {
		/**
		 * The pulse period, in time-base units, as determined for this channel in its
		 * configuration. Valid values are [0..65536]. Note that:
		 * <ul>
		 * <li>A period of 0 will result in no pulses generated for this cue.</li>
		 * <li>A non-0 period smaller than the pulse duration will result in the output constantly
		 * high.</li>
		 * </ul>
		 */
		public int period;
	}

	/**
	 * A cue for a steps channel.
	 * <p>
	 * Determines the clock rate, pulse width and period durations while this cue is executing. This
	 * kind of channel produces deterministic waveforms, which are typically used to generate a
	 * precise number of steps during a given cue period. However, this comes at a cost of being a
	 * little more involved from the user's perspective, since delicate timing considerations need
	 * to be taken into account.
	 * <p>
	 * The number of steps within a given cue period is given by floor(Tc / Ts), where Tc is the cue
	 * duration and Ts is the step period duration. Each pulse is center-aligned within its period.
	 * In order to maintain a deterministic result, the user must guarantee that no pulse falls
	 * within the last 6 microseconds of the cue period (this effectively limits the maximum pulse
	 * rate to 80[kHz], considering the the pulse itself must be at least 1/8[us] wide, and that the
	 * rising edge of the pulse is center-aligned). Thus, it is possible that due to precision
	 * limitations, in an arbitrarily long period it will be impossible to generate the exact number
	 * of desired pulses. Likewise, a very low pulse rate (high pulse duration) may be outside of
	 * the permitted range or will result in having to use a slower time-base. The solution to both
	 * problems is splitting a single cue into two or more cues of shorted durations, until
	 * eventually the precision is sufficient (this always converges, since eventually we can always
	 * go to arbitrarily short cue durations, so that each one contains either zero or one steps.
	 * <p>
	 * A steps channel allows determining the clock rate on a per-cue basis. This often allows
	 * avoiding having to split cues, thus resulting in a less total cues and more efficient
	 * execution. The rule for choosing the correct clock is to always use the highest rate that
	 * will cause the resulting period to be <= 2^16. In other words, choose the highest available
	 * clock which is less than or equal to (2^16 / Tp) or (2^16 * Fp), where Tp is the desired
	 * period in seconds, or Fp is the desired frequency in Hz. For example, if we want to generate
	 * pulses at 51Hz, 65536 * Fp = 3.34MHz, so we should use the 2MHz clock, and the period value
	 * will be round(2MHz / 51Hz) = 39216. This result in an actual rate of 2MHz / 39216 ~=
	 * 50.9996[Hz].
	 */
	public static class ChannelCueSteps implements ChannelCue {
		/**
		 * The clock rate for this cue. See discussion in the class documentation on how to choose
		 * it.
		 */
		public Clock clk;

		/**
		 * The pulse-width, in time-base units, as determined for this channel in its configuration.
		 * Valid values are [0..floor(period / 2)].
		 */
		public int pulseWidth;

		/**
		 * The pulse period, in time-base units, as determined for this channel in its
		 * configuration. Valid values are [3..65536].
		 */
		public int period;
	}

	/**
	 * A cue for a binary channel.
	 * <p>
	 * This cue determines whether the output should be high or low while the cue is executing.
	 */
	public static class ChannelCueBinary implements ChannelCue {
		/**
		 * The desired output state (true = high, false = low).
		 */
		public boolean value;
	}

	/**
	 * Push a timed cue to the sequencer.
	 * <p>
	 * This method will block until there is at least one free space in the FIFO (which may be
	 * forever if the sequencer is not running -- use {@link Sequencer.available()} first in this
	 * case). Then, it will queue the cue for execution.
	 *
	 * @param cues
	 *            An array of channel cues. Has to be the exact same length as the
	 *            {@link ChannelConfig} array that was used to configure the sequencer. Each
	 *            element's type should be the counterpart of the corresponding configuration type.
	 *            For example, it element number 5 in the configuration array was of type
	 *            {@link Sequencer.ChannleConfigBinary}, then cues[5] needs to be of type
	 *            {@link Sequencer.ChannelCueBinary}
	 * @param duration
	 *            The time duration for which this cue is to be executed, before moving to the next
	 *            cue (or stalling). The units are 16 microseconds. For example, passing 10 here
	 *            would mean a duration of 160 microseconds. Valid values are [2..65536] (approx.
	 *            1.05 seconds).
	 * @throws ConnectionLostException
	 *             Connection to the IOIO was lost before or during this operation.
	 * @throws InterruptedException
	 *             The operation was interrupted before completion.
	 */
	public void push(ChannelCue[] cues, int duration) throws ConnectionLostException,
			InterruptedException;

	/**
	 * Execute a cue until further notice.
	 * <p>
	 * This method may only be called when the sequencer is not in the Running state. It will not
	 * affect the queue of pending timed-cues previously filled via {@link #push(ChannelCue[], int)}
	 * calls. The cue will be executed until explicitly stopped via {@link #manualStop()}. A
	 * subsequent call to {@link #manualStart(ChannelCue[])} can be used to immediately have a new
	 * cue take into effect.
	 *
	 * @param cues
	 *            An array of channel cues to execute. See the description of the same argument in
	 *            {@link #push(ChannelCue[], int)} for details.
	 * @throws ConnectionLostException
	 *             Connection to the IOIO was lost before or during this operation.
	 */
	public void manualStart(ChannelCue[] cues) throws ConnectionLostException;

	/**
	 * Stop a manual cue currently running.
	 * <p>
	 * This may be called only when a the sequencer is not in the Running state, typically in the
	 * Manual state, as result of a previous {@link #manualStart(ChannelCue[])}. This causes the
	 * execution to stop immediately and the sequencer is now back in paused mode, ready for another
	 * manual cue or for resuming execution of its previously queued sequence. Calling while in the
	 * Idle state is legal, but does nothing.
	 *
	 * @throws ConnectionLostException
	 *             Connection to the IOIO was lost before or during this operation.
	 */
	public void manualStop() throws ConnectionLostException;

	/**
	 * Start execution of the sequence.
	 * <p>
	 * This method will cause any previously queued cues (via {@link #push(ChannelCue[], int)}) to
	 * start execution in order of pushing, according to their specified timings. The sequencer must
	 * be paused before calling this method.
	 *
	 * @throws ConnectionLostException
	 *             Connection to the IOIO was lost before or during this operation.
	 */
	public void start() throws ConnectionLostException;

	/**
	 * Pause execution of the sequence.
	 * <p>
	 * This method can be called when the sequencer is running, as result of previous invocation of
	 * {@link #start()}. It will cause execution to suspend as soon as the currently executing cue
	 * is done. The queue of pending cues will not be affected, and operation can be resumed
	 * seamlessly by calling {@link #start()} again.
	 *
	 * @throws ConnectionLostException
	 *             Connection to the IOIO was lost before or during this operation.
	 */
	public void pause() throws ConnectionLostException;

	/**
	 * Stop execution of the sequence.
	 * <p>
	 * This will cause the sequence execution to stop immediately (without waiting for the current
	 * cue to complete). All previously queued cues will be discarded. The sequence will then go to
	 * paused state.
	 *
	 * @throws ConnectionLostException
	 *             Connection to the IOIO was lost before or during this operation.
	 */
	public void stop() throws ConnectionLostException;

	/**
	 * Get the number of cues which can be pushed without blocking.
	 * <p>
	 * This is useful for pre-filling the queue before starting the sequencer, or if the client
	 * wants to do other operations on the same thread that pushes cues. The value returned will
	 * indicate how many calls to {@link #push(ChannelCue[], int)} can be completed without
	 * blocking.
	 *
	 * @return The number of available slots in the cue FIFO.
	 */
	public int available() throws ConnectionLostException;;

	/**
	 * Get the most recent execution event.
	 * <p>
	 * This includes the event type and the number of cues that started executing, since opening the
	 * sequencer or the last call to {@link #stop()}. Immediately after opening the sequencer, the
	 * event type may be {@link Event.Type.CLOSED}, and as soon as the sequencer finished opening an
	 * {@link Event.Type.STOPPED} will be sent.
	 *
	 * @return The last event.
	 *
	 * @throws ConnectionLostException
	 *             Connection to the IOIO was lost before or during this operation.
	 */
	public Event getLastEvent() throws ConnectionLostException;

	/**
	 * Waits until an execution event occurs and returns it.
	 * <p>
	 * In case the client is not reading fast enough, older events will be discarded as new once
	 * arrive, so that the queue always stores the most recent events.
	 *
	 * @throws ConnectionLostException
	 *             Connection to the IOIO was lost before or during this operation.
	 * @throws InterruptedException
	 *             The operation was interrupted before completion.
	 */
	public Event waitEvent() throws ConnectionLostException, InterruptedException;

	/**
	 * A convenience method for blocking until an event of a certain type appears on the event
	 * queue. All events proceeding this event type, including the event of the requested type will
	 * be removed from the queue and discarded.
	 *
	 * @throws ConnectionLostException
	 *             Connection to the IOIO was lost before or during this operation.
	 * @throws InterruptedException
	 *             The operation was interrupted before completion.
	 */
	public void waitEventType(Event.Type type) throws ConnectionLostException, InterruptedException;

	/**
	 * Sets a new size for the incoming event queue.
	 * <p>
	 * Initially the size of the queue is 32, which should suffice for most purposes. If, however, a
	 * client is not able to read frequently enough to not miss events, increasing the size is an
	 * option.
	 * <p>
	 * Any pending events will be discarded. It is recommended to call this method only once,
	 * immediately after opening the sequencer.
	 *
	 * @param size
	 *            The new queue size.
	 * @throws ConnectionLostException
	 *             Connection to the IOIO was lost before or during this operation.
	 */
	public void setEventQueueSize(int size) throws ConnectionLostException;
}
