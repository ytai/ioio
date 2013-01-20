package ioio.tests.torture;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import android.util.Log;

public class DigitalIOTest implements Test<Boolean> {
	private final IOIO ioio_;
	private final ResourceAllocator alloc_;
	private final int pin1_;
	private final int pin2_;

	public DigitalIOTest(IOIO ioio, ResourceAllocator alloc)
			throws InterruptedException {
		ioio_ = ioio;
		alloc_ = alloc;
		pin1_ = alloc.allocatePinPair(ResourceAllocator.PIN_PAIR_DIGITAL);
		pin2_ = pin1_ + 1;
	}

	@Override
	public Boolean run() throws ConnectionLostException, InterruptedException {
		Log.i("IOIOTortureTest", "Starting DigitalIOTest on pins: " + pin1_
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
		Log.i("IOIOTortureTest", "Passed DigitalIOTest on pins: " + pin1_
				+ ", " + pin2_);
		return true;
	}
	
	private boolean runTest(int inPin, int outPin) throws InterruptedException, ConnectionLostException {
		DigitalInput in = ioio_.openDigitalInput(inPin);
		DigitalOutput out = ioio_.openDigitalOutput(outPin);
		try {
			boolean value = false;
			for (int i = 0; i < 10; ++i) {
				out.write(value);
				boolean correct = false;
				// We don't care about latency here, so we're sampling the input
				// at 10ms intervals and will give up only if it doesn't reach
				// the expect value after 50 attempts.
				for (int attempt = 0; !correct && attempt < 50; ++attempt) {
					if (value == in.read()) {
						correct = true;
					} else {
						Thread.sleep(10);
					}
				}
				if (!correct) {
					Log.w("IOIOTortureTest", "Failed DigitalIOTest input: "
							+ inPin + ", output: " + outPin);
					return false;
				}
				value = !value;
			}
			return true;
		} finally {
			in.close();
			out.close();
		}
	}
}
