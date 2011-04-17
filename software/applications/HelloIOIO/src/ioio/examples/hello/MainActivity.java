package ioio.examples.hello;

import ioio.examples.hello.R;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.AbstractIOIOActivity;
import android.os.Bundle;
import android.widget.ToggleButton;

public class MainActivity extends AbstractIOIOActivity {
	private ToggleButton button_;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		button_ = (ToggleButton) findViewById(R.id.button);
	}

	class IOIOThread extends AbstractIOIOActivity.IOIOThread {
		private DigitalOutput led_;

		@Override
		protected void setup() throws ConnectionLostException {
			led_ = ioio_.openDigitalOutput(0, true);
		}

		@Override
		protected void loop() throws ConnectionLostException {
			led_.write(!button_.isChecked());
			try {
				sleep(10);
			} catch (InterruptedException e) {
			}
		}
	}

	@Override
	protected AbstractIOIOActivity.IOIOThread createIOIOThread() {
		return new IOIOThread();
	}
}