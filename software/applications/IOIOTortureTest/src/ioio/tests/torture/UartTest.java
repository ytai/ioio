package ioio.tests.torture;

import ioio.lib.api.IOIO;
import ioio.lib.api.Uart;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.tests.torture.ResourceAllocator.PeripheralType;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Random;

import android.util.Log;

public class UartTest implements Test<Boolean> {
	private final IOIO ioio_;
	private final ResourceAllocator alloc_;
	private final int pin1_;
	private final int pin2_;
	private int bytesVerified_;

	public UartTest(IOIO ioio, ResourceAllocator alloc)
			throws InterruptedException {
		ioio_ = ioio;
		alloc_ = alloc;
		pin1_ = alloc.allocatePinPair(ResourceAllocator.PIN_PAIR_PERIPHERAL);
		alloc.allocPeripheral(PeripheralType.UART);
		pin2_ = pin1_ + 1;
	}

	@Override
	public Boolean run() throws ConnectionLostException, InterruptedException {
		Log.i("IOIOTortureTest", "Starting UartTest on pins: " + pin1_ + ", "
				+ pin2_);
		try {
			if (!runTest(pin1_, pin2_)) {
				return false;
			}
			if (!runTest(pin2_, pin1_)) {
				return false;
			}
		} finally {
			alloc_.freePinPair(pin1_);
			alloc_.freePeripheral(PeripheralType.UART);
		}
		Log.i("IOIOTortureTest", "Passed UartTest on pins: " + pin1_ + ", "
				+ pin2_);
		return true;
	}

	private boolean runTest(int inPin, int outPin)
			throws ConnectionLostException, InterruptedException {
		if (outPin == 9) {
			// pin 9 doesn't support peripheral output
			return true;
		}
		final int BYTE_COUNT = 2000;
		final int SEED = 17;

		Uart uart = ioio_.openUart(inPin, outPin, 115200, Uart.Parity.NONE,
				Uart.StopBits.ONE);
		InputStream in = uart.getInputStream();
		OutputStream out = uart.getOutputStream();
		Random rand = new Random(SEED);
		bytesVerified_ = 0;
		Thread reader = new ReaderThread(in, SEED, BYTE_COUNT);
		reader.start();
		try {
			for (int i = 0; i < BYTE_COUNT; ++i) {
				byte value = (byte) rand.nextInt();
				out.write(value);
			}
			reader.join();
		} catch (Exception e) {
			try {
				in.close();
			} catch (IOException e1) {
			}
			reader.interrupt();
			reader.join();
			throw new ConnectionLostException(e);
		} finally {
			uart.close();
		}
		if (bytesVerified_ != BYTE_COUNT) {
			Log.w("IOIOTortureTest", "Failed UartTest input: " + inPin
					+ ", output: " + outPin + ". Bytes passed: "
					+ bytesVerified_);
			return false;
		}
		return true;
	}

	class ReaderThread extends Thread {
		private InputStream in_;
		private Random rand_;
		private int count_;

		public ReaderThread(InputStream in, int seed, int count) {
			in_ = in;
			rand_ = new Random(seed);
			count_ = count;
		}

		@Override
		public void run() {
			super.run();
			try {
				while (count_-- > 0) {
					int expected = rand_.nextInt() & 0xFF;
					int read = in_.read();
					if (read != expected) {
						Log.e("IOIOTortureTest", "Expected: " + expected + " got: " + read);
						return;
					} else {
						bytesVerified_++;
					}
				}
			} catch (IOException e) {
			}
		}
	}
}
