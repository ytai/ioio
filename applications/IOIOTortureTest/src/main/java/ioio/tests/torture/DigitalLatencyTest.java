package ioio.tests.torture;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;

import java.util.ArrayList;
import java.util.List;

import android.util.Log;

class DigitalLatencyTest implements Test<List<Float>> {
	private final IOIO ioio_;
	private final ResourceAllocator alloc_;
	private final int pin1_;
	private final int pin2_;

	public DigitalLatencyTest(IOIO ioio, ResourceAllocator alloc)
			throws InterruptedException {
		ioio_ = ioio;
		alloc_= alloc;
		pin1_ = alloc.allocatePinPair(ResourceAllocator.PIN_PAIR_DIGITAL);
		pin2_ = pin1_ + 1;
	}

	@Override
	public List<Float> run() throws ConnectionLostException,
			InterruptedException {
		Log.i("IOIOTortureTest", "Starting DigitalLatencyTest on pins: " + pin1_
				+ ", " + pin2_);
		List<Float> results = new ArrayList<Float>(20);
		DigitalInput in = ioio_.openDigitalInput(pin1_);
		DigitalOutput out = ioio_.openDigitalOutput(pin2_);
		try {
			boolean value = false;
			for (int i = 0; i < 10; ++i) {
				long start = System.nanoTime();
				out.write(value);
				in.waitForValue(value);
				results.add((System.nanoTime() - start) / 1000000.f);
				value = !value;
			}
			in.close();
			out.close();
			in = ioio_.openDigitalInput(pin2_);
			out = ioio_.openDigitalOutput(pin1_);
			for (int i = 0; i < 10; ++i) {
				out.write(value);
				long start = System.nanoTime();
				in.waitForValue(value);
				results.add((System.nanoTime() - start) / 1000000.f);
				value = !value;
			}
		} finally {
			in.close();
			out.close();
			alloc_.freePinPair(pin1_);
		}
		Log.i("IOIOTortureTest", "Finished DigitalLatencyTest on pins: " + pin1_
				+ ", " + pin2_);
		return results;
	}
}
