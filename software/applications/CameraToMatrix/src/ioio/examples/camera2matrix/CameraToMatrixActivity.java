package ioio.examples.camera2matrix;

import ioio.lib.api.RgbLedMatrix;
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

public class CameraToMatrixActivity extends IOIOActivity {
	private static final String TAG = "CameraToMatrixActivity";
	private Camera camera_;
	private int height_;
	private int width_;

	private short[] frame_ = new short[1024];
	private short[] rgb_;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
	}

	@Override
	protected void onStart() {
		// Start the camera preview
		camera_ = getCameraInstance();
		Parameters params = camera_.getParameters();
		params.setPreviewFormat(ImageFormat.NV21);
		getSmallestPreviewSize(params);
		params.setPreviewSize(width_, height_);
		rgb_ = new short[width_ * height_];
		params.setFlashMode(Parameters.FLASH_MODE_TORCH);
		params.setWhiteBalance(Parameters.WHITE_BALANCE_AUTO);
		camera_.setParameters(params);

		camera_.setPreviewCallback(new PreviewCallback() {
			@Override
			public void onPreviewFrame(byte[] data, Camera camera) {
				toRGB565(data, width_, height_, rgb_);
				synchronized (frame_) {
					for (int i = 0; i < 32; ++i) {
						System.arraycopy(rgb_, i * width_, frame_, i * 32, 32);
					}
					frame_.notify();
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

	class IOIOThread extends BaseIOIOLooper {
		private RgbLedMatrix matrix_;
		private int b_ = 0;

		@Override
		protected void setup() throws ConnectionLostException {
			matrix_ = ioio_.openRgbLedMatrix(RgbLedMatrix.Matrix.SEEEDSTUDIO_32x32);
		}

		@Override
		public void loop() throws ConnectionLostException {
//			synchronized (frame_) {
				try {
//					frame_.wait();
					for (int i = 0; i < 1024; ++i) {
						frame_[i] = (short) ((i << 6) | (b_ & 0x3f));
					}
					matrix_.frame(frame_);
					b_ += 4;
					Thread.sleep(50);
				} catch (InterruptedException e) {
				}
//			}
		}
	}

	@Override
	protected IOIOLooper createIOIOLooper() {
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

	// From: http://lanedetectionandroid.googlecode.com/svn-history/r8/trunk/tests/TestJniCall/src/org/siprop/opencv/Preview.java
	static public void toRGB565(byte[] yuvs, int width, int height, short[] rgbs) {
	    //the end of the luminance data
	    final int lumEnd = width * height;
	    //points to the next luminance value pair
	    int lumPtr = 0;
	    //points to the next chromiance value pair
	    int chrPtr = lumEnd;
	    //points to the next byte output pair of RGB565 value
	    int outPtr = 0;
	    //the end of the current luminance scanline
	    int lineEnd = width;

	    while (true) {

	        //skip back to the start of the chromiance values when necessary
	        if (lumPtr == lineEnd) {
	            if (lumPtr == lumEnd) break; //we've reached the end
	            //division here is a bit expensive, but's only done once per scanline
	            chrPtr = lumEnd + ((lumPtr  >> 1) / width) * width;
	            lineEnd += width;
	        }

	        //read the luminance and chromiance values
	        final int Y1 = yuvs[lumPtr++] & 0xff; 
	        final int Y2 = yuvs[lumPtr++] & 0xff; 
	        final int Cr = (yuvs[chrPtr++] & 0xff) - 128; 
	        final int Cb = (yuvs[chrPtr++] & 0xff) - 128;
	        int R, G, B;

	        //generate first RGB components
	        B = Y1 + ((454 * Cb) >> 8);
	        if(B < 0) B = 0; else if(B > 255) B = 255; 
	        G = Y1 - ((88 * Cb + 183 * Cr) >> 8); 
	        if(G < 0) G = 0; else if(G > 255) G = 255; 
	        R = Y1 + ((359 * Cr) >> 8); 
	        if(R < 0) R = 0; else if(R > 255) R = 255; 
	        rgbs[outPtr++]  = (short) ((R >> 3) << 11 | (G >> 2) << 5 | (B >> 3));

	        //generate second RGB components
	        B = Y2 + ((454 * Cb) >> 8);
	        if(B < 0) B = 0; else if(B > 255) B = 255; 
	        G = Y2 - ((88 * Cb + 183 * Cr) >> 8); 
	        if(G < 0) G = 0; else if(G > 255) G = 255; 
	        R = Y2 + ((359 * Cr) >> 8); 
	        if(R < 0) R = 0; else if(R > 255) R = 255; 
	        rgbs[outPtr++]  = (short) ((R >> 3) << 11 | (G >> 2) << 5 | (B >> 3));
	    }
	}
}