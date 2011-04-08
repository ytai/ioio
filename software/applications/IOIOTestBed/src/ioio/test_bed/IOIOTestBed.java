package ioio.test_bed;

import ioio.api.DigitalInput;
import ioio.api.DigitalOutput;
import ioio.api.IOIOLib;
import ioio.api.PeripheralException.ConnectionLostException;
import ioio.api.PeripheralException.InvalidStateException;
import ioio.api.PeripheralInterface;
import ioio.lib.Constants;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class IOIOTestBed extends Activity {
	private static final String LOG_TAG = "IOIO_TEST_BED";
	private static final int PINS_COUNT = 6;

	private static final Set<Integer> IGNORE_PINS_SET = new HashSet<Integer>(Arrays.asList(37, 38));

	private TextView EVEN_TEXT;
	private TextView ODD_TEXT;

	private IOIOLib ioio;
	private List<DigitalInput> pins = new ArrayList<DigitalInput>(PINS_COUNT);
	private DigitalOutput led;

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

    Thread thread;
    Thread ledThread;
    boolean shouldRun = false;

    @Override
    protected void onResume() {
    	super.onResume();
    	Log.i(LOG_TAG, "onResume()");
    	connect();
    }

    @Override
    protected void onPause() {
    	super.onPause();
    	Log.i(LOG_TAG, "onPaush()");
    	disconnect();
    }

    private void connect() {
    	runOnUiThread(new Runnable() {
			@Override
			public void run() {
				EVEN_TEXT.setText("???");
				EVEN_TEXT.setTextColor(Color.RED);
				ODD_TEXT.setText("???");
				ODD_TEXT.setTextColor(Color.RED);
			}
		});

    	Log.i(LOG_TAG, "Connecting to IOIO...");
    	try {
			ioio = PeripheralInterface.waitForController();
			led = ioio.openDigitalOutput(Constants.LED_PIN, true);
			for (int i = 1; i <= PINS_COUNT; ++i) {
				if (IGNORE_PINS_SET.contains(i)) {
					pins.add(null);
				} else {
					pins.add(ioio.openDigitalInput(i));
				}
			}
		} catch (Exception e) {
			Log.e(LOG_TAG, "Couldn't open connection to IOIO.", e);
			return;
		}
		Log.i(LOG_TAG, "Connected.");

    	thread = new Thread() {
    		@Override
    		public void run() {
    			Log.i(LOG_TAG, "Main thread is running...");
    			while (shouldRun) {
    				List<Integer> evenHigh = new ArrayList<Integer>();
    				List<Integer> evenLow = new ArrayList<Integer>();
    				List<Integer> oddHigh = new ArrayList<Integer>();
    				List<Integer> oddLow = new ArrayList<Integer>();
    				try {
	    				for (int i = 0; i < PINS_COUNT; ++i) {
	    					if (pins.get(i) == null) {
	    						continue;
	    					}
							if (pins.get(i).getPinNumber() % 2 == 0) {
								if (pins.get(i).read()) {
									evenHigh.add(pins.get(i).getPinNumber());
								} else {
									evenLow.add(pins.get(i).getPinNumber());
								}
							} else {
								if (pins.get(i).read()) {
									oddHigh.add(pins.get(i).getPinNumber());
								} else {
									oddLow.add(pins.get(i).getPinNumber());
								}
	    					}
	    				}
	    				updateLabel(EVEN_TEXT, evenHigh, evenLow);
	    				updateLabel(ODD_TEXT, oddHigh, oddLow);
    				} catch (InvalidStateException e) {
    					Log.w(LOG_TAG, e);
    					break;
    				}

    				try {
						Thread.sleep(SAMPLING_DELAY);
					} catch (InterruptedException e) {
						Log.w(LOG_TAG, e);
					}
    			}
    			Log.i(LOG_TAG, "Main thread is stopped");
    		};
    	};

    	ledThread = new Thread() {
    		@Override
    		public void run() {
    			boolean ledState = true;
    			while (shouldRun) {
    				ledState = !ledState;
    				try {
    					led.write(ledState);
						Thread.sleep(LED_BLINK_SPEED);
					} catch (InterruptedException e) {
						Log.w(LOG_TAG, e);
					} catch (ConnectionLostException e) {
						restart();
						return;
					} catch (InvalidStateException e) {
						// Invalid state? try to reconnect.
						Log.w(LOG_TAG, e);
						restart();
						break;
					}
    			}
    			Log.i(LOG_TAG, "LED thread is stopped");
    		};
    	};
    	shouldRun = true;
    	thread.start();
    	ledThread.start();

    	Log.i(LOG_TAG, "Threads started");
    }

    private void updateLabel(final TextView textView, final List<Integer> high, final List<Integer> low) {
    	runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (high.size() == 0) {
					textView.setText("All LOW");
					textView.setTextColor(Color.GREEN);
					return;
				}
				if (low.size() == 0) {
					textView.setText("All HIGH");
					textView.setTextColor(Color.GREEN);
					return;
				}
				textView.setTextColor(Color.RED);
				if (high.size() < low.size()) {
					StringBuilder sb = new StringBuilder("High:");
					for (int pin : high) {
						sb.append(" " + pin);
					}
					textView.setText(sb.toString());
				} else {
					StringBuilder sb = new StringBuilder("Low:");
					for (int pin : low) {
						sb.append(" " + pin);
					}
					textView.setText(sb.toString());
				}
			}
		});
    }

    private void disconnect() {
    	Log.i(LOG_TAG, "disconnet()");
    	shouldRun = false;
    	try {
	    	if (thread != null) {
	    		Log.i(LOG_TAG, "Joining main thread...");
				thread.join();
	    	}
	    	if (ledThread != null) {
	    		Log.i(LOG_TAG, "Joining LED thread...");
	    		ledThread.join();
	    	}
    	} catch (InterruptedException e) {
    		Log.e(LOG_TAG, "Interrupt on disconnect", e);
    	}
    	pins.clear();
    	led = null;
    	Log.i(LOG_TAG, "Disconnecting IOIO");
    	if (ioio != null) {
    		ioio.disconnect();
    	}
    	ioio = null;
    	Log.i(LOG_TAG, "Disconnect complete");
    }

    private void restart() {
    	Log.i(LOG_TAG, "Restarting...");
    	new Thread() {
    		public void run() {
    			disconnect();
    			connect();
    		};
    	}.start();
    }
}