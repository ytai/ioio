package ioio.tests.torture;

import ioio.lib.api.AnalogInput;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import android.util.Log;

public class AnalogInputTest implements Test<Boolean> {
	private final IOIO ioio_;
	private final ResourceAllocator alloc_;
	private final int pin1_;
	private final int pin2_;

	public AnalogInputTest(IOIO ioio, ResourceAllocator alloc)
			throws InterruptedException {
		ioio_ = ioio;
		alloc_ = alloc;
		pin1_ = alloc.allocatePinPair(ResourceAllocator.PIN_PAIR_ANALOG);
		pin2_ = pin1_ + 1;
	}

	@Override
	public Boolean run() throws ConnectionLostException, InterruptedException {
		Log.i("IOIOTortureTest", "Starting AnalogInputTest on pins: " + pin1_
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
		}
		Log.i("IOIOTortureTest", "Passed AnalogInputTest on pins: " + pin1_
				+ ", " + pin2_);
		return true;
	}

	private boolean runTest(int inPin, int outPin)
			throws ConnectionLostException, InterruptedException {
		AnalogInput in = null;
		DigitalOutput out = null;
		try {
			in = ioio_.openAnalogInput(inPin);
			out = ioio_.openDigitalOutput(outPin);
			boolean value = false;
			for (int i = 0; i < 10; ++i) {
				out.write(value);
				boolean correct = false;
				// We don't care about latency here, so we're sampling the input
				// at 10ms intervals and will give up only if it doesn't reach
				// the expect value after 50 attempts.
				for (int attempt = 0; !correct && attempt < 50; ++attempt) {
					if (value && in.read() > 0.9 || !value && in.read() < 0.1) {
						correct = true;
					} else {
						Thread.sleep(10);
					}
				}
				if (!correct) {
					Log.w("IOIOTortureTest", "Failed AnalogInputTest input: "
							+ inPin + ", output: " + outPin);
					return false;
				}
				value = !value;
			}
		} finally {
			if (in != null) {
				in.close();
			}
			if (out != null) {
				out.close();
			}
		}
		return true;
	}
}
