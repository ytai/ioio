package ioio.hackathon_app;

import java.net.SocketException;
import java.util.Timer;
import java.util.TimerTask;

import ioio.api.AnalogInput;
import ioio.api.DigitalOutput;
import ioio.api.DigitalOutputMode;
import ioio.api.IOIOLib;
import ioio.api.PeripheralException;
import ioio.api.PeripheralInterface;
import ioio.api.PwmOutput;
import ioio.lib.Constants;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.ToggleButton;

public class HackathonApp extends Activity {
	private static final String LOG_TAG = "IOIO_HACKATHON_APP";

	private TextView textView;
	private SeekBar seekBar;
	private ToggleButton toggleButton;

	private IOIOLib ioio;

	private Timer timer;
	private Thread servoThread;
	private boolean servoRunner = false;

	private float servoMin = .1f;
	private float servoMax = .2f;
	private float servoSpeed = .001f;

	private boolean disconnecting = false;

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
				Log.i(LOG_TAG, "Progress changed. New progress: " + Integer.toString(progress));
				// Servo's manufacturer specifications for range are 0.1 through 0.2. Our seek bar
				// will control the range from full (0.1 through 0.2) to empty (0.15 through 0.15).
				float normalizedProgress = progress / 2000.f;
				servoMin = 0.15f - normalizedProgress;
				servoMax = 0.15f + normalizedProgress;
			}
		});

        seekBar.setEnabled(false);
        toggleButton.setEnabled(false);
    }

    @Override
    protected void onStart() {
    	super.onStart();
    	reconnect();
    }

    private void reconnect() {
    	Log.i(LOG_TAG, "Starting communication thread");
    	new CommunicationThread().start();
	}

    @Override
    protected void onPause() {
    	super.onPause();

    	// Dropping IOIO connection, so other applications could acquire it.
    	boardDisconnected(false);
    	ioio.disconnect();
    }

    private void boardDisconnected(boolean reconnect) {
    	synchronized(this) {
	    	if (disconnecting) {
	    		return;
	    	}
	    	disconnecting = true;
    	}
    	Log.i(LOG_TAG, "Disconnecting from IOIO");
    	new Thread() {
    		@Override
    		public void run() {
    			super.run();

    	    	timer.cancel();
    	    	servoRunner = false;
    			try {
    				servoThread.join();
    			} catch (InterruptedException e) {
    				Log.e(LOG_TAG, e.getMessage());
    			}
    		}
    	}.start();
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				seekBar.setEnabled(false);
				toggleButton.setEnabled(false);
				toggleButton.setChecked(false);
			}
		});
		disconnecting = false;
		if (reconnect) {
			reconnect();
		}
    }

    private class CommunicationThread extends Thread {
		@Override
		public void run() {
			super.run();

			try {
				ioio = PeripheralInterface.waitForController();
				Log.i(LOG_TAG, "Connection to IOIO aquired");
				runOnUiThread(new Runnable() {
					@Override
					public void run() {
						seekBar.setEnabled(true);
						toggleButton.setEnabled(true);
					}
				});
			} catch (PeripheralException.OperationAbortedException e) {
				Log.e(LOG_TAG, e.getMessage());
				return;
			} catch (SocketException e) {
				Log.e(LOG_TAG, e.getMessage());
				return;
			}

			// Setting pin 40 for picking up analog signals.
			final AnalogInput input;
			try {
				input = ioio.openAnalogInput(40);
			} catch (PeripheralException e) {
				Log.e(LOG_TAG, e.getMessage());
				boardDisconnected(true);
				return;
			}

			// We'll read the updated analog signal every 100ms.
			timer = new Timer();
	        timer.schedule(new TimerTask() {
				@Override
				public void run() {
					final float potentiometerPosition;
					try {
						potentiometerPosition = input.read();
					} catch (PeripheralException e) {
						Log.e(LOG_TAG, e.getMessage());
						boardDisconnected(true);
						return;
					}
					servoSpeed = 0.0001f + potentiometerPosition*0.01f;
					runOnUiThread(new Runnable() {
						@Override
						public void run() {
							textView.setText(Float.toString(potentiometerPosition));
						}
					});
				}
			}, 0, 100);

			// Setting pin 12 for PWM output.
			final PwmOutput pwmOutput;
			try {
				pwmOutput = ioio.openPwmOutput(12, 100);
			} catch (PeripheralException e) {
				Log.e(LOG_TAG, e.getMessage());
				boardDisconnected(true);
				return;
			}

			// Start the thread that moves the servo constantly.
			servoRunner = true;
			servoThread = new Thread() {
				@Override
				public void run() {
					super.run();

					Log.i(LOG_TAG, "Starting servo process...");
					int direction = 1;
					float pos = .1f;
					while (servoRunner) {
						if (pos > servoMax) {
							direction = -1;
						} else if (pos < servoMin) {
							direction = 1;
						}
						pos += servoSpeed*direction;
						// Setting the new position of the servo.
						try {
							pwmOutput.setDutyCycle(pos);
						} catch (PeripheralException e) {
							Log.e(LOG_TAG, e.getMessage());
							boardDisconnected(true);
							return;
						}
						// Waiting for 10ms
						try {
							sleep(10);
						} catch (InterruptedException e) {
							Log.w(LOG_TAG, e.getMessage());
						}
					}
				}
			};
			servoThread.start();

	        final DigitalOutput led;
			try {
				led = ioio.openDigitalOutput(Constants.LED_PIN, true, DigitalOutputMode.OPEN_DRAIN);
			} catch (PeripheralException e) {
				Log.e(LOG_TAG, e.getMessage());
				boardDisconnected(true);
				return;
			}

	        toggleButton.setOnCheckedChangeListener(new OnCheckedChangeListener() {
				@Override
				public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
					// The led is connected in a way, where 0 is light, 1 is off.
					try {
						led.write(!isChecked);
					} catch (PeripheralException e) {
						Log.e(LOG_TAG, e.getMessage());
						boardDisconnected(true);
						return;
					}
				}
			});
		}
    }
}
