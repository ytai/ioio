package ioio.test_bed;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.DigitalInput.Spec.Mode;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.android.IOIOActivity;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class IOIOTestBed extends IOIOActivity {
	private static final String LOG_TAG = "IOIO_TEST_BED";
	private static final int PIN_COUNT = 48;

	private static final Set<Integer> IGNORE_PINS_SET = new HashSet<Integer>(
			Arrays.asList(37, 38));

	private TextView EVEN_TEXT;
	private TextView ODD_TEXT;

	private static final int SAMPLING_DELAY = 100;
	private static final int LED_BLINK_SPEED = 250;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		EVEN_TEXT = (TextView) findViewById(R.id.even_text);
		ODD_TEXT = (TextView) findViewById(R.id.odd_text);
	}

	class Looper extends BaseIOIOLooper {
		Thread ledThread;

		public Looper() {
			disableUI();
		}
		
		@Override
		protected void setup() throws ConnectionLostException,
				InterruptedException {
			Log.i(LOG_TAG, "Connected.");
			ledThread = new LEDThread(ioio_);
			ledThread.start();
			Log.i(LOG_TAG, "Thread started");
			runTest();
		}

		@Override
		public void loop() throws ConnectionLostException,
				InterruptedException {
		}

		@Override
		public void disconnected() {
			Log.i(LOG_TAG, "IOIO disconnected");
			disableUI();
			if (ledThread != null) {
				Log.i(LOG_TAG, "Joining LED thread...");
				ledThread.interrupt();
				try {
					ledThread.join();
					ledThread = null;
					Log.i(LOG_TAG, "Joined LED thread...");
				} catch (InterruptedException e) {
				}
			}
		}

		@Override
		public void incompatible() {
			Log.e(LOG_TAG, "Incompatbile firmware!");
		}

		private void disableUI() {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					EVEN_TEXT.setText("???");
					EVEN_TEXT.setTextColor(Color.RED);
					ODD_TEXT.setText("???");
					ODD_TEXT.setTextColor(Color.RED);
				}
			});
		}

		private void runTest() {
			try {
				Log.i(LOG_TAG, "Test is running...");
				List<DigitalInput> pins = new ArrayList<DigitalInput>(PIN_COUNT);
				List<Integer> evenHigh = new ArrayList<Integer>(
						(PIN_COUNT + 1) / 2);
				List<Integer> evenLow = new ArrayList<Integer>(
						(PIN_COUNT + 1) / 2);
				List<Integer> oddHigh = new ArrayList<Integer>(
						(PIN_COUNT + 1) / 2);
				List<Integer> oddLow = new ArrayList<Integer>(
						(PIN_COUNT + 1) / 2);
				pins.add(null);  // for pin 0 - LED
				for (int i = 1; i <= PIN_COUNT; ++i) {
					if (IGNORE_PINS_SET.contains(i)) {
						pins.add(null);
					} else {
						pins.add(ioio_.openDigitalInput(i, Mode.PULL_UP));
					}
				}
				while (true) {
					evenHigh.clear();
					evenLow.clear();
					oddHigh.clear();
					oddLow.clear();
					for (int i = 0; i < pins.size(); ++i) {
						if (pins.get(i) == null) {
							continue;
						}
						// even pins below 9 belong to the "odd" group and odd
						// pins below 10 belong to the "even" group.
						if ((i > 9) == (i % 2 == 0)) {
							if (pins.get(i).read()) {
								evenHigh.add(i);
							} else {
								evenLow.add(i);
							}
						} else {
							if (pins.get(i).read()) {
								oddHigh.add(i);
							} else {
								oddLow.add(i);
							}
						}
					}
					updateLabel(EVEN_TEXT, evenHigh, evenLow);
					updateLabel(ODD_TEXT, oddHigh, oddLow);
					Thread.sleep(SAMPLING_DELAY);
				}
			} catch (Exception e) {
				Log.i(LOG_TAG, "Test is stopped", e);
			}
		}
	}

	class LEDThread extends Thread {
		private DigitalOutput led;
		private IOIO ioio;

		public LEDThread(IOIO ioio) {
			this.ioio = ioio;
		}

		@Override
		public void run() {
			try {
				boolean ledState = true;
				led = ioio.openDigitalOutput(0, ledState);
				while (true) {
					ledState = !ledState;
					led.write(ledState);
					Thread.sleep(LED_BLINK_SPEED);
				}
			} catch (Exception e) {
				Log.i(LOG_TAG, "LED thread is stopped");
			}
		};
	}

	private void updateLabel(final TextView textView, final List<Integer> high,
			final List<Integer> low) {
		final String text;
		final int color;
		if (high.size() == 0) {
			text = "All LOW";
			color = Color.GREEN;
		} else if (low.size() == 0) {
			text = "All HIGH";
			color = Color.YELLOW;
		} else {
			color = Color.RED;
			if (high.size() < low.size()) {
				StringBuilder sb = new StringBuilder("High:");
				for (int pin : high) {
					sb.append(" " + pin);
				}
				text = sb.toString();
			} else {
				StringBuilder sb = new StringBuilder("Low:");
				for (int pin : low) {
					sb.append(" " + pin);
				}
				text = sb.toString();
			}
		}
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				textView.setText(text);
				textView.setTextColor(color);
			}
		});
	}

	@Override
	protected IOIOLooper createIOIOLooper() {
		return new Looper();
	}
}