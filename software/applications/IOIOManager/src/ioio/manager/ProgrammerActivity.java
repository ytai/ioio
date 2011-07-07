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
import ioio.manager.IOIOFileReader.FormatException;

import java.io.File;
import java.io.IOException;

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
	// private static final String TAG = "IOIOProgrammer";
	private static final int REQUEST_IMAGE_SELECT = 0;

	private static final String SELECTED_IMAGE_NAME = "SELECTED_IMAGE_NAME";
	private static final String SELECTED_IMAGE_FILE_NAME = "SELECTED_IMAGE_FILE_NAME";

	private TextView statusTextView_;
	private TextView selectedImageTextView_;
	private Button selectButton_;
	private File selectedImage_;
	private String selectedImageName_;

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
	}

	@Override
	protected IOIOThread createIOIOThread() {
		return new MainIOIOThread();
	}

	class MainIOIOThread extends IOIOThread implements ProgressListener {
		private int nTotalBlocks_;
		private int nBlocksDone_;

		@Override
		protected void setup() throws ConnectionLostException,
				InterruptedException {
			IcspMaster icsp = ioio_.openIcspMaster();
			setText("Waiting for slave...");
			while (true) {
				icsp.enterProgramming();
				int id = Scripts.getDeviceId(icsp);
				if ((id & 0xFF00) == 0x4100) {
					setText("\nConnected: 0x" + Integer.toHexString(id));
					break;
				}
				sleep(100);
				icsp.exitProgramming();
			}
			try {
				nTotalBlocks_ = IOIOFileProgrammer
						.countBlocks(new IOIOFileReader(getAssets().open(
								"test.ioio")));
				appendText("\nErasing...");
				Scripts.chipErase(icsp);
				appendText("\nProgramming...");
				nBlocksDone_ = 0;
				IOIOFileProgrammer.programIOIOFile(icsp, new IOIOFileReader(
						getAssets().open("test.ioio")), this);
				appendText("\nVerifying...");
				nBlocksDone_ = 0;
				if (IOIOFileProgrammer.verifyIOIOFile(icsp, new IOIOFileReader(
						getAssets().open("test.ioio")), this)) {
					appendText(" PASS");
				} else {
					appendText(" FAIL");
				}
			} catch (FormatException e) {
			} catch (IOException e) {
			} finally {
				icsp.exitProgramming();
				icsp.close();
			}
		}

		@Override
		public void blockDone() {
			++nBlocksDone_;
			appendText(" "
					+ Integer.toString(100 * nBlocksDone_ / nTotalBlocks_)
					+ "%");
		}
	}

	private void setText(final String text) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				statusTextView_.setText(text);
			}
		});
	}

	private void appendText(final String text) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				statusTextView_.append(text);
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