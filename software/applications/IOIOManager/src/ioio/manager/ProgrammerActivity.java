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

import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnCancelListener;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class ProgrammerActivity extends AbstractIOIOActivity {
	enum ProgrammerState {
		STATE_IOIO_DISCONNECTED, STATE_IOIO_CONNECTED, STATE_TARGET_CONNECTED, STATE_UNKOWN_TARGET_CONNECTED, STATE_ERASE_START, STATE_PROGRAM_START, STATE_PROGRAM_IN_PROGRESS, STATE_ERASE_IN_PROGRESS, STATE_VERIFY_IN_PROGRESS
	}

	private static final String TAG = "ProgrammerActivity";
	private static final int REQUEST_IMAGE_SELECT = 0;
	private static final String SELECTED_IMAGE_NAME = "SELECTED_IMAGE_NAME";
	private static final String SELECTED_IMAGE_FILE_NAME = "SELECTED_IMAGE_FILE_NAME";
	private static final int PROGRESS_DIALOG = 0;

	private TextView statusTextView_;
	private TextView selectedImageTextView_;
	private Button selectButton_;
	private Button eraseButton_;
	private Button programButton_;
	private File selectedImage_;
	private String selectedImageName_;
	private int targetId_;
	private boolean cancel_ = false;

	private ProgrammerState programmerState_;
	private ProgrammerState desiredState_;
	private ProgressDialog progressDialog_;

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
		desiredState_ = ProgrammerState.STATE_IOIO_DISCONNECTED;
		prepareGui();
		setProgrammerState(ProgrammerState.STATE_IOIO_DISCONNECTED);
	}

	private void prepareGui() {
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
		eraseButton_.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				desiredState_ = ProgrammerState.STATE_ERASE_START;
			}
		});
		programButton_ = (Button) findViewById(R.id.programButton);
		programButton_.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				desiredState_ = ProgrammerState.STATE_PROGRAM_START;
			}
		});
	}

	private synchronized void setProgrammerState(final ProgrammerState state) {
		Log.e(TAG, "state is: " + state);
		programmerState_ = state;

		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				updateButtonState();
				switch (state) {
				case STATE_IOIO_DISCONNECTED:
					setStatusText(R.string.waiting_ioio_conn);
					break;

				case STATE_IOIO_CONNECTED:
					setStatusText(R.string.waiting_target);
					break;

				case STATE_TARGET_CONNECTED:
					setStatusText(getString(R.string.target_connected) + "0x"
							+ Integer.toHexString(targetId_));
					break;

				case STATE_UNKOWN_TARGET_CONNECTED:
					setStatusText(getString(R.string.unknown_target_connected)
							+ "0x" + Integer.toHexString(targetId_));
					break;

				case STATE_PROGRAM_START:
				case STATE_ERASE_START:
					showDialog(PROGRESS_DIALOG);
					break;
				}
			}
		});
	}

	@Override
	protected Dialog onCreateDialog(int id) {
		if (id == PROGRESS_DIALOG) {
			progressDialog_ = new ProgressDialog(this);
			progressDialog_.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
			progressDialog_.setTitle(R.string.programming_in_progress);
			progressDialog_.setMessage("");
			progressDialog_.setOnCancelListener(new OnCancelListener() {
				@Override
				public void onCancel(DialogInterface dialog) {
					cancel_ = true;
				}
			});
			return progressDialog_;
		} else {
			return super.onCreateDialog(id);
		}
	}

	@Override
	protected void onPrepareDialog(int id, Dialog dialog) {
		switch (id) {
		case PROGRESS_DIALOG:
			progressDialog_ = (ProgressDialog) dialog;
			progressDialog_.setIndeterminate(true);
			progressDialog_.setProgress(0);
			progressDialog_.setMessage(getString(R.string.erasing));
			break;
		}
	}

	public void updateProgress(final int done, final int total) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (done == -1) {
					dismissDialog(PROGRESS_DIALOG);
				} else {
					progressDialog_.setIndeterminate(false);
					switch (programmerState_) {
					case STATE_ERASE_IN_PROGRESS:
						progressDialog_.setMessage(getString(R.string.erasing));
						break;

					case STATE_PROGRAM_IN_PROGRESS:
						progressDialog_
								.setMessage(getString(R.string.programming));
						break;

					case STATE_VERIFY_IN_PROGRESS:
						progressDialog_
								.setMessage(getString(R.string.verifying));
						break;
					}
					progressDialog_.setMax(total);
					progressDialog_.setProgress(done);
				}
			}
		});
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
		}

		@Override
		protected void loop() throws ConnectionLostException,
				InterruptedException {
			if (programmerState_ == ProgrammerState.STATE_TARGET_CONNECTED
					&& desiredState_ != ProgrammerState.STATE_IOIO_DISCONNECTED) {
				setProgrammerState(desiredState_);
				desiredState_ = ProgrammerState.STATE_IOIO_DISCONNECTED;
			}
			switch (programmerState_) {
			case STATE_IOIO_CONNECTED:
			case STATE_TARGET_CONNECTED:
			case STATE_UNKOWN_TARGET_CONNECTED:
				icsp_.enterProgramming();
				targetId_ = Scripts.getDeviceId(icsp_);
				if ((targetId_ & 0xFF00) == 0x4100) {
					setProgrammerState(ProgrammerState.STATE_TARGET_CONNECTED);
				} else if (targetId_ == 0xFFFF) {
					setProgrammerState(ProgrammerState.STATE_IOIO_CONNECTED);
				} else {
					setProgrammerState(ProgrammerState.STATE_UNKOWN_TARGET_CONNECTED);
				}
				sleep(100);
				icsp_.exitProgramming();
				break;

			case STATE_ERASE_START:
				try {
					icsp_.enterProgramming();
					setProgrammerState(ProgrammerState.STATE_ERASE_IN_PROGRESS);
					Scripts.chipErase(icsp_);
					toast(R.string.success);
				} catch (ConnectionLostException e) {
					toast(R.string.erase_failed);
					throw e;
				} catch (InterruptedException e) {
					toast(R.string.erase_failed);
					throw e;
				} catch (Exception e) {
					toast(R.string.erase_failed);
					Log.w(TAG, e);
				} finally {
					icsp_.exitProgramming();
					updateProgress(-1, 0);
					setProgrammerState(ProgrammerState.STATE_TARGET_CONNECTED);
				}
				break;

			case STATE_PROGRAM_START:
				try {
					cancel_ = false;
					icsp_.enterProgramming();
					setProgrammerState(ProgrammerState.STATE_ERASE_IN_PROGRESS);
					IOIOFileReader file = new IOIOFileReader(selectedImage_);
					nTotalBlocks_ = IOIOFileProgrammer.countBlocks(file);
					Scripts.chipErase(icsp_);
					setProgrammerState(ProgrammerState.STATE_PROGRAM_IN_PROGRESS);
					nBlocksDone_ = 0;
					file.rewind();
					while (file.next()) {
						if (cancel_) {
							toast(R.string.aborted);
							return;
						}
						IOIOFileProgrammer.programIOIOFileBlock(icsp_, file);
						blockDone();
					}
					setProgrammerState(ProgrammerState.STATE_VERIFY_IN_PROGRESS);
					nBlocksDone_ = 0;
					file.rewind();
					while (file.next()) {
						if (cancel_) {
							toast(R.string.aborted);
							return;
						}
						if (!IOIOFileProgrammer
								.verifyIOIOFileBlock(icsp_, file)) {
							toast(R.string.verify_failed);
							return;
						}
						blockDone();
					}
					toast(R.string.success);
				} catch (FormatException e) {
					toast(R.string.image_file_corrupt);
				} catch (ConnectionLostException e) {
					toast(R.string.programming_failed);
					throw e;
				} catch (InterruptedException e) {
					toast(R.string.programming_failed);
					throw e;
				} catch (Exception e) {
					toast(R.string.programming_failed);
					Log.w(TAG, e);
				} finally {
					updateProgress(-1, 0);
					icsp_.exitProgramming();
					setProgrammerState(ProgrammerState.STATE_TARGET_CONNECTED);
				}
				break;
			}
			sleep(100);
		}

		@Override
		protected void disconnected() throws InterruptedException {
			setProgrammerState(ProgrammerState.STATE_IOIO_DISCONNECTED);
		}

		@Override
		public void blockDone() {
			updateProgress(++nBlocksDone_, nTotalBlocks_);
		}
	}

	private void setStatusText(String text) {
		statusTextView_.setText(text);
	}

	private void setStatusText(int resid) {
		statusTextView_.setText(resid);
	}

	private void toast(final int resid) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				Toast.makeText(ProgrammerActivity.this, resid, Toast.LENGTH_LONG)
						.show();
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