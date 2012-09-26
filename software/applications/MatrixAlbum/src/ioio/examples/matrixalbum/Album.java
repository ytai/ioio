package ioio.examples.matrixalbum;

import ioio.lib.api.RgbLedMatrix;
import ioio.lib.api.RgbLedMatrix.Matrix;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.android.IOIOActivity;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

import android.app.AlertDialog;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageButton;

public class Album extends IOIOActivity {
	// private static final String TAG = "Album";
	
	// TODO: change this on need or add a user-selectable option.
	private static final RgbLedMatrix.Matrix KIND = Matrix.SEEEDSTUDIO_32x32_NEW;

	private int height_;
	private int width_;

	private short[] frame_ = new short[KIND.width * KIND.height];

	private byte[] BitmapBytes;
	private InputStream BitmapInputStream;

	private ConnectTimer connectTimer;

	private int deviceFound = 0;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main2);
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT); // force
																			// only
																			// portrait
																			// mode
																			// /

		connectTimer = new ConnectTimer(15000, 5000); // pop up a message if
														// it's not connected by
														// this timer
		connectTimer.start(); // this timer will pop up a message box if the
								// device is not found

		width_ = 32;
		height_ = 32;
		BitmapBytes = new byte[width_ * height_ * 2]; // 512 * 2 = 1024

		BitmapInputStream = getResources().openRawResource(R.raw.selectpic);
		loadImage();

		ImageButton button1 = (ImageButton) findViewById(R.id.imageButton1);
		ImageButton button2 = (ImageButton) findViewById(R.id.imageButton2);
		ImageButton button3 = (ImageButton) findViewById(R.id.imageButton3);
		ImageButton button4 = (ImageButton) findViewById(R.id.imageButton4);
		ImageButton button5 = (ImageButton) findViewById(R.id.imageButton5);
		ImageButton button6 = (ImageButton) findViewById(R.id.imageButton6);
		ImageButton button7 = (ImageButton) findViewById(R.id.imageButton7);
		ImageButton button8 = (ImageButton) findViewById(R.id.imageButton8);
		ImageButton button9 = (ImageButton) findViewById(R.id.imageButton9);
		ImageButton button10 = (ImageButton) findViewById(R.id.imageButton10);
		ImageButton button11 = (ImageButton) findViewById(R.id.imageButton11);
		ImageButton button12 = (ImageButton) findViewById(R.id.imageButton12);
		ImageButton button13 = (ImageButton) findViewById(R.id.imageButton13);
		ImageButton button14 = (ImageButton) findViewById(R.id.imageButton14);
		ImageButton button15 = (ImageButton) findViewById(R.id.imageButton15);
		ImageButton button16 = (ImageButton) findViewById(R.id.imageButton16);
		ImageButton button17 = (ImageButton) findViewById(R.id.imageButton17);
		ImageButton button18 = (ImageButton) findViewById(R.id.imageButton18);
		ImageButton button19 = (ImageButton) findViewById(R.id.imageButton19);
		ImageButton button20 = (ImageButton) findViewById(R.id.imageButton20);
		ImageButton button21 = (ImageButton) findViewById(R.id.imageButton21);
		ImageButton button22 = (ImageButton) findViewById(R.id.imageButton22);
		ImageButton button23 = (ImageButton) findViewById(R.id.imageButton23);
		ImageButton button24 = (ImageButton) findViewById(R.id.imageButton24);
		ImageButton button25 = (ImageButton) findViewById(R.id.imageButton25);
		ImageButton button26 = (ImageButton) findViewById(R.id.imageButton26);
		ImageButton button27 = (ImageButton) findViewById(R.id.imageButton27);
		ImageButton button28 = (ImageButton) findViewById(R.id.imageButton28);
		ImageButton button29 = (ImageButton) findViewById(R.id.imageButton29);
		ImageButton button30 = (ImageButton) findViewById(R.id.imageButton30);
		ImageButton button31 = (ImageButton) findViewById(R.id.imageButton31);

		button1.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(R.raw.apple);
				loadImage();
			}
		});

		button2.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(R.raw.balls);
				loadImage();
			}
		});

		button3.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources()
						.openRawResource(R.raw.island);
				loadImage();
			}
		});

		button4.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(
						R.raw.bluebird);
				loadImage();
			}
		});

		button5.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(
						R.raw.charater);
				loadImage();
			}
		});

		button6.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(R.raw.check);
				loadImage();
			}
		});

		button7.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources()
						.openRawResource(R.raw.clover);
				loadImage();
			}
		});

		button8.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(
						R.raw.cupcake);
				loadImage();
			}
		});

		button9.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(
						R.raw.equalizer);
				loadImage();
			}
		});

		button10.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(
						R.raw.exclamation);
				loadImage();
			}
		});

		button11.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(R.raw.faces);
				loadImage();
			}
		});

		button12.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(
						R.raw.greenguy);
				loadImage();
			}
		});

		button13.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(
						R.raw.greenmonster);
				loadImage();
			}
		});

		button14.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(R.raw.heart);
				loadImage();
			}
		});

		button15.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(
						R.raw.mushroom);
				loadImage();
			}
		});

		button16.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(R.raw.new2);
				loadImage();
			}
		});

		button17.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(R.raw.nike);
				loadImage();
			}
		});

		button18.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(R.raw.pills);
				loadImage();
			}
		});

		button19.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(
						R.raw.pinkbear);
				loadImage();
			}
		});

		button20.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources()
						.openRawResource(R.raw.popper);
				loadImage();
			}
		});

		button21.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(R.raw.puma);
				loadImage();
			}
		});

		button22.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(
						R.raw.pumpkin);
				loadImage();
			}
		});

		button23.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources()
						.openRawResource(R.raw.purple);
				loadImage();
			}
		});

		button24.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(
						R.raw.rainbox);
				loadImage();
			}
		});

		button25.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(R.raw.ruby);
				loadImage();
			}
		});

		button26.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources()
						.openRawResource(R.raw.tomato);
				loadImage();
			}
		});

		button27.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(
						R.raw.watermelon);
				loadImage();
			}
		});

		button28.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(R.raw.wine);
				loadImage();
			}
		});

		button29.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(R.raw.world);
				loadImage();
			}
		});

		button30.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(R.raw.pin);
				loadImage();
			}
		});

		button31.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				BitmapInputStream = getResources().openRawResource(
						R.raw.marilyn);
				loadImage();
			}
		});
	}

	public void loadImage() {
		try {
			int n = BitmapInputStream.read(BitmapBytes, 0, BitmapBytes.length); // reads
																				// the
																				// input
																				// stream
																				// into
																				// a
																				// byte
																				// array
			Arrays.fill(BitmapBytes, n, BitmapBytes.length, (byte) 0);
		} catch (IOException e) {
			e.printStackTrace();
		}

		int y = 0;
		for (int i = 0; i < frame_.length; i++) {
			frame_[i] = (short) (((short) BitmapBytes[y] & 0xFF) | (((short) BitmapBytes[y + 1] & 0xFF) << 8));
			y = y + 2;
		}

	}

	public class ConnectTimer extends CountDownTimer {

		public ConnectTimer(long startTime, long interval) {
			super(startTime, interval);
		}

		@Override
		public void onFinish() {
			if (deviceFound == 0) {
				showNotFound();
			}

		}

		@Override
		public void onTick(long millisUntilFinished) {
			// not used
		}
	}

	private void showNotFound() {
		AlertDialog.Builder alert = new AlertDialog.Builder(this);
		alert.setTitle("Not Found")
				.setIcon(R.drawable.icon)
				.setMessage(
						"Please ensure Bluetooth pairing has been completed prior. The Bluetooth pairing code is: 4545.")
				.setNeutralButton("OK", null).show();
	}

	class IOIOThread extends BaseIOIOLooper {
		private RgbLedMatrix matrix_;

		@Override
		protected void setup() throws ConnectionLostException {
			matrix_ = ioio_.openRgbLedMatrix(KIND);
			deviceFound = 1; // if we went here, then we are connected over
								// bluetooth or USB
			connectTimer.cancel(); // we can stop this since it was found
		}

		@Override
		public void loop() throws ConnectionLostException {

			matrix_.frame(frame_); // writes whatever is in bitmap raw 565 file
									// buffer to the RGB LCD

		}
	}

	@Override
	protected IOIOLooper createIOIOLooper() {
		return new IOIOThread();
	}

	@Override
	public void onDestroy() {

		connectTimer.cancel();
		super.onDestroy();
	}

}