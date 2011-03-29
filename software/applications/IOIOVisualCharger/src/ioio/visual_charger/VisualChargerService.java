package ioio.visual_charger;

import ioio.api.IOIOLib;
import ioio.api.PeripheralException.ConnectionLostException;
import ioio.api.PeripheralException.InvalidStateException;
import ioio.api.PeripheralInterface;
import ioio.api.PwmOutput;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.Cursor;
import android.net.Uri;
import android.os.BatteryManager;
import android.os.Binder;
import android.os.IBinder;
import android.provider.CallLog.Calls;
import android.telephony.TelephonyManager;
import android.util.Log;

/**
 * This is the service that takes care of the IOIO Visual Charger.
 *
 * This is the 7-segment we used: http://www.sparkfun.com/products/8530
 * The way to connect the wires (The permutation): (1 2 4 5 7 6 3 8)
 *
 * @author Misha Seltzer
 */
public class VisualChargerService extends Service {
	private static final String LOG_TAG = "VISUAL_CHARGER";

	private static final int[] IOIO_PWM_PINS = {3, 4, 5, 6, 7, 10, 11, 12, 13, 14};
	private IOIOLib ioio;

	private List<PwmOutput> digitPins = new ArrayList<PwmOutput>(7);
	private PwmOutput dotPin;

	private int currentDigit = 11;

	private boolean animationShouldShow = false;
	private Thread animationThread = null;

	private boolean dotShouldPump = false;
	private Thread dotPumpingThread = null;

	private static final int DOT_PUMPING_CHECK_PERIOD = 1000;
	private Timer dotPumpingNeededChecker = null;

	private static final float[][] DIGIT_ENCODINGS = new float[][] {
		{1, 1, 1, 0, 1, 1, 1},
		{0, 0, 1, 0, 0, 1, 0},
		{1, 0, 1, 1, 1, 0, 1},
		{1, 0, 1, 1, 0, 1, 1},
		{0, 1, 1, 1, 0, 1, 0},
		{1, 1, 0, 1, 0, 1, 1},
		{1, 1, 0, 1, 1, 1, 1},
		{1, 0, 1, 0, 0, 1, 0},
		{1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 0, 1, 1},
		{1, 1, 0, 1, 1, 0, 0}, // 10 represented with F letter.
		{0, 0, 0, 0, 0, 0, 0}, // 11 is empty board.
	};

	private static final int PWM_MINIMUM_TIMEOUT = 20;

	private static final int DIGIT_CHANGE_DURATION_MILLIS = 500;

	private static final int DOT_PUMPING_DURATION_MILLIS = 1000;
	private static final int DOT_PUMPING_PAUSE_DURATION_MILLIS = 10;

	private static final int ANIMATION_DURATION_MILLIS = 500;
	private static final float[][] ANIMATION_STEPS_ENCODIGNS = new float[][] {
		{1, 0, 1, 0, 0, 0, 0},
		{0, 0, 1, 0, 0, 1, 0},
		{0, 0, 0, 0, 0, 1, 1},
		{0, 0, 0, 0, 1, 0, 1},
		{0, 1, 0, 0, 1, 0, 0},
		{1, 1, 0, 0, 0, 0, 0},
	};

	private BroadcastReceiver batteryInfoReciever = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			if (intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, -1) == 0) {
				reconnectIoio();
				return;
			}
			int level = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, 0);
			int max = intent.getIntExtra(BatteryManager.EXTRA_SCALE, 100);
			int normalizedLevel = (level * 10) / max;
			if (normalizedLevel == currentDigit) {
				return;
			}
			if (animationShouldShow) {
				currentDigit = normalizedLevel;
				return;
			}
			try {
				updateDigit(normalizedLevel);
				currentDigit = normalizedLevel;
			} catch (ConnectionLostException e) {
				// The battery manager will take care of disconnection.
				return;
			} catch (InvalidStateException e) {
				Log.w(LOG_TAG, e);
			}
		}
	};

	private BroadcastReceiver phoneStateChangeReciever = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			String state = intent.getStringExtra(TelephonyManager.EXTRA_STATE);
			if (state.equals(TelephonyManager.EXTRA_STATE_RINGING)) {
				startAnimation();
			} else if (state.equals(TelephonyManager.EXTRA_STATE_IDLE)) {
				stopAnimation(true);
			}
		}
	};

	private HugeMissedCallsServiceBinder binder;

	public class HugeMissedCallsServiceBinder extends Binder {
		VisualChargerService getService() {
			return VisualChargerService.this;
		}
	}

	@Override
	public IBinder onBind(Intent intent) {
		return this.binder;
	}

	@Override
	public void onCreate() {
		super.onCreate();
		try {
			ioio = PeripheralInterface.waitForController();
		} catch (Exception e) {
			Log.e(LOG_TAG, "Couldn't get IOIO controller.", e);
			return;
		}
		connectToIoio();
	};

	private void connectToIoio() {
		Log.i(LOG_TAG, "Connecting to IOIO...");
		try {
			ioio.waitForConnect();
			for (int i = 0; i < 7; ++i) {
				digitPins.add(ioio.openPwmOutput(IOIO_PWM_PINS[i], 100));
			}
			dotPin = ioio.openPwmOutput(IOIO_PWM_PINS[7], 100);
		} catch (Exception e) {
			Log.e(LOG_TAG, "Couldn't open connection to IOIO.", e);
			return;
		}
		Log.i(LOG_TAG, "Connected.");

		registerReceiver(batteryInfoReciever, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
		registerReceiver(phoneStateChangeReciever, new IntentFilter(TelephonyManager.ACTION_PHONE_STATE_CHANGED));
		dotPumpingNeededChecker = new Timer();
		dotPumpingNeededChecker.schedule(new TimerTask() {
			@Override
			public void run() {
				if (checkNeedDotPumping()) {
					if (!dotShouldPump) {
						startDotPumping();
					}
				} else {
					stopDotPumping(true);
				}
			}
		}, 0, DOT_PUMPING_CHECK_PERIOD);
		Log.i(LOG_TAG, "Registered receivers.");
	}

	private void disconnectIoio() {
		Log.w(LOG_TAG, "Disconnecting IOIO...");
		stopAnimation(false);
		dotPumpingNeededChecker.cancel();
		stopDotPumping(false);

		unregisterReceiver(phoneStateChangeReciever);
		unregisterReceiver(batteryInfoReciever);

		ioio.disconnect();

		try {
			for (PwmOutput pin : digitPins) {
				pin.close();
			}
			dotPin.close();
		} catch (IOException e) {
			Log.e(LOG_TAG, "Couldn't close PWM pins", e);
		}
		digitPins.clear();
		dotPin = null;
		currentDigit = 11;
	}

	private void reconnectIoio() {
		disconnectIoio();
		connectToIoio();
	}

	private void updateDigit(int d) throws ConnectionLostException, InvalidStateException {
		Log.i(LOG_TAG, "Updating digit to: " + d);
		float delta = 1.f / DIGIT_CHANGE_DURATION_MILLIS;
		for (int j = 0; j <= DIGIT_CHANGE_DURATION_MILLIS; j += PWM_MINIMUM_TIMEOUT) {
			float jdelta = delta * j;
			for (int i = 0; i < digitPins.size(); ++i) {
				digitPins.get(i).setDutyCycle(DIGIT_ENCODINGS[d][i] * jdelta + DIGIT_ENCODINGS[currentDigit][i] * (1-jdelta));
			}
			try {
				Thread.sleep(PWM_MINIMUM_TIMEOUT);
			} catch (InterruptedException e) {
				Log.w(LOG_TAG, e);
			}
		}
	}

	private void startAnimation() {
		Log.i(LOG_TAG, "Playing animation...");
		animationThread = new Thread() {
			@Override
			public void run() {
				int updateDelay = ANIMATION_DURATION_MILLIS / ANIMATION_STEPS_ENCODIGNS.length;
				while (animationShouldShow) {
					for (int step = 0; step < ANIMATION_STEPS_ENCODIGNS.length; ++step) {
						try {
							for (int i = 0; i < ANIMATION_STEPS_ENCODIGNS[step].length; ++i) {
								digitPins.get(i).setDutyCycle(ANIMATION_STEPS_ENCODIGNS[step][i]);
							}
							Thread.sleep(updateDelay);
						} catch (ConnectionLostException e) {
							// The battery manager will take care of disconnection.
							return;
						} catch (InvalidStateException e) {
							Log.w(LOG_TAG, e);
						} catch (InterruptedException e) {
							Log.w(LOG_TAG, e);
						}
					}
				}
			};
		};
		animationShouldShow = true;
		animationThread.start();
	}

	private void stopAnimation(boolean restoreDigit) {
		Log.i(LOG_TAG, "Stopping animation...");
		animationShouldShow = false;
		try {
			if (animationThread != null) {
				animationThread.join();
			}
			animationThread = null;
			if (restoreDigit) {
				int restore = currentDigit;
				currentDigit = 11;
				updateDigit(restore);
				currentDigit = restore;
			}
		} catch (InterruptedException e) {
			Log.w(LOG_TAG, e);
		} catch (ConnectionLostException e) {
			// The battery manager will take care of disconnection.
			return;
		} catch (InvalidStateException e) {
			Log.w(LOG_TAG, e);
		}
	}

	private boolean checkNeedDotPumping() {
		Cursor c = getContentResolver().query(Uri.parse("content://call_log/calls"), null, null, null, null);
		if (c.moveToFirst()) {
			do {
				if ((Integer.parseInt(c.getString(c.getColumnIndex(Calls.TYPE))) == 3) &&
						(Integer.parseInt(c.getString(c.getColumnIndex(Calls.NEW))) != 0)){
					return true;
				}
			} while (c.moveToNext());
		}
		c.deactivate();

		c = getContentResolver().query(Uri.parse("content://sms/inbox"), null, "read = 0", null, null);
		boolean value = c.getCount() > 0;
		c.deactivate();
		return value;
	}

	private void startDotPumping() {
		Log.i(LOG_TAG, "Pumping the dot...");
		dotPumpingThread = new Thread() {
			@Override
			public void run() {
				float delta = (float) PWM_MINIMUM_TIMEOUT / (float) DOT_PUMPING_DURATION_MILLIS;
				float dutyCycle = 0;
				while (dotShouldPump) {
					try {
						dotPin.setDutyCycle(dutyCycle);
						dutyCycle += delta;
						if ((delta > 0) && (dutyCycle > 1)) {
							dutyCycle = 1;
							delta *= -1;
						} else if ((delta < 0) && (dutyCycle < 0)) {
							dutyCycle = 0;
							delta *= -1;
							Thread.sleep(DOT_PUMPING_PAUSE_DURATION_MILLIS);
						}
						Thread.sleep(PWM_MINIMUM_TIMEOUT);
					} catch (ConnectionLostException e) {
						// The battery manager will take care of disconnection.
						return;
					} catch (InvalidStateException e) {
						Log.e(LOG_TAG, e.getMessage());
					} catch (InterruptedException e) {
						Log.w(LOG_TAG, e);
					}
				}
			};
		};
		dotShouldPump = true;
		dotPumpingThread.start();
	}

	private void stopDotPumping(boolean shouldTurnDotOff) {
		Log.i(LOG_TAG, "Stopping dot pumping...");
		dotShouldPump = false;
		try {
			if (dotPumpingThread != null) {
				dotPumpingThread.join();
			}
			dotPumpingThread = null;
			if (shouldTurnDotOff) {
				dotPin.setDutyCycle(0);
			}
		} catch (InterruptedException e) {
			Log.w(LOG_TAG, e);
		} catch (ConnectionLostException e) {
			// The battery manager will take care of disconnection.
			return;
		} catch (InvalidStateException e) {
			Log.w(LOG_TAG, e);
		}
	}
}
