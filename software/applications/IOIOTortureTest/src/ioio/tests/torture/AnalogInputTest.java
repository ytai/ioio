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
	public Boolean run() throws ConnectionLostException {
		Log.i("IOIOTortureTest", "Starting AnalogInputTest on pins: " + pin1_
				+ ", " + pin2_);
		AnalogInput in = ioio_.openAnalogInput(pin1_);
		DigitalOutput out = ioio_.openDigitalOutput(pin2_);
		try {
			boolean value = false;
			for (int i = 0; i < 10; ++i) {
				out.write(value);
				Thread.sleep(100);
				if (value && in.read() < 0.7 || !value && in.read() > 0.3) {
					Log.w("IOIOTortureTest", "Failed AnalogInputTest input: "
							+ pin1_ + ", output: " + pin2_);
					return false;
				}
				value = !value;
			}
			in.close();
			out.close();
			in = ioio_.openAnalogInput(pin2_);
			out = ioio_.openDigitalOutput(pin1_);
			for (int i = 0; i < 10; ++i) {
				out.write(value);
				Thread.sleep(50);
				if (value && in.read() < 0.7 || !value && in.read() > 0.3) {
					Log.w("IOIOTortureTest", "Failed AnalogInputTest input: "
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
		Log.i("IOIOTortureTest", "Passed AnalogInputTest on pins: " + pin1_
				+ ", " + pin2_);
		return true;
	}
}
