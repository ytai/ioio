package ioio.tests.torture;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.IOIO;
import ioio.lib.api.PulseInput;
import ioio.lib.api.PulseInput.ClockRate;
import ioio.lib.api.PulseInput.PulseMode;
import ioio.lib.api.PwmOutput;
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
		alloc.allocPeripheral(PeripheralType.INCAP_SINGLE);
		alloc.allocPeripheral(PeripheralType.INCAP_DOUBLE);
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
			alloc_.freePeripheral(PeripheralType.INCAP_SINGLE);
			alloc_.freePeripheral(PeripheralType.INCAP_DOUBLE);
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
		if (!runTest(inPin, outPin, 2000, 20, ClockRate.RATE_16MHz,
				PulseMode.FREQ_SCALE_4, false))
			return false;
		if (!runTest(inPin, outPin, 2000, 50, ClockRate.RATE_16MHz,
				PulseMode.FREQ, false))
			return false;
		if (!runTest(inPin, outPin, 2000, 100, ClockRate.RATE_2MHz,
				PulseMode.FREQ_SCALE_16, false))
			return false;
		if (!runTest(inPin, outPin, 2000, 100, ClockRate.RATE_16MHz,
				PulseMode.FREQ_SCALE_16, true))
			return false;
		return true;
	}

	private boolean runTest(int inPin, int outPin, int freq,
			int pulseWidthUsec, ClockRate rate, PulseMode freqScaling,
			boolean doublePrecision) throws ConnectionLostException,
			InterruptedException {
		PulseInput pulseDurIn = null;
		PulseInput pulseFreqIn = null;
		PwmOutput out = null;
		try {
			out = ioio_.openPwmOutput(outPin, freq);
			out.setPulseWidth(pulseWidthUsec);
			// measure positive pulse
			pulseDurIn = ioio_.openPulseInput(new DigitalInput.Spec(inPin),
					rate, PulseMode.POSITIVE, doublePrecision);
			float duration = pulseDurIn.getDuration();
			float expectedDuration = pulseWidthUsec / 1000000.f;
			if (Math.abs((duration - expectedDuration) / duration) > 0.02) {
				Log.w("IOIOTortureTest", "Positive pulse duration is: "
						+ duration + "[s] while expected " + expectedDuration
						+ "[s]");
				return false;
			}
			pulseDurIn.close();
			// measure negative pulse
			pulseDurIn = ioio_.openPulseInput(new DigitalInput.Spec(inPin),
					rate, PulseMode.NEGATIVE, doublePrecision);
			duration = pulseDurIn.getDuration();
			expectedDuration = (1.f / freq) - (pulseWidthUsec / 1000000.f);
			if (Math.abs((duration - expectedDuration) / duration) > 0.02) {
				Log.w("IOIOTortureTest", "Negative pulse duration is: "
						+ duration + "[s] while expected " + expectedDuration
						+ "[s]");
				return false;
			}
			pulseDurIn.close();
			pulseDurIn = null;
			// measure frequency
			pulseFreqIn = ioio_.openPulseInput(new DigitalInput.Spec(inPin),
					rate, freqScaling, doublePrecision);
			float actualFreq = pulseFreqIn.getFrequency();
			if (Math.abs((actualFreq - freq) / freq) > 0.02) {
				Log.w("IOIOTortureTest", "Frequency is: " + actualFreq
						+ " while expected " + freq);
				return false;
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
