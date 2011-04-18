package ioio.sample;

import ioio.api.AnalogInput;
import ioio.api.DigitalOutput;
import ioio.api.IOIOLib;
import ioio.api.PeripheralException;
import ioio.api.PwmOutput;
import ioio.lib.Constants;
import ioio.lib.IOIOImpl;

import java.net.SocketException;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.ToggleButton;

public class IOIOSimpleApp extends Activity {
	private static final String LOG_TAG = "IOIO_SIMPLE_APP";

	private TextView textView;
	private SeekBar seekBar;
	private ToggleButton toggleButton;

	private int seekBarProgress = 0;

	private boolean disconnecting = false;
	private IOIOThread thread;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        textView = (TextView)findViewById(R.id.TextView);
        seekBar = (SeekBar)findViewById(R.id.SeekBar);
        toggleButton = (ToggleButton)findViewById(R.id.ToggleButton);

        Log.i(LOG_TAG, "Assigning listener to the SeekBar");
        seekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) { }
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) { }
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				seekBarProgress = progress;
			}
		});

        seekBar.setEnabled(false);
        toggleButton.setEnabled(false);
    }

    @Override
    protected void onResume() {
    	super.onResume();

    	disconnecting = false;
    	thread = new IOIOThread();
    	thread.start();
    }

    @Override
    protected void onPause() {
    	super.onPause();

    	disconnecting = true;
    	try {
			thread.join();
		} catch (InterruptedException e) {
			Log.e(LOG_TAG, e.getMessage());
		}
    }

    class IOIOThread extends Thread {
    	@Override
    	public void run() {
    		super.run();

    		IOIOLib ioio = new IOIOImpl();
    		while (!disconnecting) {
	    		try {
					ioio.waitForConnect();
					Log.i(LOG_TAG, "Connection to IOIO aquired");

					runOnUiThread(new Runnable() {
						@Override
						public void run() {
							seekBar.setEnabled(true);
							toggleButton.setEnabled(true);
						}
					});

					AnalogInput input = ioio.openAnalogInput(40);
					PwmOutput pwmOutput = ioio.openPwmOutput(12, 100);
					DigitalOutput led = ioio.openDigitalOutput(Constants.LED_PIN, true);

					while (!disconnecting) {
						final float reading = input.read();
						runOnUiThread(new Runnable() {
							@Override
							public void run() {
								textView.setText(Float.toString(reading));
							}
						});

						pwmOutput.setPulseWidth(1000 + seekBarProgress);

						led.write(!toggleButton.isChecked());

						sleep(10);
					}
				} catch (PeripheralException e) {
					Log.e(LOG_TAG, e.getMessage());
				} catch (SocketException e) {
					Log.e(LOG_TAG, e.getMessage());
				} catch (InterruptedException e) {
					Log.e(LOG_TAG, e.getMessage());
				} finally {
					ioio.disconnect();
					runOnUiThread(new Runnable() {
						@Override
						public void run() {
							seekBar.setEnabled(false);
							toggleButton.setEnabled(false);
							toggleButton.setChecked(false);
						}
					});
				}
    		}
    	}
    }
}