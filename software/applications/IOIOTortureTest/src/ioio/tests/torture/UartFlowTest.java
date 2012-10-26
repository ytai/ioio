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

import ioio.lib.api.Uart.FlowMode;

public class UartFlowTest implements Test<Boolean> {
	private final IOIO ioio_;
	private final ResourceAllocator alloc_;
	private final int pin1_;
	private final int pin2_;
	private final int pin3_;
	private final int pin4_;
	private int bytesVerified_;
	private final static FlowMode[] modeMap_ = new FlowMode[] {
	  Uart.FlowMode.NONE, 
	  Uart.FlowMode.IRDA, 
	  Uart.FlowMode.RTSCTS, 
	  Uart.FlowMode.RS485};
    private int modeCount_;

	public UartFlowTest(IOIO ioio, ResourceAllocator alloc)
			throws InterruptedException {
	    final Random rand = new Random();
		ioio_ = ioio;
		alloc_ = alloc;
		pin1_ = alloc.allocatePinPair(ResourceAllocator.PIN_PAIR_PERIPHERAL);
		pin2_ = pin1_ + 1;
		pin3_ = alloc.allocatePinPair(ResourceAllocator.PIN_PAIR_PERIPHERAL);
		alloc.allocPeripheral(PeripheralType.UART);
		pin4_ = pin3_ + 1;
        modeCount_ = rand.nextInt(3);
	}

	@Override
	public Boolean run() throws ConnectionLostException, InterruptedException {
		Log.i("IOIOTortureTest", "Starting UartFlowTest on pins: " + pin1_ + ", "
				+ pin2_ + ", " + pin3_ + ", " + pin4_);
		try {
			if (!runTest(pin1_, pin2_, pin3_, pin4_)) {
				return false;
			}
			if (!runTest(pin2_, pin1_, pin4_, pin3_)) {
				return false;
			}
		} finally {
			alloc_.freePinPair(pin1_);
			alloc_.freePinPair(pin3_);
			alloc_.freePeripheral(PeripheralType.UART);
		}
		Log.i("IOIOTortureTest", "Passed UartFlowTest on pins: " + pin1_ + ", "
				+ pin2_ + ", " + pin3_ + ", " + pin4_);
		return true;
	}

	private boolean runTest(int inPin, int outPin, int rtsPin, int ctsPin)
			throws ConnectionLostException, InterruptedException {
		if ((outPin == 9) || (rtsPin == 9)) {
			// pin 9 doesn't support peripheral output
			return true;
		}
		final int BYTE_COUNT = 2000;
		final int SEED = 17;
		FlowMode mode = modeMap_[modeCount_ & 0x3];
        modeCount_ += 1;

		Uart uart = ioio_.openUart(inPin, outPin, 115200, Uart.Parity.NONE,
                                   Uart.StopBits.ONE, mode, rtsPin, ctsPin);
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
		} catch (IOException e) {
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
			Log.w("IOIOTortureTest", "Failed UartFlowTest input: " + inPin
					+ ", output: " + outPin + ", " + rtsPin + ", " + ctsPin 
					+ ". Bytes passed: " + bytesVerified_);
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
