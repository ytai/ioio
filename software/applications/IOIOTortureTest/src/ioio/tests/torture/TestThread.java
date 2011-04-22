package ioio.tests.torture;

import android.util.Log;
import ioio.lib.api.exception.ConnectionLostException;

class TestThread extends Thread {
	private final TestProvider provider_;

	public TestThread(TestProvider provider) {
		provider_ = provider;
	}

	@Override
	public void run() {
		super.run();
		while (true) {
			try {
				TestRunner runner = provider_.newTest();
				runner.run();
			} catch (InterruptedException e) {
				Log.i("TortureTest", "TestThread interrupted, exiting.");
				break;
			} catch (ConnectionLostException e) {
				Log.i("TortureTest",
						"IOIO connection lost, TestThread exiting.");
				break;
			}
		}
	}
}
