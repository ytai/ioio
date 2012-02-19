package ioio.tests.digital_latency;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.AbstractIOIOActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AbstractIOIOActivity {
	private TextView text_;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		text_ = (TextView) findViewById(R.id.text_view);
	}

	class IOIOThread extends AbstractIOIOActivity.IOIOThread {
		private DigitalOutput outPin_;
		private DigitalInput inPin_;
		boolean value_ = false;

		@Override
		protected void setup() throws ConnectionLostException {
			outPin_ = ioio_.openDigitalOutput(1);
			inPin_ = ioio_.openDigitalInput(2);
		}

		@Override
		protected void loop() throws ConnectionLostException {
			try {
				outPin_.write(value_);
				long startTime = System.nanoTime();
				inPin_.waitForValue(value_);
				float deltaTime = (System.nanoTime() - startTime) / 1000000.f;
				setText(Float.toString(deltaTime));
				value_ = !value_;
			} catch (InterruptedException e) {
			}
		}
	}
	
	private void setText(final String text) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				text_.setText(text);
			}
		});
	}

	@Override
	protected AbstractIOIOActivity.IOIOThread createIOIOThread() {
		return new IOIOThread();
	}
}