package ioio.examples.adk;

import ioio.lib.adk.BaseIOIOAdkActivity;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import android.os.Bundle;
import android.util.Log;

public class IOIOLibAdkTestMainActivity extends BaseIOIOAdkActivity {
	private static final String TAG = "IOIOLibAdkTestMainActivity";

	private IOIOThread thread_;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		Log.d(TAG, "onCreate");
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
	}

	@Override
	public void onResume() {
		Log.d(TAG, "onResume");
		super.onResume();
		thread_ = new RealIOIOThread();
		thread_.start();
	}

	@Override
	public void onPause() {
		Log.d(TAG, "onPause");
		super.onPause();
		Log.d(TAG, "aborting");
		thread_.abort();
		Log.d(TAG, "joining");
		try {
			thread_.join();
		} catch (InterruptedException e) {
		}
		Log.d(TAG, "joined");
	}
	
	protected class IOIOThread extends Thread {
		protected IOIO ioio_;
		private boolean abort_ = false;

		@Override
		public final void run() {
			super.run();
			while (true) {
				try {
					synchronized (this) {
						if (abort_) {
							break;
						}
						Log.d(TAG, "Creating IOIO");
						ioio_ = createIOIO();
						Log.d(TAG, "Created IOIO");
					}
					ioio_.waitForConnect();
					setup();
					while (true) {
						loop();
					}
				} catch (ConnectionLostException e) {
					Log.d(TAG, "Caught ConnectionLostException");
					if (abort_) {
						break;
					}
				} catch (Exception e) {
					Log.e(TAG, "Unexpected exception caught", e);
					ioio_.disconnect();
					break;
				} finally {
					try {
						Log.d(TAG, "Waiting for disconnect");
						ioio_.waitForDisconnect();
					} catch (InterruptedException e) {
					}
					Log.d(TAG, "thread done");
				}
			}
		}

		protected void setup() throws ConnectionLostException {
		}

		protected void loop() throws ConnectionLostException {
		}

		public synchronized final void abort() {
			Log.d(TAG, "abort()");
			abort_ = true;
			if (ioio_ != null) {
				ioio_.disconnect();
			}
		}
	}

	class RealIOIOThread extends IOIOThread {
		private boolean state_ = false;
		private DigitalOutput led_;
		
		@Override
		protected void setup() throws ConnectionLostException {
			led_ = ioio_.openDigitalOutput(IOIO.LED_PIN);
		}
		
		@Override
		protected void loop() throws ConnectionLostException {
			try {
				Thread.sleep(500);
			} catch (InterruptedException e) {
			}
			state_ = !state_;
			led_.write(state_);
		}
	}
}