package ioio.lib.util;

import ioio.lib.api.IOIO;
import ioio.lib.api.IOIOFactory;
import ioio.lib.api.exception.ConnectionLostException;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public abstract class AbstractIOIOActivity extends Activity {
	private IOIOThread ioio_thread_;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	}

	@Override
	protected void onPause() {
		super.onPause();
		ioio_thread_.abort();
		try {
			ioio_thread_.join();
		} catch (InterruptedException e) {
		}
	}

	@Override
	protected void onResume() {
		super.onResume();
		ioio_thread_ = createIOIOThread();
		ioio_thread_.start();
	}

	protected abstract IOIOThread createIOIOThread();

	protected abstract class IOIOThread extends Thread {
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
						ioio_ = IOIOFactory.create();
					}
					ioio_.waitForConnect();
					setup();
					while (true) {
						loop();
					}
				} catch (ConnectionLostException e) {
					if (abort_) {
						break;
					}
				} catch (Exception e) {
					Log.e("AbstractIOIOActivity",
							"Unexpected exception caught", e);
					ioio_.disconnect();
					break;
				} finally {
					try {
						ioio_.waitForDisconnect();
					} catch (InterruptedException e) {
					}
				}
			}
		}

		protected void setup() throws ConnectionLostException {
		}

		protected void loop() throws ConnectionLostException {
		}

		public synchronized final void abort() {
			abort_ = true;
			if (ioio_ != null) {
				ioio_.disconnect();
			}
		}
	}
}
