/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *  
 * 
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */
package ioio.manager;

import ioio.lib.api.IcspMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.AbstractIOIOActivity;
import ioio.manager.IOIOFileProgrammer.ProgressListener;

import java.io.File;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class ProgrammerActivity extends AbstractIOIOActivity {
	enum ProgrammerState {
		STATE_IOIO_DISCONNECTED, STATE_IOIO_CONNECTED, STATE_TARGET_CONNECTED
	}

	// private static final String TAG = "IOIOProgrammer";
	private static final int REQUEST_IMAGE_SELECT = 0;

	private static final String SELECTED_IMAGE_NAME = "SELECTED_IMAGE_NAME";
	private static final String SELECTED_IMAGE_FILE_NAME = "SELECTED_IMAGE_FILE_NAME";

	private TextView statusTextView_;
	private TextView selectedImageTextView_;
	private Button selectButton_;
	private Button eraseButton_;
	private Button programButton_;
	private File selectedImage_;
	private String selectedImageName_;
	private int targetId_;

	private ProgrammerState programmerState_;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if (savedInstanceState != null) {
			String selectedImageFileName = savedInstanceState
					.getString(SELECTED_IMAGE_FILE_NAME);
			if (selectedImageFileName != null) {
				selectedImage_ = new File(selectedImageFileName);
			}
			selectedImageName_ = savedInstanceState
					.getString(SELECTED_IMAGE_NAME);
		}
		setContentView(R.layout.programmer);
		statusTextView_ = (TextView) findViewById(R.id.programmerStatusTextView);
		selectedImageTextView_ = (TextView) findViewById(R.id.selectedImage);
		if (selectedImage_ != null) {
			selectedImageTextView_.setText(selectedImageName_);
			selectedImageTextView_.setTextColor(Color.WHITE);
		}
		selectButton_ = (Button) findViewById(R.id.selectButton);
		selectButton_.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				startActivityForResult(new Intent(
						ImageLibraryActivity.ACTION_SELECT, null,
						ProgrammerActivity.this, ImageLibraryActivity.class),
						REQUEST_IMAGE_SELECT);
			}
		});
		eraseButton_ = (Button) findViewById(R.id.eraseButton);
		programButton_ = (Button) findViewById(R.id.programButton);
		setProgrammerState(ProgrammerState.STATE_IOIO_DISCONNECTED);
	}

	private void setProgrammerState(ProgrammerState state) {
		programmerState_ = state;
		updateButtonState();
		switch (state) {
		case STATE_IOIO_DISCONNECTED:
			setStatusText("Waiting for IOIO connection...");
			break;
		case STATE_IOIO_CONNECTED:
			setStatusText("Waiting for target...");
			break;
		case STATE_TARGET_CONNECTED:
			setStatusText("Target connected: 0x"
					+ Integer.toHexString(targetId_));
			break;
		}
	}

	private void updateButtonState() {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				eraseButton_
				.setEnabled(programmerState_ == ProgrammerState.STATE_TARGET_CONNECTED);
				programButton_
				.setEnabled(programmerState_ == ProgrammerState.STATE_TARGET_CONNECTED
						&& selectedImage_ != null);
			}
		});
	}

	@Override
	protected IOIOThread createIOIOThread() {
		return new MainIOIOThread();
	}

	class MainIOIOThread extends IOIOThread implements ProgressListener {
		private int nTotalBlocks_;
		private int nBlocksDone_;
		private IcspMaster icsp_;

		@Override
		protected void setup() throws ConnectionLostException,
				InterruptedException {
			setProgrammerState(ProgrammerState.STATE_IOIO_CONNECTED);
			icsp_ = ioio_.openIcspMaster();
			// try {
			// nTotalBlocks_ = IOIOFileProgrammer
			// .countBlocks(new IOIOFileReader(getAssets().open(
			// "test.ioio")));
			// appendText("\nErasing...");
			// Scripts.chipErase(icsp);
			// appendText("\nProgramming...");
			// nBlocksDone_ = 0;
			// IOIOFileProgrammer.programIOIOFile(icsp, new IOIOFileReader(
			// getAssets().open("test.ioio")), this);
			// appendText("\nVerifying...");
			// nBlocksDone_ = 0;
			// if (IOIOFileProgrammer.verifyIOIOFile(icsp, new IOIOFileReader(
			// getAssets().open("test.ioio")), this)) {
			// appendText(" PASS");
			// } else {
			// appendText(" FAIL");
			// }
			// } catch (FormatException e) {
			// } catch (IOException e) {
			// } finally {
			// icsp.exitProgramming();
			// icsp.close();
			// }
		}

		@Override
		protected void loop() throws ConnectionLostException,
				InterruptedException {
			switch (programmerState_) {
			case STATE_IOIO_CONNECTED:
			case STATE_TARGET_CONNECTED:
				icsp_.enterProgramming();
				targetId_ = Scripts.getDeviceId(icsp_);
				if ((targetId_ & 0xFF00) == 0x4100) {
					setProgrammerState(ProgrammerState.STATE_TARGET_CONNECTED);
				} else {
					setProgrammerState(ProgrammerState.STATE_IOIO_CONNECTED);
				}
				sleep(100);
				icsp_.exitProgramming();
				break;
			}
		}

		@Override
		protected void disconnected() throws InterruptedException {
			setProgrammerState(ProgrammerState.STATE_IOIO_DISCONNECTED);
		}

		@Override
		public void blockDone() {
			++nBlocksDone_;
		}
	}

	private void setStatusText(final String text) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				statusTextView_.setText(text);
			}
		});
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.programmer_options, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle item selection
		switch (item.getItemId()) {
		case R.id.editImageLibrary:
			startActivity(new Intent(ImageLibraryActivity.ACTION_EDIT, null,
					this, ImageLibraryActivity.class));
			return true;
		default:
			return super.onOptionsItemSelected(item);
		}
	}

	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		switch (requestCode) {
		case REQUEST_IMAGE_SELECT:
			if (resultCode == RESULT_OK) {
				File file = (File) new File(data.getData().getPath());
				String bundleName = data
						.getStringExtra(ImageLibraryActivity.EXTRA_BUNDLE_NAME);
				String imageName = data
						.getStringExtra(ImageLibraryActivity.EXTRA_IMAGE_NAME);
				setSelectedImage(file, bundleName, imageName);
			}
			break;
		}
	}

	private void setSelectedImage(File file, String bundleName, String imageName) {
		selectedImage_ = file;
		selectedImageName_ = bundleName + " / " + imageName;
		selectedImageTextView_.setText(selectedImageName_);
		selectedImageTextView_.setTextColor(Color.WHITE);
		updateButtonState();
	}

	@Override
	protected void onSaveInstanceState(Bundle outState) {
		super.onSaveInstanceState(outState);
		if (selectedImage_ != null) {
			outState.putString(SELECTED_IMAGE_NAME, selectedImageName_);
			outState.putString(SELECTED_IMAGE_FILE_NAME,
					selectedImage_.getAbsolutePath());
		}
	}

}