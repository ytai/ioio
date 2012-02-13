package ioio.tests.torture;

import ioio.lib.api.exception.ConnectionLostException;

import java.util.Timer;
import java.util.TimerTask;

import android.util.Log;

class TestThread extends Thread {
	private final TestProvider provider_;

	public TestThread(TestProvider provider) {
		provider_ = provider;
	}

	@Override
	public void run() {
		super.run();
		while (true) {
			Timer t = new Timer();
			t.schedule(new TimerTask() {
				@Override
				public void run() {
					StackTraceElement[] stackTrace = TestThread.this.getStackTrace();
					Log.e("TortureTest", "====== Hung test stack trace ======");
					for (StackTraceElement el : stackTrace) {
						Log.e("TortureTest", el.toString());
					}
				}
			}, 10000);
			try {
				TestRunner runner = provider_.newTest();
				Log.i("TortureTest", "TestThread [" + getId() + "] starting test " + runner.testClassName());
				runner.run();
				Log.i("TortureTest", "TestThread [" + getId() + "] finished test " + runner.testClassName());
			} catch (InterruptedException e) {
				Log.i("TortureTest", "TestThread [" + getId() + "] interrupted, exiting.");
				break;
			} catch (ConnectionLostException e) {
				Log.i("TortureTest",
						"IOIO connection lost, TestThread [" + getId() + "] exiting.");
				break;
			} finally {
				t.cancel();
			}
		}
	}
}
