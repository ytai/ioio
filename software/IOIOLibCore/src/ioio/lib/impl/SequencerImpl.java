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
package ioio.lib.impl;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.Sequencer;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.SequencerEventListener;
import ioio.lib.impl.InterruptibleQueue.Nudged;
import ioio.lib.impl.ResourceManager.Resource;
import ioio.lib.impl.ResourceManager.ResourceType;
import ioio.lib.spi.Log;

import java.io.IOException;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.LinkedList;
import java.util.List;

public class SequencerImpl extends AbstractResource implements Sequencer, SequencerEventListener {
	private static final String TAG = "SequencerImpl";

	private enum RemoteState {
		IDLE_OR_MANUAL, RUNNING, STALLED, CLOSED
	}

	private enum LocalState {
		IDLE, RUNNING, MANUAL, CLOSED
	}

	// A mapping between the ChannelConfig sub-types to their respective
	// Channel sub-types.
	private static class ClassMapEntry {
		public final Class<? extends ChannelConfig> configClass;
		public final Class<? extends Channel> channelClass;

		public ClassMapEntry(Class<? extends ChannelConfig> configClass,
				Class<? extends Channel> channelClass) {
			this.configClass = configClass;
			this.channelClass = channelClass;
		}
	}

	private static final ClassMapEntry[] CLASS_MAP = {
			new ClassMapEntry(ChannelConfigBinary.class, ChannelBinary.class),
			new ClassMapEntry(ChannelConfigPwmPosition.class, ChannelPwmPosition.class),
			new ClassMapEntry(ChannelConfigPwmSpeed.class, ChannelPwmSpeed.class),
			new ClassMapEntry(ChannelConfigFmSpeed.class, ChannelFmSpeed.class),
			new ClassMapEntry(ChannelConfigSteps.class, ChannelSteps.class) };

	private final Channel[] channels_;
	private final List<Resource> pins_ = new LinkedList<ResourceManager.Resource>();
	private final List<Resource> ocs_ = new LinkedList<ResourceManager.Resource>();
	private final Resource sequencer_ = new Resource(ResourceType.SEQUENCER);
	int availableSlots_ = 0;
	int numCuesStarted_ = 0;
	private byte[] serializedBuf_ = new byte[68];
	private LocalState localState_ = LocalState.IDLE;
	private RemoteState remoteState_ = RemoteState.CLOSED;
	private InterruptibleQueue<Event> eventQueue_ = new InterruptibleQueue<Sequencer.Event>(32);
	private Event lastEvent_ = new Event(Event.Type.CLOSED, 0);

	public SequencerImpl(IOIOImpl ioio, ChannelConfig[] config) throws ConnectionLostException {
		super(ioio);

		if (config.length > 32) {
			throw new IllegalArgumentException("Up to 32 channels are supported.");
		}

		// Traverse config, create channels and an array of resources to
		// allocate.
		channels_ = new Channel[config.length];
		for (int i = 0; i < config.length; ++i) {
			channels_[i] = createChannel(config[i]);
		}

		// Allocate resources.
		ioio_.resourceManager_.alloc(pins_, ocs_, sequencer_);

		// Register for events, before they start coming.
		ioio_.incomingState_.addSequencerEventListener(this);
		ioio_.incomingState_.addDisconnectListener(this);

		// Protocol (pins, sequencer).
		for (Channel ch : channels_) {
			ch.openPins();
		}
		openSequencer();
	}

	@Override
	public synchronized void push(ChannelCue[] cues, int duration) throws ConnectionLostException,
			InterruptedException {
		checkState();
		if (duration < 2 || duration > (1 << 16)) {
			throw new IllegalArgumentException("Duration must be in the range [2..65536]");
		}
		while (availableSlots_ == 0) {
			wait();
		}
		try {
			final int size = serializeCues(cues, serializedBuf_);
			ioio_.protocol_.sequencerPush(duration - 1, serializedBuf_, size);
			--availableSlots_;
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	public synchronized void manualStart(ChannelCue[] cues) throws ConnectionLostException {
		checkState();
		if (localState_ == LocalState.RUNNING) {
			throw new IllegalStateException("manualStart() may not be called when running.");
		}
		try {
			final int size = serializeCues(cues, serializedBuf_);
			ioio_.protocol_.sequencerManualStart(serializedBuf_, size);
			localState_ = LocalState.MANUAL;
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	public synchronized void manualStop() throws ConnectionLostException {
		checkState();
		if (localState_ == LocalState.RUNNING) {
			throw new IllegalStateException("manualStop() may not be called when running.");
		}
		if (localState_ == LocalState.MANUAL) {
			try {
				ioio_.protocol_.sequencerManualStop();
				localState_ = LocalState.IDLE;
			} catch (IOException e) {
				throw new ConnectionLostException(e);
			}
		}
	}

	@Override
	public synchronized void start() throws ConnectionLostException {
		checkState();
		if (localState_ != LocalState.IDLE) {
			throw new IllegalStateException("start() may only be called when idle.");
		}
		try {
			ioio_.protocol_.sequencerStart();
			localState_ = LocalState.RUNNING;
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	public synchronized void stop() throws ConnectionLostException {
		checkState();
		try {
			ioio_.protocol_.sequencerStop();
			localState_ = LocalState.IDLE;
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	public synchronized void pause() throws ConnectionLostException {
		checkState();
		if (localState_ != LocalState.RUNNING) {
			throw new IllegalStateException("pause() may only be called when running.");
		}
		try {
			ioio_.protocol_.sequencerPause();
			localState_ = LocalState.IDLE;
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	public synchronized int available() {
		return availableSlots_;
	}

	@Override
	public synchronized void close() {
		checkClose();
		try {
			// Send the protocol message to close the sequencer.
			closeSequencer();
			// Send the protocol messages to close the pins.
			for (Resource pin : pins_) {
				ioio_.protocol_.setPinDigitalIn(pin.id, DigitalInput.Spec.Mode.FLOATING);
			}
			// Wait until the IOIO acknowledges the close. Unlike most other interfaces, this is
			// important, since the close command send to the IOIO here is asynchronous, so we do
			// not want to free the resources or exit the call before the close is complete.
			waitRemoteState(RemoteState.CLOSED);
			// Free resources.
			ioio_.resourceManager_.free(pins_, ocs_, sequencer_);
		} catch (Exception e) {
		} finally {
			super.close();
		}
	}

	@Override
	public synchronized void opened(int arg) {
		availableSlots_ = arg;
		remoteState_ = RemoteState.IDLE_OR_MANUAL;
		pushEvent(Event.Type.STOPPED);
		notifyAll();
	}

	@Override
	public synchronized void stalled() {
		remoteState_ = RemoteState.STALLED;
		pushEvent(Event.Type.STALLED);
		notifyAll();
	}

	@Override
	public synchronized void nextCue() {
		++availableSlots_;
		++numCuesStarted_;
		pushEvent(Event.Type.CUE_STARTED);
		notifyAll();
	}

	@Override
	public synchronized void paused() {
		remoteState_ = RemoteState.IDLE_OR_MANUAL;
		pushEvent(Event.Type.PAUSED);
		notifyAll();
	}

	@Override
	public synchronized void stopped(int arg) {
		availableSlots_ += arg;
		numCuesStarted_ = 0;
		remoteState_ = RemoteState.IDLE_OR_MANUAL;
		pushEvent(Event.Type.STOPPED);
		notifyAll();
	}

	@Override
	public synchronized void closed() {
		remoteState_ = RemoteState.CLOSED;
		pushEvent(Event.Type.CLOSED);
		notifyAll();
	}

	private void closeSequencer() throws ConnectionLostException {
		try {
			ioio_.protocol_.sequencerClose();
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	private void openSequencer() throws ConnectionLostException {
		try {
			int offset = 0;
			for (Channel ch : channels_) {
				offset = ch.serializeConfig(serializedBuf_, offset);
			}
			ioio_.protocol_.sequencerOpen(serializedBuf_, offset);
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	private synchronized void waitRemoteState(RemoteState target) throws InterruptedException,
			ConnectionLostException {
		while (remoteState_ != target) {
			safeWait();
		}
	}

	// /////////////////////////////////////////////////////////////////////////
	// Configuration wrappers (serializers).

	private static int convertClock(Clock clk) {
		switch (clk) {
		case CLK_16M:
			return 0;
		case CLK_2M:
			return 1;
		case CLK_250K:
			return 2;
		case CLK_62K5:
			return 3;
		default:
			return -1;
		}
	}

	private int serializeCues(ChannelCue[] cues, byte[] buf) {
		if (cues.length != channels_.length) {
			throw new IllegalArgumentException("Wrong number of channels.");
		}
		int offset = 0;
		int i = 0;
		for (Channel ch : channels_) {
			offset = ch.serializeCue(cues[i++], buf, offset);
		}
		return offset;
	}

	private Channel createChannel(ChannelConfig config) throws ConnectionLostException {
		for (ClassMapEntry mapping : CLASS_MAP) {
			if (mapping.configClass.isInstance(config)) {
				Constructor<?> ctor;
				try {
					ctor = mapping.channelClass.getConstructor(SequencerImpl.class,
							mapping.configClass);
					return (Channel) ctor.newInstance(this, config);
				} catch (InvocationTargetException e) {
					try {
						throw e.getCause();
					} catch (RuntimeException cause) {
						throw cause;
					} catch (ConnectionLostException cause) {
						throw cause;
					} catch (Throwable cause) {
						Log.e(TAG, "Unexpected exception caught.", e);
					}
				} catch (Exception e) {
					Log.e(TAG, "Unexpected exception caught.", e);
				}
			}
		}
		throw new IllegalArgumentException("Unsupported config type: "
				+ config.getClass().getName());
	}

	private static interface Channel {
		int serializeConfig(byte[] buf, int offset);

		int serializeCue(ChannelCue cue, byte[] buf, int offset);

		void openPins() throws ConnectionLostException;
	}

	private abstract class ChannelOutCompare implements Channel {
		protected final Resource oc_;
		private final DigitalOutput.Spec[] specs_;

		protected ChannelOutCompare(DigitalOutput.Spec[] specs) {
			specs_ = specs;
			oc_ = new Resource(ResourceType.OUTCOMPARE);
			ocs_.add(oc_);
			for (DigitalOutput.Spec spec : specs) {
				ioio_.hardware_.checkSupportsPeripheralOutput(spec.pin);
				pins_.add(new Resource(ResourceType.PIN, spec.pin));
			}
		}

		@Override
		public final void openPins() throws ConnectionLostException {
			try {
				for (DigitalOutput.Spec spec : specs_) {
					ioio_.protocol_.setPinDigitalOut(spec.pin, false, spec.mode);
					ioio_.protocol_.setPinPwm(spec.pin, oc_.id, true);
				}
			} catch (IOException e) {
				throw new ConnectionLostException(e);
			}
		}
	}

	private class ChannelPwmPosition extends ChannelOutCompare {
		private final ChannelConfigPwmPosition cfg_;

		public ChannelPwmPosition(ChannelConfigPwmPosition cfg) {
			super(cfg.pinSpec);
			cfg_ = cfg;
		}

		@Override
		public int serializeConfig(byte[] buf, int offset) {
			final int encPulseWidth = cfg_.initialPulseWidth == 0 ? 0 : cfg_.initialPulseWidth - 1;
			final int encPeriod = cfg_.period - 1;

			buf[offset++] = 0;
			buf[offset++] = (byte) ((oc_.id & 0x0F) | (convertClock(cfg_.clk) << 4));
			buf[offset++] = (byte) ((encPeriod >> 0) & 0xFF);
			buf[offset++] = (byte) ((encPeriod >> 8) & 0xFF);
			buf[offset++] = (byte) ((encPulseWidth >> 0) & 0xFF);
			buf[offset++] = (byte) ((encPulseWidth >> 8) & 0xFF);
			return offset;
		}

		@Override
		public int serializeCue(ChannelCue cue, byte[] buf, int offset) {
			if (!(cue instanceof ChannelCuePwmPosition)) {
				throw new IllegalArgumentException("Wrong cue type.");
			}
			ChannelCuePwmPosition pwm = (ChannelCuePwmPosition) cue;
			if (pwm.pulseWidth != 0 && (pwm.pulseWidth < 2 || pwm.pulseWidth > (1 << 16))) {
				throw new IllegalArgumentException("Pulse width must be 0 or between [2..65536]");
			}
			final int encPw = pwm.pulseWidth == 0 ? 0 : pwm.pulseWidth - 1;
			buf[offset++] = (byte) ((encPw >> 0) & 0xFF);
			buf[offset++] = (byte) ((encPw >> 8) & 0xFF);
			return offset;
		}
	}

	private class ChannelPwmSpeed extends ChannelOutCompare {
		private final ChannelConfigPwmSpeed cfg_;

		public ChannelPwmSpeed(ChannelConfigPwmSpeed cfg) {
			super(cfg.pinSpec);
			cfg_ = cfg;
		}

		@Override
		public int serializeConfig(byte[] buf, int offset) {
			final int encPulseWidth = cfg_.initialPulseWidth == 0 ? 0 : cfg_.initialPulseWidth - 1;
			final int encPeriod = cfg_.period - 1;

			buf[offset++] = 1;
			buf[offset++] = (byte) ((oc_.id & 0x0F) | (convertClock(cfg_.clk) << 4));
			buf[offset++] = (byte) ((encPeriod >> 0) & 0xFF);
			buf[offset++] = (byte) ((encPeriod >> 8) & 0xFF);
			buf[offset++] = (byte) ((encPulseWidth >> 0) & 0xFF);
			buf[offset++] = (byte) ((encPulseWidth >> 8) & 0xFF);
			return offset;
		}

		@Override
		public int serializeCue(ChannelCue cue, byte[] buf, int offset) {
			if (!(cue instanceof ChannelCuePwmSpeed)) {
				throw new IllegalArgumentException("Wrong cue type.");
			}
			ChannelCuePwmSpeed pwm = (ChannelCuePwmSpeed) cue;
			if (pwm.pulseWidth != 0 && (pwm.pulseWidth < 2 || pwm.pulseWidth > (1 << 16))) {
				throw new IllegalArgumentException("Pulse width must be 0 or between [2..65536]");
			}
			final int encPw = pwm.pulseWidth == 0 ? 0 : pwm.pulseWidth - 1;
			buf[offset++] = (byte) ((encPw >> 0) & 0xFF);
			buf[offset++] = (byte) ((encPw >> 8) & 0xFF);
			return offset;
		}
	}

	private class ChannelFmSpeed extends ChannelOutCompare {
		private final ChannelConfigFmSpeed cfg_;

		public ChannelFmSpeed(ChannelConfigFmSpeed cfg) {
			super(cfg.pinSpec);
			cfg_ = cfg;
		}

		@Override
		public int serializeConfig(byte[] buf, int offset) {
			final int encPulseWidth = cfg_.pulseWidth - 1;
			buf[offset++] = 2;
			buf[offset++] = (byte) ((oc_.id & 0x0F) | (convertClock(cfg_.clk) << 4));
			buf[offset++] = (byte) ((encPulseWidth >> 0) & 0xFF);
			buf[offset++] = (byte) ((encPulseWidth >> 8) & 0xFF);
			return offset;
		}

		@Override
		public int serializeCue(ChannelCue cue, byte[] buf, int offset) {
			if (!(cue instanceof ChannelCueFmSpeed)) {
				throw new IllegalArgumentException("Wrong cue type.");
			}
			ChannelCueFmSpeed fm = (ChannelCueFmSpeed) cue;
			if (fm.period < 0 || fm.period > (1 << 16)) {
				throw new IllegalArgumentException("Period must be between [0..65536]");
			}
			final int encPeriod = fm.period < 2 ? fm.period : fm.period - 1;
			buf[offset++] = (byte) ((encPeriod >> 0) & 0xFF);
			buf[offset++] = (byte) ((encPeriod >> 8) & 0xFF);
			return offset;
		}
	}

	private class ChannelSteps extends ChannelOutCompare {
		public ChannelSteps(ChannelConfigSteps cfg) {
			super(cfg.pinSpec);
		}

		@Override
		public int serializeConfig(byte[] buf, int offset) {
			buf[offset++] = 3;
			buf[offset++] = (byte) ((oc_.id & 0x0F));
			return offset;
		}

		@Override
		public int serializeCue(ChannelCue cue, byte[] buf, int offset) {
			if (!(cue instanceof ChannelCueSteps)) {
				throw new IllegalArgumentException("Wrong cue type.");
			}
			ChannelCueSteps steps = (ChannelCueSteps) cue;

			if (steps.pulseWidth < 0 || steps.pulseWidth > steps.period / 2) {
				throw new IllegalArgumentException(
						"Pulse width must be 0 or between [0..floor(period/2)]");
			}
			if (steps.period < 3 || steps.period > (1 << 16)) {
				throw new IllegalArgumentException("Period must be between [3..65536]");
			}

			final int pwEnc = steps.pulseWidth; // No -1 here on purpose!
			final int periodEnd = steps.period - 1;

			buf[offset++] = (byte) convertClock(steps.clk);
			buf[offset++] = (byte) ((pwEnc >> 0) & 0xFF);
			buf[offset++] = (byte) ((pwEnc >> 8) & 0xFF);
			buf[offset++] = (byte) ((periodEnd >> 0) & 0xFF);
			buf[offset++] = (byte) ((periodEnd >> 8) & 0xFF);
			return offset;
		}
	}

	private class ChannelBinary implements Channel {
		private final ChannelConfigBinary cfg_;
		private final Resource pin_;

		@SuppressWarnings("unused")
		public ChannelBinary(ChannelConfigBinary cfg) {
			cfg_ = cfg;
			pin_ = new Resource(ResourceType.PIN, cfg.pinSpec.pin);
			pins_.add(pin_);
		}

		@Override
		public int serializeConfig(byte[] buf, int offset) {
			buf[offset++] = 0x04;
			buf[offset++] = (byte) ((cfg_.pinSpec.pin & 0x3F) | (cfg_.initialValue ? (1 << 6) : 0) | (cfg_.initWhenIdle ? (1 << 7)
					: 0));
			return offset;
		}

		@Override
		public int serializeCue(ChannelCue cue, byte[] buf, int offset) {
			if (!(cue instanceof ChannelCueBinary)) {
				throw new IllegalArgumentException("Wrong cue type.");
			}
			ChannelCueBinary bin = (ChannelCueBinary) cue;
			buf[offset++] = (byte) (bin.value ? 0x01 : 0x00);
			return offset;
		}

		@Override
		public void openPins() throws ConnectionLostException {
			try {
				ioio_.protocol_.setPinDigitalOut(cfg_.pinSpec.pin, cfg_.initialValue,
						cfg_.pinSpec.mode);
			} catch (IOException e) {
				throw new ConnectionLostException(e);
			}
		}
	}

	@Override
	public Event getLastEvent() throws ConnectionLostException {
		checkState();
		return lastEvent_;
	}

	@Override
	public Event waitEvent() throws ConnectionLostException, InterruptedException {
		checkState();
		while (true) {
			try {
				return eventQueue_.pull();
			} catch (Nudged e) {
				checkState();
			}
		}
	}

	@Override
	public void waitEventType(Event.Type type) throws ConnectionLostException, InterruptedException {
		while (waitEvent().type != type)
			;
	}

	@Override
	public synchronized void setEventQueueSize(int size) throws ConnectionLostException {
		checkState();
		if (size <= 0) {
			throw new IllegalArgumentException("Event queue size must be positive.");
		}
		InterruptibleQueue<Event> prevQueue = eventQueue_;
		eventQueue_ = new InterruptibleQueue<Event>(size);
		prevQueue.nudge();
	}

	private void pushEvent(Event.Type t) {
		lastEvent_ = new Event(t, numCuesStarted_);
		eventQueue_.pushDiscardingOld(lastEvent_);
	}
}
