package ioio.examples.adk;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.AbstractIOIOAdkActivity;
import android.os.Bundle;
import android.util.Log;

public class AdkBlinkMainActivity extends AbstractIOIOAdkActivity {
	private static final String TAG = "AdkBlinkMainActivity";

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		Log.d(TAG, "onCreate");
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
	}

	class MainThread extends IOIOThread {
		private boolean ledState_ = false;
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
			ledState_ = !ledState_;
			led_.write(ledState_);
		}
	}

	@Override
	protected IOIOThread createIOIOThread() {
		return new MainThread();
	}
}