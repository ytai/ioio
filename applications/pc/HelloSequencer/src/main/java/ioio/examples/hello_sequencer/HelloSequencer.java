package ioio.examples.hello_sequencer;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.Sequencer;
import ioio.lib.api.Sequencer.ChannelConfig;
import ioio.lib.api.Sequencer.ChannelConfigBinary;
import ioio.lib.api.Sequencer.ChannelConfigPwmSpeed;
import ioio.lib.api.Sequencer.ChannelConfigSteps;
import ioio.lib.api.Sequencer.ChannelCueBinary;
import ioio.lib.api.Sequencer.ChannelCuePwmSpeed;
import ioio.lib.api.Sequencer.ChannelCueSteps;
import ioio.lib.api.Sequencer.Clock;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.pc.IOIOConsoleApp;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class HelloSequencer extends IOIOConsoleApp {

	// Boilerplate main(). Copy-paste this code into any IOIOapplication.
	public static void main(String[] args) throws Exception {
		new HelloSequencer().go(args);
	}

	@Override
	protected void run(String[] args) throws IOException {
		BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
		boolean abort = false;
		String line;
		while (!abort && (line = reader.readLine()) != null) {
			if (line.equals("q")) {
				abort = true;
			} else {
				System.out.println("Unknown input. q=quit.");
			}
		}
	}

	@Override
	public IOIOLooper createIOIOLooper(String connectionType, Object extra) {
		return new BaseIOIOLooper() {
			private boolean dir_ = false;
			private int color_ = 0;

			private Sequencer.ChannelCueBinary led1Cue_ = new ChannelCueBinary();
			private Sequencer.ChannelCueBinary led2Cue_ = new ChannelCueBinary();
			private Sequencer.ChannelCueBinary led3Cue_ = new ChannelCueBinary();
			private Sequencer.ChannelCuePwmSpeed dcMotorACue_ = new ChannelCuePwmSpeed();
			private Sequencer.ChannelCuePwmSpeed dcMotorBCue_ = new ChannelCuePwmSpeed();
			private Sequencer.ChannelCueBinary stepperDirCue_ = new ChannelCueBinary();
			private Sequencer.ChannelCueSteps stepperStepCue_ = new ChannelCueSteps();

			private Sequencer.ChannelCue[] cue_ = new Sequencer.ChannelCue[] { led1Cue_, led2Cue_,
					led3Cue_, dcMotorACue_, dcMotorBCue_, stepperDirCue_, stepperStepCue_ };
			private Sequencer sequencer_;

			@Override
			protected void setup() throws ConnectionLostException, InterruptedException {
				final ChannelConfigBinary led1Config = new Sequencer.ChannelConfigBinary(true,
						true, new DigitalOutput.Spec(5, DigitalOutput.Spec.Mode.OPEN_DRAIN));
				final ChannelConfigBinary led2Config = new Sequencer.ChannelConfigBinary(true,
						true, new DigitalOutput.Spec(6, DigitalOutput.Spec.Mode.OPEN_DRAIN));
				final ChannelConfigBinary led3Config = new Sequencer.ChannelConfigBinary(true,
						true, new DigitalOutput.Spec(7, DigitalOutput.Spec.Mode.OPEN_DRAIN));
				final ChannelConfigPwmSpeed dcMotorAConfig = new Sequencer.ChannelConfigPwmSpeed(
						Sequencer.Clock.CLK_2M, 2000, 0, new DigitalOutput.Spec(1));
				final ChannelConfigPwmSpeed dcMotorBConfig = new Sequencer.ChannelConfigPwmSpeed(
						Sequencer.Clock.CLK_2M, 2000, 0, new DigitalOutput.Spec(2));
				final ChannelConfigBinary stepperDirConfig = new Sequencer.ChannelConfigBinary(
						false, false, new DigitalOutput.Spec(3));
				final ChannelConfigSteps stepperStepConfig = new ChannelConfigSteps(
						new DigitalOutput.Spec(4));
				final ChannelConfig[] config = new ChannelConfig[] { led1Config, led2Config,
						led3Config, dcMotorAConfig, dcMotorBConfig, stepperDirConfig,
						stepperStepConfig };

				sequencer_ = ioio_.openSequencer(config);

				// Pre-fill.
				sequencer_.waitEventType(Sequencer.Event.Type.STOPPED);
				while (sequencer_.available() > 0) {
					push();
				}

				sequencer_.start();
			}

			@Override
			public void loop() throws ConnectionLostException, InterruptedException {
				push();
			}

			private void push() throws ConnectionLostException, InterruptedException {
				stepperStepCue_.clk = Clock.CLK_2M;
				stepperStepCue_.pulseWidth = 2;
				stepperStepCue_.period = 400;
				if (dir_) {
					dcMotorACue_.pulseWidth = 1000;
					dcMotorBCue_.pulseWidth = 0;
					stepperDirCue_.value = false;
				} else {
					dcMotorACue_.pulseWidth = 0;
					dcMotorBCue_.pulseWidth = 1000;
					stepperDirCue_.value = true;
				}
				led1Cue_.value = (color_ & 1) == 0;
				led2Cue_.value = (color_ & 2) == 0;
				led3Cue_.value = (color_ & 4) == 0;

				sequencer_.push(cue_, 62500 / 2);
				dir_ = !dir_;
				color_++;
			}
		};
	}
}
