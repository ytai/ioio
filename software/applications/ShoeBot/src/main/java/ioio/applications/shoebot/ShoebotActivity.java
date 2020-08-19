package ioio.applications.shoebot;

import ioio.applications.shoebot.TcpServer.LineListener;
import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.Sequencer;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.android.IOIOActivity;

import java.io.IOException;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.PowerManager;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.TextView;
import android.widget.ToggleButton;

public class ShoebotActivity extends IOIOActivity implements
		SensorEventListener, LineListener {
	public static final float STEPS_FREQ = 62500;

	private Sequencer.ChannelConfig[] cfg_ = {
			new Sequencer.ChannelConfigFmSpeed(Sequencer.Clock.CLK_62K5, 2, new DigitalOutput.Spec(
					3)),
			new Sequencer.ChannelConfigBinary(false, false, new DigitalOutput.Spec(2)),
			new Sequencer.ChannelConfigFmSpeed(Sequencer.Clock.CLK_62K5, 2, new DigitalOutput.Spec(
					14)),
			new Sequencer.ChannelConfigBinary(false, false, new DigitalOutput.Spec(13)) };

	private Sequencer.ChannelCueFmSpeed leftSteps_ = new Sequencer.ChannelCueFmSpeed();
	private Sequencer.ChannelCueFmSpeed rightSteps_ = new Sequencer.ChannelCueFmSpeed();
	private Sequencer.ChannelCueBinary leftDir_ = new Sequencer.ChannelCueBinary();
	private Sequencer.ChannelCueBinary rightDir_ = new Sequencer.ChannelCueBinary();
	private Sequencer.ChannelCue[] cue_ = {
			leftSteps_, leftDir_, rightSteps_, rightDir_
	};

	private ToggleButton enableButton_;
	private Button setButton_;
	private Button driveButton_;
	private TextView pTextView_;
	private TextView iTextView_;
	private TextView dTextView_;
	private TextView tTextView_;
	private TextView eTextView_;
	private TextView sTextView_;
	private TcpServer tcpServer_;
	PowerManager.WakeLock wakeLock_;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
		wakeLock_ = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "IOIO:ShoeBot");

		enableButton_ = (ToggleButton) findViewById(R.id.enableButton);
		setButton_ = (Button) findViewById(R.id.setButton);
		driveButton_ = (Button) findViewById(R.id.driveButton);

		setButton_.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				targetOrientation_ = orientation_;
				errorInt_ = 0;
			}
		});

		enableButton_.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				errorInt_ = 0;
			}
		});

		SensorManager sm = (SensorManager) getSystemService(SENSOR_SERVICE);
		orientationSensor_ = sm.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		gyroSensor_ = sm.getDefaultSensor(Sensor.TYPE_GYROSCOPE);

		gyroTimestamp_ = 0;
		errorInt_ = 0;

		sm.registerListener(this, orientationSensor_,
				orientationSensor_.getMinDelay());
		sm.registerListener(this, gyroSensor_, SensorManager.SENSOR_DELAY_GAME);

		pTextView_ = (TextView) findViewById(R.id.p);
		iTextView_ = (TextView) findViewById(R.id.i);
		dTextView_ = (TextView) findViewById(R.id.d);
		tTextView_ = (TextView) findViewById(R.id.t);
		eTextView_ = (TextView) findViewById(R.id.e);
		sTextView_ = (TextView) findViewById(R.id.s);

		tcpServer_ = new TcpServer(4646, this);
	}

	@Override
	protected void onStart() {
		super.onStart();
		 wakeLock_.acquire();
	}

	@Override
	protected void onStop() {
		wakeLock_.release();
		super.onStop();
	}

	@Override
	protected void onDestroy() {
		tcpServer_.abort();
		super.onDestroy();
	}

	@Override
	public void onLine(String line) {
		if (line.length() > 0) {
			float f = 0;
			try {
				f = Float.valueOf(line.substring(1));
			} catch (NumberFormatException e) {
			}
			switch (line.charAt(0)) {
			case 'p':
				p_ = f;
				break;
			case 'i':
				i_ = f;
				break;
			case 'd':
				d_ = f;
				break;

			case 's':
				s_ = f;
				break;

			case 't':
				targetOrientation_ = orientation_;
				errorInt_ = 0;
				break;

			case 'r':
				rotation_ = f;
				break;

			case 'o':
				runOnUiThread(new Runnable() {
					@Override
					public void run() {
						enableButton_.toggle();
					}
				});
				break;
			}
		}
		try {
			tcpServer_.write("P: " + p_ + "\nI: " + i_ + "\nD: " + d_ + "\nS: "
					+ s_ + "\nT: " + targetOrientation_ + "\nE: " + error_
					+ "\n");
		} catch (IOException e) {
		}
	}

	private void updateDisplay() {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				pTextView_.setText("P: " + p_);
				iTextView_.setText("I: " + i_);
				dTextView_.setText("D: " + d_);
				tTextView_.setText("T: " + targetOrientation_);
				eTextView_.setText("E: " + error_);
				sTextView_.setText("S: " + s_);
			}
		});
	}

	private class Stepper {
		private final Sequencer.ChannelCueBinary dir_;
		private final Sequencer.ChannelCueFmSpeed stp_;
		private final DigitalOutput slp_;

		public Stepper(IOIO ioio, int startPin, Sequencer.ChannelCueFmSpeed step,
				Sequencer.ChannelCueBinary dir) throws ConnectionLostException {
			dir_ = dir;
			stp_ = step;
			stp_.period = 0;
			slp_ = ioio.openDigitalOutput(startPin + 2, false);
			ioio.openDigitalOutput(startPin + 3, true); // rst
			ioio.openDigitalOutput(startPin + 4, true); // ms3
			ioio.openDigitalOutput(startPin + 5, true); // ms2
			ioio.openDigitalOutput(startPin + 6, true); // ms1
			ioio.openDigitalOutput(startPin + 7, false); // en
		}

		public void setEnable(boolean en) throws ConnectionLostException {
			slp_.write(en);
		}

		public void setSpeed(float speed) throws ConnectionLostException {
			dir_.value = (speed > 0);
			speed = Math.abs(speed) * 10000;
			if (speed < 10) speed = 10;
			if (speed > STEPS_FREQ / 3) speed = STEPS_FREQ / 3;
			stp_.period = Math.round(STEPS_FREQ / speed);
		}
	}

	class BalancerLooper extends BaseIOIOLooper {
		private DigitalOutput led_;
		private final Stepper[] steppers_ = new Stepper[2];
		private static final int SLEEP_MS = 2;
		private int loopCount_ = 0;

		private Sequencer sequencer_;

		@Override
		public void setup() throws ConnectionLostException {
			led_ = ioio_.openDigitalOutput(IOIO.LED_PIN, true);
			steppers_[0] = new Stepper(ioio_, 2, leftSteps_, leftDir_);
			steppers_[1] = new Stepper(ioio_, 13, rightSteps_, rightDir_);
			errorInt_ = 0;
			targetOrientation_ = 0.147f;
			sequencer_ = ioio_.openSequencer(cfg_);
			enableUi(true);
		}

		@Override
		public void loop() throws ConnectionLostException {
			try {
				final boolean checked = enableButton_.isChecked();
				led_.write(!checked);
				steppers_[0].setEnable(checked);
				steppers_[1].setEnable(checked);

				float speed = 0;
				if (checked) {
					speed = p_ * error_ + i_ * errorInt_ + d_ * errorDeriv_;
					if (Math.abs(speed) > 0.5) {
						// Emergency stop
						speed = 0;
						runOnUiThread(new Runnable() {
							@Override
							public void run() {
								enableButton_.setChecked(false);
							}
						});
					}
					targetOrientation_-= speed * s_;
				}
				steppers_[0].setSpeed(-speed + rotation_);
				steppers_[1].setSpeed(speed + rotation_);
				sequencer_.manualStart(cue_);
				if (loopCount_++ == 50) {
					updateDisplay();
					loopCount_ = 0;
				}
				Thread.sleep(SLEEP_MS);
			} catch (InterruptedException e) {
				ioio_.disconnect();
			}
		}

		@Override
		public void disconnected() {
			enableUi(false);
		}

		private void enableUi(final boolean en) {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					enableButton_.setEnabled(en);
					setButton_.setEnabled(en);
				}
			});
		}
	}

	class RemoteLooper extends BaseIOIOLooper {
		private DigitalOutput led_;
		private final Stepper[] steppers_ = new Stepper[2];
		private Sequencer sequencer_;

		@Override
		public void setup() throws ConnectionLostException {
			try {
				led_ = ioio_.openDigitalOutput(IOIO.LED_PIN, true);
				steppers_[0] = new Stepper(ioio_, 2, leftSteps_, leftDir_);
				steppers_[1] = new Stepper(ioio_, 13, rightSteps_, rightDir_);
				sequencer_ = ioio_.openSequencer(cfg_);
				enableUi(true);
			} catch (ConnectionLostException e) {
				enableUi(false);
				throw e;
			}
		}

		@Override
		public void loop() throws ConnectionLostException, InterruptedException {
			float speedX = 0, speedY = 0;
			led_.write(!driveButton_.isPressed());
			if (driveButton_.isPressed()) {
				speedX = crop((float) (orientationX_ / Math.PI), -1 ,1);
				speedY = crop((float) (orientation_ * 2 / Math.PI - 0.5), -1, 1);;
				steppers_[0].setEnable(true);
				steppers_[1].setEnable(true);
				steppers_[0].setSpeed(speedY + speedX);
				steppers_[1].setSpeed(-speedY + speedX);
				sequencer_.manualStart(cue_);
			} else {
				steppers_[0].setEnable(false);
				steppers_[1].setEnable(false);
				sequencer_.manualStop();
			}

			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					((TextView) findViewById(R.id.p)).setText("X: "
							+ orientationX_);
					((TextView) findViewById(R.id.i)).setText("Y: "
							+ orientation_);
					((TextView) findViewById(R.id.d)).setText("dY: "
							+ errorDeriv_);
				}
			});

			Thread.sleep(100);
		}

		@Override
		public void disconnected() {
			enableUi(false);
		}

		private void enableUi(final boolean en) {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					driveButton_.setEnabled(en);
				}
			});
		}
	}

	@Override
	public IOIOLooper createIOIOLooper(String connectionType, Object extra) {
		if (connectionType
				.equals("ioio.lib.android.accessory.AccessoryConnectionBootstrap.Connection")
				|| connectionType.equals("ioio.lib.impl.SocketIOIOConnection")) {
			return new BalancerLooper();
		}
		if (connectionType
				.equals("ioio.lib.android.bluetooth.BluetoothIOIOConnection")) {
			return new RemoteLooper();
		}
		return null;
	}

	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
	}

	private long gyroTimestamp_;
	private float targetOrientation_;
	private float orientation_ = 0;
	private float orientationX_ = 0;
	private float error_ = 0; // orientation - target
	private float errorDeriv_;
	private float errorInt_;
	private Sensor orientationSensor_;
	private Sensor gyroSensor_;
	private float p_ = 2.0F;
	private float i_ = 40.0f;
	private float d_ = 0.02f;
	private float s_ = 0.001f;
	private float rotation_ = 0;

	@Override
	public void onSensorChanged(SensorEvent event) {
		if (event.sensor == gyroSensor_) {
			if (gyroTimestamp_ != 0) {
				long dt = event.timestamp - gyroTimestamp_;
				errorDeriv_ = -event.values[0];
				orientation_ += errorDeriv_ * dt * 1e-9;
				error_ = orientation_ - targetOrientation_;
				errorInt_ = (float) ((error_ * dt * 1e-9) + 0.999 * errorInt_);
			}
			gyroTimestamp_ = event.timestamp;
		} else if (event.sensor == orientationSensor_) {
			float current = (float) (Math.atan2(event.values[2],
					event.values[1]));
			orientation_ = 0.99f * orientation_ + 0.01f * current;
			orientation_ = normAngle(orientation_);
			error_ = orientation_ - targetOrientation_;

			orientationX_ = normAngle((float) (Math.atan2(-event.values[0],
					event.values[2])));
		}
	}

	private static float normAngle(float angle) {
		while (angle < -Math.PI)
			angle += 2 * Math.PI;
		while (angle > Math.PI)
			angle -= 2 * Math.PI;
		return angle;
	}

	private static float crop(float value, float min, float max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

}