package ioio.tests.torture;

import ioio.lib.api.IOIO;
import ioio.lib.api.SpiMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.tests.torture.ResourceAllocator.PeripheralType;

import java.util.Arrays;
import java.util.Random;

import android.util.Log;

public class SpiTest implements Test<Boolean> {
	private final IOIO ioio_;
	private final ResourceAllocator alloc_;
	private final int pin1_;
	private final int pin2_;
	private final int pin3_;
	private final int pin4_;

	public SpiTest(IOIO ioio, ResourceAllocator alloc)
			throws InterruptedException {
		ioio_ = ioio;
		alloc_ = alloc;
		pin1_ = alloc.allocatePinPair(ResourceAllocator.PIN_PAIR_PERIPHERAL);
		pin3_ = alloc.allocatePinPair(ResourceAllocator.PIN_PAIR_PERIPHERAL);
		alloc.allocPeripheral(PeripheralType.SPI);
		pin2_ = pin1_ + 1;
		pin4_ = pin3_ + 1;
	}

	@Override
	public Boolean run() throws ConnectionLostException, InterruptedException {
		Log.i("IOIOTortureTest", "Starting SpiTest on pins: " + pin1_ + ", "
				+ pin2_ + ", " + pin3_ + ", " + pin4_);
		try {
			// pin 4 is even => never pin 9, so we'll use it for clk
			if (!runTest(pin1_, pin2_, pin4_, pin3_)) {
				return false;
			}
			if (!runTest(pin2_, pin1_, pin4_, pin3_)) {
				return false;
			}
		} finally {
			alloc_.freePinPair(pin1_);
			alloc_.freePinPair(pin3_);
			alloc_.freePeripheral(PeripheralType.SPI);
		}
		Log.i("IOIOTortureTest", "Passed SpiTest on pins: " + pin1_ + ", "
				+ pin2_ + ", " + pin3_ + ", " + pin4_);
		return true;
	}

	private boolean runTest(int misoPin, int mosiPin, int clkPin, int ssPin)
			throws ConnectionLostException, InterruptedException {
		if (mosiPin == 9) {
			// pin 9 doesn't support peripheral output
			return true;
		}
		final int SEED = 17;

		SpiMaster spi = ioio_.openSpiMaster(misoPin, mosiPin, clkPin, ssPin,
				SpiMaster.Rate.RATE_1M);

		try {
			Random rand = new Random(SEED);

			byte[] request = new byte[20];
			byte[] response = new byte[15];
			for (int i = 0; i < 100; ++i) {
				rand.nextBytes(request);
				spi.writeRead(request, request.length, 25, response,
						response.length);
				for (int j = 0; j < response.length; ++j) {
					if (j < 10) {
						if (response[j] != request[j + 10]) {
							Log.w("IOIOTortureTest",
									"Failed SpiTest (contents) on pins: "
											+ pin1_ + ", " + pin2_ + ", "
											+ pin3_ + ", " + pin4_);
							Log.w("IOIOTortureTest",
									"Requested: " + Arrays.toString(request)
											+ " got: "
											+ Arrays.toString(response));
							return false;
						}
					} else {
						if (response[j] != (byte) 0xFF) {
							Log.w("IOIOTortureTest",
									"Failed SpiTest (suffix) on pins: " + pin1_
											+ ", " + pin2_ + ", " + pin3_
											+ ", " + pin4_);
							Log.w("IOIOTortureTest",
									"Requested: " + Arrays.toString(request)
											+ " got: "
											+ Arrays.toString(response));
							return false;
						}
					}
				}
				// send a write-only and a read-only request every 10 requests
				if (i % 10 == 0) {
					spi.writeRead(request, request.length, request.length,
							null, 0);
					spi.writeRead(null, 0, response.length, response,
							response.length);
					for (int j = 0; j < response.length; ++j) {
						if (response[j] != (byte) 0xFF) {
							Log.w("IOIOTortureTest",
									"Failed SpiTest (read-only) on pins: "
											+ pin1_ + ", " + pin2_ + ", "
											+ pin3_ + ", " + pin4_);
							Log.w("IOIOTortureTest",
									" got: " + Arrays.toString(response));
							return false;
						}
					}
				}
			}
			return true;
		} finally {
			spi.close();
		}
	}
}
