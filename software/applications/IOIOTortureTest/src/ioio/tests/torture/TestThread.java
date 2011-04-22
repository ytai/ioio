package ioio.tests.torture;

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
				break;
			} catch (ConnectionLostException e) {
				break;
			}
		}
	}
}
