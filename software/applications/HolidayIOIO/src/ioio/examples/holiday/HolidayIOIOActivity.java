package ioio.examples.holiday;

import ioio.lib.api.SpiMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.AbstractIOIOActivity;

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

public class HolidayIOIOActivity extends AbstractIOIOActivity {
	private static final String TAG = "HolidayIOIO";
	private Camera camera_;
	byte[][] frameBuffers_ = new byte[2][];
	RGB[] frame_ = null;
	private int height_;
	private int width_;
	double frequency_;
	double fadeSpeed_;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		((SeekBar) findViewById(R.id.frequencySeekBar)).setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
			}
			
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
			}
			
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				frequency_ = Math.pow((double) progress / seekBar.getMax(), 2.0);
			}
		});
		
		((SeekBar) findViewById(R.id.fadeSeekBar)).setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
			}
			
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
			}
			
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				fadeSpeed_ = Math.pow((double) progress / seekBar.getMax(), 2.0);
			}
		});
	}

	@Override
	protected void onStart() {
		camera_ = getCameraInstance();
		Parameters params = camera_.getParameters();
		params.setPreviewFormat(ImageFormat.NV21);
		getSmallestPreviewSize(params);
		params.setPreviewSize(width_, height_);
		// params.setFlashMode(Parameters.FLASH_MODE_TORCH);
		// params.setWhiteBalance(Parameters.WHITE_BALANCE_AUTO);
		camera_.setParameters(params);
		final Size previewSize = camera_.getParameters().getPreviewSize();
		final int bytePerPixel = 2;
		final int bufferSize = previewSize.height * previewSize.width
				* bytePerPixel;
		frameBuffers_[0] = new byte[bufferSize];
		frameBuffers_[1] = new byte[bufferSize];
		frame_ = new RGB[width_ * height_];
		for (int i = 0; i < frame_.length; ++i) {
			frame_[i] = new RGB(0, 0, 0);
		}

		camera_.setPreviewCallback(new PreviewCallback() {
			@Override
			public void onPreviewFrame(byte[] data, Camera camera) {
				synchronized(frame_) {
					decodeYUV420SP(frame_, data, width_, height_);
				}
			}
		});
		camera_.startPreview();
		super.onStart();
	}

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
		camera_.stopPreview();
		camera_.setPreviewCallback(null);
		camera_.release();
	}

	private static class RGB {
		double r;
		double g;
		double b;

		RGB(double rr, double gg, double bb) {
			r = rr;
			g = gg;
			b = bb;
		}

		public void clear() {
			r = g = b = 0;
		}
	}

	class IOIOThread extends AbstractIOIOActivity.IOIOThread {
		private SpiMaster spi_;
		private RGB tempRGB_ = new RGB(0, 0, 0);
		byte[] buffer1_ = new byte[48];
		byte[] buffer2_ = new byte[48];

		@Override
		protected void setup() throws ConnectionLostException {
			spi_ = ioio_.openSpiMaster(5, 4, 3, 6, SpiMaster.Rate.RATE_250K);
		}

		@Override
		protected void loop() throws ConnectionLostException {
			for (int i = 0; i < 32; i++) {
				tempRGB_.clear();
				if (Math.random() < frequency_) {
					getRandomColor(tempRGB_);
				}
				setLed(i, tempRGB_);
			}
			try {
				spi_.writeReadAsync(0, buffer1_, buffer1_.length,
						buffer1_.length, null, 0);
				spi_.writeRead(buffer2_, buffer2_.length, buffer2_.length,
						null, 0);
				sleep(50);
			} catch (InterruptedException e1) {
			}
		}

		private void getRandomColor(RGB rgb) {
			rgb.clear();
			synchronized (frame_) {
				final int i = (int) (Math.random() * frame_.length);
				rgb.r = frame_[i].r;
				rgb.g = frame_[i].g;
				rgb.b = frame_[i].b;
			}
		}

		private void setLed(int num, RGB rgb) {
			byte[] buffer;
			if (num >= 16) {
				buffer = buffer2_;
				num -= 16;
			} else {
				buffer = buffer1_;
			}
			num *= 3;
			if (rgb.r == 0 && rgb.g == 0 && rgb.b == 0) {
				fade(buffer, num++);
				fade(buffer, num++);
				fade(buffer, num++);
			} else {
				setLedComponent(buffer, num++, rgb.r);
				setLedComponent(buffer, num++, rgb.g);
				setLedComponent(buffer, num++, rgb.b);
			}
		}

		private void setLedComponent(byte[] buffer, int num, double value) {
			value = value * value;
			buffer[num] = (byte) (value * 256);
		}

		private void fade(byte[] buffer, int num) {
			double value = ((double) buffer[num]) / 256;
			value = Math.sqrt(value);
			setLedComponent(buffer, num, value * fadeSpeed_);
		}

	}

	@Override
	protected AbstractIOIOActivity.IOIOThread createIOIOThread() {
		return new IOIOThread();
	}

	private static Camera getCameraInstance() {
		try {
			return Camera.open();
		} catch (Exception e) {
			Log.e(TAG, "Failed to open camera.", e);
		}
		return null;
	}

	private static void decodeYUV420SP(RGB[] rgb, byte[] yuv420sp, int width, int height) {
		final int frameSize = width * height;
		for (int j = 0, yp = 0; j < height; j++) {
			int uvp = frameSize + (j >> 1) * width, u = 0, v = 0;
			for (int i = 0; i < width; i++, yp++) {
				int y = (0xff & ((int) yuv420sp[yp])) - 16;
				if (y < 0)
					y = 0;
				if ((i & 1) == 0) {
					v = (0xff & yuv420sp[uvp++]) - 128;
					u = (0xff & yuv420sp[uvp++]) - 128;
				}

				int y1192 = 1192 * y;
				int r = (y1192 + 1634 * v);
				int g = (y1192 - 833 * v - 400 * u);
				int b = (y1192 + 2066 * u);

				if (r < 0)
					r = 0;
				else if (r > 262143)
					r = 262143;
				if (g < 0)
					g = 0;
				else if (g > 262143)
					g = 262143;
				if (b < 0)
					b = 0;
				else if (b > 262143)
					b = 262143;

				rgb[yp].r = ((double) r) / (1 << 18);
				rgb[yp].g = ((double) g) / (1 << 18);
				rgb[yp].b = ((double) b) / (1 << 18);
			}
		}
	}
}