package ioio.examples.holiday;

import ioio.lib.api.SpiMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.android.IOIOActivity;

import java.util.List;

import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.Size;
import android.os.Bundle;
import android.util.Log;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;

/**
 * A IOIO holiday application!
 * Blink an LED strip by using random color from the Android camera.
 * IOIO pin 3 -> clock
 * IOIO pin 4 -> data
 * Built for an LED strip such as this one:
 * http://www.sparkfun.com/products/10312
 * 
 * That's just a sample application - use your best creativity to modify it!
 *
 * @author ytai
 */
public class HolidayIOIOActivity extends IOIOActivity {
	private static final String TAG = "HolidayIOIO";
	private Camera camera_;
	RGB[] frame_ = null;
	private int height_;
	private int width_;
	double frequency_;
	double fadeRate_;
	private RGB tempRGB_ = new RGB();
	byte[] buffer1_ = new byte[48];
	byte[] buffer2_ = new byte[48];

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		// The first seekbar controls the blink frequency.
		final SeekBar freqSeekBar = (SeekBar) findViewById(R.id.frequencySeekBar);
		updateFrequency(freqSeekBar, freqSeekBar.getProgress());
		freqSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
			}

			@Override
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				updateFrequency(seekBar, progress);
			}
		});

		// The second seekbar controls the fade time.
		final SeekBar fadeSeekBar = (SeekBar) findViewById(R.id.fadeSeekBar);
		updateFade(fadeSeekBar, fadeSeekBar.getProgress());
		fadeSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
			}

			@Override
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				updateFade(seekBar, progress);
			}
		});

	}

	@Override
	protected void onStart() {
		// Start the camera oreview
		camera_ = getCameraInstance();
		Parameters params = camera_.getParameters();
		params.setPreviewFormat(ImageFormat.NV21);
		getSmallestPreviewSize(params);
		params.setPreviewSize(width_, height_);
		//params.setFlashMode(Parameters.FLASH_MODE_TORCH);
		params.setWhiteBalance(Parameters.WHITE_BALANCE_AUTO);
		camera_.setParameters(params);
		frame_ = new RGB[width_ * height_];
		for (int i = 0; i < frame_.length; ++i) {
			frame_[i] = new RGB();
		}

		camera_.setPreviewCallback(new PreviewCallback() {
			@Override
			public void onPreviewFrame(byte[] data, Camera camera) {
				synchronized (frame_) {
					decodeYUV420SP(frame_, data, width_, height_);
				}
			}
		});
		camera_.startPreview();
		super.onStart();
	}

	/** Chooses the smallest supported preview size. */
	private void getSmallestPreviewSize(Parameters params) {
		List<Size> supportedPreviewSizes = params.getSupportedPreviewSizes();
		Size minSize = null;
		for (Size s : supportedPreviewSizes) {
			if (minSize == null || s.width < minSize.width) {
				minSize = s;
			}
		}
		height_ = minSize.height;
		width_ = minSize.width;
	}

	@Override
	protected void onStop() {
		super.onStop();
		// Stop the camera preview.
		camera_.stopPreview();
		camera_.setPreviewCallback(null);
		camera_.release();
	}

	/** An RGB triplet. */
	private static class RGB {
		byte r;
		byte g;
		byte b;

		RGB() {
			clear();
		}

		public void clear() {
			r = g = b = 0;
		}
	}

	class IOIOThread extends BaseIOIOLooper {
		private SpiMaster spi_;

		@Override
		protected void setup() throws ConnectionLostException {
			spi_ = ioio_.openSpiMaster(5, 4, 3, 6, SpiMaster.Rate.RATE_50K);
		}

		@Override
		public void loop() throws ConnectionLostException {
			// We have 32 LEDs. Each one of them gets lit with probability
			// frequency_. If lit, we pick a random pixel from the current preview
			// frame and use its color. Otherwise, we set the LED to black and
			// setLed() will take care of gradual fading.
			for (int i = 0; i < 32; i++) {
				tempRGB_.clear();
				if (Math.random() < frequency_) {
					getRandomColor(tempRGB_);
				}
				setLed(i, tempRGB_);
			}
			// Since SPI messages are limited to 64 bytes, and we need to send
			// 96 bytes, we divide the message into two chunks of 48. We assume
			// that the SPI clock is slow enough (50K) so that the second half
			// will finish being sent to the IOIO before the first half
			// finished transmission.
			try {
				spi_.writeReadAsync(0, buffer1_, buffer1_.length,
						buffer1_.length, null, 0);
				spi_.writeRead(buffer2_, buffer2_.length, buffer2_.length,
						null, 0);
				Thread.sleep(50);
			} catch (InterruptedException e1) {
			}
		}

		/** Choose a random pixel from the current preview frame. */
		private void getRandomColor(RGB rgb) {
			synchronized (frame_) {
				final int i = (int) (Math.random() * frame_.length);
				rgb.r = frame_[i].r;
				rgb.g = frame_[i].g;
				rgb.b = frame_[i].b;
			}
		}

		/**
		 * Set an LED to a certain color.
		 * If black is applied, the LED will fade out.
		 */
		private void setLed(int num, RGB rgb) {
			// Find the right buffer to write to (first or second half).
			byte[] buffer;
			if (num >= 16) {
				buffer = buffer2_;
				num -= 16;
			} else {
				buffer = buffer1_;
			}
			num *= 3;
			if (rgb.r == 0 && rgb.g == 0 && rgb.b == 0) {
				fadeOut(buffer, num++);
				fadeOut(buffer, num++);
				fadeOut(buffer, num++);
			} else {
				// Poor-man's white balanace :)
				buffer[num++] = fixColor(rgb.r, 0.9);
				buffer[num++] = rgb.g;
				buffer[num++] = fixColor(rgb.b, 0.5);
			}
		}

		/** Attenuates a brightness level. */
		private byte fixColor(byte color, double attenuation) {
			double d = (double) ((int) color & 0xFF) / 256;
			d *= attenuation;
			return (byte) (d * 256);
		}

		private void fadeOut(byte[] buffer, int num) {
			final int value = (int) buffer[num] & 0xFF;
			buffer[num] = (byte) (value * fadeRate_);
		}

	}

	@Override
	protected IOIOLooper createIOIOLooper() {
		return new IOIOThread();
	}

	/** Read the progress bar value and scale to [0-1]. */
	private void updateFrequency(SeekBar seekBar, int progress) {
		frequency_ = Math.pow((double) progress / seekBar.getMax(), 2.0);
	}

	/** Read the progress bar value and scale to [0-1]. */
	private void updateFade(SeekBar seekBar, int progress) {
		fadeRate_ = Math.pow((double) progress / seekBar.getMax(), 2.0);
	}

	private static Camera getCameraInstance() {
		try {
			return Camera.open();
		} catch (Exception e) {
			Log.e(TAG, "Failed to open camera.", e);
		}
		return null;
	}

	/**
	 * Transform frame from YUV420 to RGB.
	 * Based on code from http://code.google.com/p/ketai/
	 * THANKS!
	 */
	private static void decodeYUV420SP(RGB[] rgb, byte[] yuvs, int width,
			int height) {
		final int lumEnd = width * height;
		int lumPtr = 0;
		int chrPtr = lumEnd;
		int outPtr = 0;
		int lineEnd = width;

		while (true) {
			if (lumPtr == lineEnd) {
				if (lumPtr == lumEnd)
					break; // we've reached the end
				chrPtr = lumEnd + ((lumPtr >> 1) / width) * width;
				lineEnd += width;
			}

			final int Y1 = yuvs[lumPtr++] & 0xff;
			final int Y2 = yuvs[lumPtr++] & 0xff;
			final int Cr = (yuvs[chrPtr++] & 0xff) - 128;
			final int Cb = (yuvs[chrPtr++] & 0xff) - 128;
			int R, G, B;

			// generate first RGB components
			B = Y1 + ((454 * Cb) >> 8);
			if (B < 0)
				B = 0;
			else if (B > 255)
				B = 255;
			G = Y1 - ((88 * Cb + 183 * Cr) >> 8);
			if (G < 0)
				G = 0;
			else if (G > 255)
				G = 255;
			R = Y1 + ((359 * Cr) >> 8);
			if (R < 0)
				R = 0;
			else if (R > 255)
				R = 255;
			rgb[outPtr].r = (byte) R;
			rgb[outPtr].g = (byte) G;
			rgb[outPtr++].b = (byte) B;

			// generate second RGB components
			B = Y2 + ((454 * Cb) >> 8);
			if (B < 0)
				B = 0;
			else if (B > 255)
				B = 255;
			G = Y2 - ((88 * Cb + 183 * Cr) >> 8);
			if (G < 0)
				G = 0;
			else if (G > 255)
				G = 255;
			R = Y2 + ((359 * Cr) >> 8);
			if (R < 0)
				R = 0;
			else if (R > 255)
				R = 255;
			rgb[outPtr].r = (byte) R;
			rgb[outPtr].g = (byte) G;
			rgb[outPtr++].b = (byte) B;
		}
	}
}