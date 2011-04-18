package ioio.examples.simple;

import ioio.lib.api.AnalogInput;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.IOIOFactory;
import ioio.lib.api.PwmOutput;
import ioio.lib.api.exception.ConnectionLostException;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.ToggleButton;

public class IOIOSimpleApp extends Activity {
	private static final String LOG_TAG = "IOIO_SIMPLE_APP";

	private TextView textView_;
	private SeekBar seekBar_;
	private ToggleButton toggleButton_;

	private boolean disconnecting = false;
	private IOIOThread thread;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        textView_ = (TextView)findViewById(R.id.TextView);
        seekBar_ = (SeekBar)findViewById(R.id.SeekBar);
        toggleButton_ = (ToggleButton)findViewById(R.id.ToggleButton);

        seekBar_.setEnabled(false);
        toggleButton_.setEnabled(false);
    }

    @Override
    protected void onResume() {
    	super.onResume();
    	thread = new IOIOThread();
    	thread.start();
    }

    @Override
    protected void onPause() {
    	super.onPause();
    	thread.abort();
    	try {
			thread.join();
		} catch (InterruptedException e) {
			Log.e(LOG_TAG, e.getMessage());
		}
    }
    
	class IOIOThread extends Thread {
		private IOIO ioio_;
		private boolean abort_ = false;

		/** Thread body. */
		@Override
		public void run() {
			super.run();
			while (true) {
				synchronized (this) {
					if (abort_) {
						break;
					}
					ioio_ = IOIOFactory.create();
				}
				try {
					ioio_.waitForConnect();
					runOnUiThread(new Runnable() {
						@Override
						public void run() {
							seekBar_.setEnabled(true);
							toggleButton_.setEnabled(true);
						}
					});

					AnalogInput input = ioio_.openAnalogInput(40);
					PwmOutput pwmOutput = ioio_.openPwmOutput(12, 100);
					DigitalOutput led = ioio_.openDigitalOutput(0, true);

					while (!disconnecting) {
						final float reading = input.read();
						runOnUiThread(new Runnable() {
							@Override
							public void run() {
								textView_.setText(Float.toString(reading));
							}
						});

						pwmOutput.setPulseWidth(1000 + seekBar_.getProgress());

						led.write(!toggleButton_.isChecked());

						sleep(10);
					}
				} catch (ConnectionLostException e) {
				} catch (Exception e) {
					Log.e("IOIOSimpleApp", "Unexpected exception caught", e);
					ioio_.disconnect();
					break;
				} finally {
					try {
						runOnUiThread(new Runnable() {
							@Override
							public void run() {
								seekBar_.setEnabled(false);
								toggleButton_.setEnabled(false);
							}
						});
						ioio_.waitForDisconnect();
					} catch (InterruptedException e) {
					}
				}
			}
		}

		/**
		 * Abort the connection.
		 * 
		 * This is a little tricky synchronization-wise: we need to be handle
		 * the case of abortion happening before the IOIO instance is created or
		 * during its creation.
		 */
		synchronized public void abort() {
			abort_ = true;
			if (ioio_ != null) {
				ioio_.disconnect();
			}
		}
	}
}