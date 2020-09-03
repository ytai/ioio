package ioio.tests.torture;

import ioio.lib.api.exception.ConnectionLostException;

import java.util.Timer;
import java.util.TimerTask;

import android.util.Log;

class TestThread extends Thread {
	private final TestProvider provider_;
	private boolean run_ = true;
	private Timer timer_ = new Timer();

	public TestThread(TestProvider provider, String name) {
		super(name);
		provider_ = provider;
	}

	@Override
	public void run() {
		Log.d("TortureTest", "TestThread [" + getName() + "] is running.");
		try {
			while (run_) {
				TestRunner runner;
				try {
					runner = provider_.newTest();
				} catch (InterruptedException e) {
					continue;
				}
				// This TimerTask will interrupt the thread if a test hangs and print out the stack trace. 
				final TimerTask timerTask = new TimerTask() {
					@Override
					public void run() {
						StackTraceElement[] stackTrace = TestThread.this.getStackTrace();
						Log.e("TortureTest", "====== Hung test on thread [" + TestThread.this.getName()
								+ "]. Stack trace ======");
						for (StackTraceElement el : stackTrace) {
							Log.e("TortureTest", el.toString());
						}
						TestThread.this.interrupt();
					}
				};
				timer_.schedule(timerTask, 10000);
				try {
					Log.i("TortureTest", "TestThread [" + getName() + "] starting test " + runner.testClassName());
					runner.run();
					Log.i("TortureTest", "TestThread [" + getName() + "] finished test " + runner.testClassName());
				} catch (InterruptedException e) {
					Log.i("TortureTest", "TestThread [" + getName() + "] interrupted.");
				} catch (ConnectionLostException e) {
					Log.i("TortureTest", "IOIO connection lost, TestThread [" + getName() + "] exiting.");
					break;
				} finally {
					timerTask.cancel();
				}
			}
		} finally {
			Log.d("TortureTest", "TestThread [" + getName() + "] is exiting.");
		}
	}

	public void abort() {
		run_ = false;
		interrupt();
	}
}
