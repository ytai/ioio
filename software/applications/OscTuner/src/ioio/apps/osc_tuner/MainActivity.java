package ioio.apps.osc_tuner;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.android.IOIOActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends IOIOActivity {
	private TextView textView_;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		textView_ = (TextView) findViewById(R.id.textView);
	}

	class Looper extends BaseIOIOLooper {
		private DigitalOutput led_;
		private DigitalInput input_;
		boolean value_;
		long startTime_;
		int pulses_;

		@Override
		protected void setup() throws ConnectionLostException {
			led_ = ioio_.openDigitalOutput(0, true);
			input_ = ioio_.openDigitalInput(38,
					DigitalInput.Spec.Mode.PULL_DOWN);
			value_ = true;
			startTime_ = 0;
			pulses_ = 0;
		}

		@Override
		public void loop() throws ConnectionLostException, InterruptedException {
			input_.waitForValue(value_);
			long time = System.nanoTime();
			led_.write(value_);
			if (startTime_ == 0) {
				startTime_ = time;
			} else {
				++pulses_;
				final double deviation = ((double) (time - startTime_) * 1e-9 / pulses_ - 1.0) * 100;
				runOnUiThread(new Runnable() {
					@Override
					public void run() {
						textView_.setText(String.format("%.3f%%", deviation));
					}
				});
			}
			value_ = !value_;
		}
	}

	@Override
	protected IOIOLooper createIOIOLooper() {
		return new Looper();
	}
}