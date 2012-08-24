package ioio.examples.hello_console;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOConnectionManager.Thread;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.pc.IOIOConsoleApp;

public class HelloIOIOConsole extends IOIOConsoleApp {
	private boolean ledOn_ = false;

	// Boilerplate main(). Copy-paste this code into any IOIOapplication.
	public static void main(String[] args) throws Exception {
		new HelloIOIOConsole().go(args);
	}

	@Override
	protected void run(String[] args) throws IOException {
		BufferedReader reader = new BufferedReader(new InputStreamReader(
				System.in));
		boolean abort = false;
		String line;
		while (!abort && (line = reader.readLine()) != null) {
			if (line.equals("t")) {
				ledOn_ = !ledOn_;
			} else if (line.equals("n")) {
				ledOn_ = true;
			} else if (line.equals("f")) {
				ledOn_ = false;
			} else if (line.equals("q")) {
				abort = true;
			} else {
				System.out
						.println("Unknown input. t=toggle, n=on, f=off, q=quit.");
			}
		}
	}

	@Override
	public IOIOLooper createIOIOLooper(String connectionType, Object extra) {
		return new BaseIOIOLooper() {
			private DigitalOutput led_;

			@Override
			protected void setup() throws ConnectionLostException,
					InterruptedException {
				led_ = ioio_.openDigitalOutput(IOIO.LED_PIN, true);
			}

			@Override
			public void loop() throws ConnectionLostException,
					InterruptedException {
				led_.write(!ledOn_);
				Thread.sleep(10);
			}
		};
	}
}
