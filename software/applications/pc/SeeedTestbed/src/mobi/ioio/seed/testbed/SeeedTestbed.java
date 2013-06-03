package mobi.ioio.seed.testbed;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOConnectionManager.Thread;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.pc.IOIOConsoleApp;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class SeeedTestbed extends IOIOConsoleApp {
	// Boilerplate main(). Copy-paste this code into any IOIOapplication.
	public static void main(String[] args) throws Exception {
		new SeeedTestbed().go(args);
	}

	@Override
	protected void run(String[] args) throws IOException {
		BufferedReader reader = new BufferedReader(new InputStreamReader(
				System.in));
		boolean abort = false;
		String line;
		while (!abort && (line = reader.readLine()) != null) {
			if (line.equals("q")) {
				abort = true;
			} else {
				System.out.println("Unknown input. t=toggle, n=on, f=off, q=quit.");
			}
		}
	}

	@Override
	public IOIOLooper createIOIOLooper(String connectionType, Object extra) {
		return new BaseIOIOLooper() {
			private DigitalOutput[] pins_ = new DigitalOutput[45];
			int state_ = 0;

			@Override
			protected void setup() throws ConnectionLostException,
					InterruptedException {
				int j = 0;
				for (int i = 0; i <= 46; ++i) {
					if (i == 37 || i == 38) continue;
					pins_[j++] = ioio_.openDigitalOutput(i);
				}
			}

			@Override
			public void loop() throws ConnectionLostException,
					InterruptedException {
				for (int i = 0; i < pins_.length; ++i) {
					pins_[i] .write(state_ == (i & 1));
				}
				Thread.sleep(500);
				state_ ^= 1;
			}
		};
	}
}
