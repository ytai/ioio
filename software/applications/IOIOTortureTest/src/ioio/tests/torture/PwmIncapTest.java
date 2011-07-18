package ioio.tests.torture;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.IOIO;
import ioio.lib.api.PulseDurationInput;
import ioio.lib.api.PulseFrequencyInput;
import ioio.lib.api.PulseFrequencyInput.Scaling;
import ioio.lib.api.PwmOutput;
import ioio.lib.api.PulseDurationInput.ClockRate;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.tests.torture.ResourceAllocator.PeripheralType;
import android.util.Log;

public class PwmIncapTest implements Test<Boolean> {
	private final IOIO ioio_;
	private final ResourceAllocator alloc_;
	private final int pin1_;
	private final int pin2_;

	public PwmIncapTest(IOIO ioio, ResourceAllocator alloc)
			throws InterruptedException {
		ioio_ = ioio;
		alloc_ = alloc;
		pin1_ = alloc.allocatePinPair(ResourceAllocator.PIN_PAIR_PERIPHERAL);
		alloc.allocPeripheral(PeripheralType.PWM);
		alloc.allocPeripheral(PeripheralType.INCAP);
		pin2_ = pin1_ + 1;
	}

	@Override
	public Boolean run() throws ConnectionLostException, InterruptedException {
		Log.i("IOIOTortureTest", "Starting PwmIncapTest on pins: " + pin1_
				+ ", " + pin2_);
		try {
			if (!runTest(pin1_, pin2_)) {
				return false;
			}
			if (!runTest(pin2_, pin1_)) {
				return false;
			}
		} finally {
			alloc_.freePinPair(pin1_);
			alloc_.freePeripheral(PeripheralType.PWM);
			alloc_.freePeripheral(PeripheralType.INCAP);
		}
		Log.i("IOIOTortureTest", "Passed PwmIncapTest on pins: " + pin1_ + ", "
				+ pin2_);
		return true;
	}

	private boolean runTest(int inPin, int outPin)
			throws ConnectionLostException, InterruptedException {
		if (outPin == 9) {
			// pin 9 doesn't support peripheral output
			return true;
		}
		PulseDurationInput pulseDurIn = null;
		PulseFrequencyInput pulseFreqIn = null;
		PwmOutput out = null;
		try {
			out = ioio_.openPwmOutput(outPin, 2000);
			out.setPulseWidth(10);
			Thread.sleep(100);
			pulseDurIn = ioio_.openPulseDurationInput(inPin,
					ClockRate.RATE_16MHz);
			float duration = pulseDurIn.getDuration();
			if (duration < 9.9 * 1e-6 || duration > 10.1 * 1e-6) {
				Log.w("IOIOTortureTest", "Pulse duration is: " + duration
						+ " while expected 10.0");
			}
			pulseDurIn.close();
			pulseDurIn = null;
			pulseFreqIn = ioio_.openPulseFrequencyInput(new DigitalInput.Spec(
					inPin),
					ioio.lib.api.PulseFrequencyInput.ClockRate.RATE_16MHz,
					Scaling.SCALE_4);
			float freq = pulseFreqIn.getFrequency();
			if (freq < 1990 || freq > 2010) {
				Log.w("IOIOTortureTest", "Frequency is: " + freq
						+ " while expected 2000");
			}
		} finally {
			if (pulseDurIn != null) {
				pulseDurIn.close();
			}
			if (pulseFreqIn != null) {
				pulseFreqIn.close();
			}
			if (out != null) {
				out.close();
			}
		}
		return true;
	}
}
