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
	public Boolean run() throws ConnectionLostException {
		Log.i("IOIOTortureTest", "Starting DigitalIOTest on pins: " + pin1_
				+ ", " + pin2_);
		DigitalInput in = ioio_.openDigitalInput(pin1_);
		DigitalOutput out = ioio_.openDigitalOutput(pin2_);
		try {
			boolean value = false;
			for (int i = 0; i < 10; ++i) {
				out.write(value);
				Thread.sleep(100);
				if (in.read() != value) {
					Log.w("IOIOTortureTest", "Failed DigitalIOTest input: "
							+ pin1_ + ", output: " + pin2_);
					return false;
				}
				value = !value;
			}
			in.close();
			out.close();
			in = ioio_.openDigitalInput(pin2_);
			out = ioio_.openDigitalOutput(pin1_);
			for (int i = 0; i < 10; ++i) {
				out.write(value);
				Thread.sleep(50);
				if (in.read() != value) {
					Log.w("IOIOTortureTest", "Failed DigitalIOTest input: "
							+ pin2_ + ", output: " + pin1_);
					return false;
				}
				value = !value;
			}
		} catch (InterruptedException e) {
		} finally {
			in.close();
			out.close();
			alloc_.freePinPair(pin1_);
		}
		Log.i("IOIOTortureTest", "Passed DigitalIOTest on pins: " + pin1_
				+ ", " + pin2_);
		return true;
	}
}
