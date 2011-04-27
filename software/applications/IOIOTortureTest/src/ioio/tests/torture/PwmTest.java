package ioio.tests.torture;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.IOIO;
import ioio.lib.api.PwmOutput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.tests.torture.ResourceAllocator.PeripheralType;
import android.util.Log;

public class PwmTest implements Test<Boolean> {
	private final IOIO ioio_;
	private final ResourceAllocator alloc_;
	private final int pin1_;
	private final int pin2_;

	public PwmTest(IOIO ioio, ResourceAllocator alloc)
			throws InterruptedException {
		ioio_ = ioio;
		alloc_ = alloc;
		pin1_ = alloc.allocatePinPair(ResourceAllocator.PIN_PAIR_PERIPHERAL);
		alloc.allocPeripheral(PeripheralType.PWM);
		pin2_ = pin1_ + 1;
	}

	@Override
	public Boolean run() throws ConnectionLostException, InterruptedException {
		Log.i("IOIOTortureTest", "Starting PwmTest on pins: " + pin1_
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
		}
		Log.i("IOIOTortureTest", "Passed PwmTest on pins: " + pin1_
				+ ", " + pin2_);
		return true;
	}
	
	private boolean runTest(int inPin, int outPin)
			throws ConnectionLostException, InterruptedException {
		if (outPin == 9) {
			// pin 9 doesn't support peripheral output
			return true;
		}
		DigitalInput in;
		PwmOutput out;
		in = ioio_.openDigitalInput(inPin);
		out = ioio_.openPwmOutput(outPin, 131);
		try {
			for (int i = 0; i <= 4; ++i) {
				if (!runSingleTest(out, in, i / 4.f)) {
					Log.w("IOIOTortureTest", "Failed PwmTest input: "
							+ inPin + ", output: " + outPin);
					return false;
				}
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
	
	private boolean runSingleTest(PwmOutput out, DigitalInput in, float dc)
			throws InterruptedException, ConnectionLostException {
		out.setDutyCycle(dc);
		Thread.sleep(100);
		int highCount = 0;
		for (int i = 0; i < 500; i++) {
			highCount += in.read() ? 1 : 0;
			Thread.sleep(1);
		}
		return Math.abs((highCount / 500.f) - dc) < 0.2;
	}
}
