package ioio.examples.hello;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.IOIOFactory;
import ioio.lib.api.exception.ConnectionLostException;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import android.widget.ToggleButton;

public class MainActivity extends Activity {
	private TextView title_;
	private ToggleButton button_;
	private IOIOThread ioio_thread_;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		title_ = (TextView) findViewById(R.id.title);
		button_ = (ToggleButton) findViewById(R.id.button);
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
		ioio_thread_ = new IOIOThread();
		ioio_thread_.start();
	}


	class IOIOThread extends Thread {
		private IOIO ioio_;
		private boolean abort_ = false; 

		@Override
		public void run() {
			super.run();
			while (!abort_) {
				ioio_ = IOIOFactory.create();
				try {
					setText("Waiting for IOIO...");
					ioio_.waitForConnect();
					setText("IOIO connected!");
					DigitalOutput led = ioio_.openDigitalOutput(0, true);
					while (true) {
						led.write(!button_.isChecked());
						sleep(10);
					}
				} catch (ConnectionLostException e) {
				} catch (Exception e) {
					Log.e("IOIOTester", "Exception caught", e);
					ioio_.disconnect();
				} finally {
					try {
						ioio_.waitForDisconnect();
					} catch (InterruptedException e) {
					}
				}
			}
		}

		public void abort() {
			abort_ = true;
			ioio_.disconnect();
			try {
				ioio_.waitForDisconnect();
			} catch (InterruptedException e) {
			}
		}
		
		private void setText(final String str) {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					title_.setText(str);
				}
			});
		}
	}
}