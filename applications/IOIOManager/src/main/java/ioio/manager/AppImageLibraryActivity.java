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

import ioio.manager.FirmwareManager.ImageBundle;
import ioio.manager.FirmwareManager.ImageFile;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.Comparator;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ListActivity;
import android.content.ActivityNotFoundException;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView.AdapterContextMenuInfo;
import android.widget.BaseAdapter;
import android.widget.RadioButton;
import android.widget.TextView;
import android.widget.Toast;

public class AppImageLibraryActivity extends ListActivity {
	private static final String TAG = "AppImageLibraryActivity";
	private static final int ADD_FROM_FILE = 0;
	private static final int ADD_FROM_QR = 1;
	private static final int ABOUT_DIALOG = 0;
	FirmwareManager firmwareManager_;
	private ListAdapter listAdapter_;
	private ImageBundle[] bundles_;

	private class ListAdapter extends BaseAdapter {

		@Override
		public void notifyDataSetChanged() {
			getAppBundles();
			super.notifyDataSetChanged();
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			final int pos = position;
			if (convertView == null) {
				convertView = getLayoutInflater().inflate(
						R.layout.app_image_list_item, parent, false);
			}
			convertView.setOnClickListener(new View.OnClickListener() {
				@Override
				public void onClick(View v) {
					AppImageLibraryActivity.this.onClick(v, pos);
				}

			});
			ImageBundle bundle = bundles_[position];
			((RadioButton) convertView.findViewById(R.id.checked))
					.setChecked(bundle.isActive());
			String name = bundle.getName();
			if (name == null) {
				name = getString(R.string.unnamed);
				((TextView) convertView.findViewById(R.id.title))
						.setTextColor(Color.DKGRAY);
			}
			convertView.setLongClickable(name != null);
			((TextView) convertView.findViewById(R.id.title)).setText(name);
			ImageFile[] images = bundle.getImages();
			Arrays.sort(images, new Comparator<ImageFile>() {
				@Override
				public int compare(ImageFile i1, ImageFile i2) {
					return i1.getName().compareTo(i2.getName());
				}
			});
			StringBuffer strbuf = new StringBuffer();
			for (int i = 0; i < images.length; ++i) {
				if (i != 0) {
					strbuf.append(' ');
				}
				strbuf.append(images[i].getName());
			}
			((TextView) convertView.findViewById(R.id.content)).setText(strbuf
					.toString());
			return convertView;
		}

		@Override
		public long getItemId(int position) {
			return position;
		}

		@Override
		public Object getItem(int position) {
			return bundles_[position];
		}

		@Override
		public int getCount() {
			return bundles_.length;
		}

	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.app_image_library);
		setTitle(R.string.app_image_title);
		try {
			firmwareManager_ = FirmwareManager.getInstance(this);
			getAppBundles();
			listAdapter_ = new ListAdapter();
			setListAdapter(listAdapter_);
			registerForContextMenu(getListView());
		} catch (IOException e) {
			Log.w(TAG, e);
		}
		handleIntent(getIntent());
	}

	@Override
	protected void onNewIntent(Intent intent) {
		super.onNewIntent(intent);
		handleIntent(intent);
	}

	private void handleIntent(Intent intent) {
		final Intent fintent = intent;
		if (intent.getAction().equals(Intent.ACTION_VIEW)) {
			AlertDialog.Builder builder = new AlertDialog.Builder(this);
			builder.setTitle(R.string.grant_permission);
			builder.setMessage(R.string.external_approval_add_app);
			builder.setPositiveButton(R.string.yes,
					new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
							if (fintent.getScheme().equals("file")) {
								addBundleFromFile(new File(fintent.getData()
										.getPath()));
							} else if (fintent.getScheme().equals("http")) {
								addBundleFromUrl(fintent.getDataString());
							}
							setIntent(new Intent(Intent.ACTION_MAIN));
						}
					});
			builder.setNegativeButton(R.string.no,
					new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
							setIntent(new Intent(Intent.ACTION_MAIN));
						}
					});
			builder.setIcon(android.R.drawable.ic_dialog_alert);
			builder.show();
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.firmware_options, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle item selection
		switch (item.getItemId()) {
		case R.id.addByScanMenuItem:
			addByScan();
			return true;
		case R.id.addFromStorageMenuItem:
			addFromExternalStorage();
			return true;
		case R.id.clearActiveBundle:
			clearActiveBundle();
			return true;
		case R.id.about:
			showDialog(ABOUT_DIALOG);
			return true;
		case R.id.programmer:
			startProgrammer();
			return true;
		default:
			return super.onOptionsItemSelected(item);
		}
	}

	private void startProgrammer() {
		startActivity(new Intent(this, ProgrammerActivity.class));
	}

	@Override
	protected Dialog onCreateDialog(int id) {
		switch (id) {
		case ABOUT_DIALOG:
			return new AlertDialog.Builder(this)
					.setIcon(R.drawable.ic_dialog_about)
					.setTitle(R.string.about_title)
					.setView(getLayoutInflater().inflate(R.layout.about, null))
					.create();

		default:
			return super.onCreateDialog(id);
		}
	}

	private void clearActiveBundle() {
		try {
			firmwareManager_.clearActiveBundle();
			listAdapter_.notifyDataSetChanged();
			Toast.makeText(this, R.string.active_bundle_cleared,
					Toast.LENGTH_SHORT).show();
		} catch (IOException e) {
			Log.w(TAG, e);
		}
	}

	private void addByScan() {
		Intent intent = new Intent("com.google.zxing.client.android.SCAN");
		intent.putExtra("SCAN_MODE", "QR_CODE_MODE");
		try {
			startActivityForResult(intent, ADD_FROM_QR);
		} catch (ActivityNotFoundException e) {
			Toast.makeText(this, R.string.install_zxing, Toast.LENGTH_SHORT)
					.show();
		}
	}

	private void addFromExternalStorage() {
		Intent intent = new Intent(this, SelectFileActivity.class);
		intent.putExtra(SelectFileActivity.EXTRA_START_DIR, Environment
				.getExternalStorageDirectory().getAbsolutePath());
		intent.putExtra(SelectFileActivity.EXTRA_SUFFIXES,
				new String[] { ".ioioapp" });
		startActivityForResult(intent, ADD_FROM_FILE);
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		switch (requestCode) {
		case ADD_FROM_FILE:
			if (resultCode == RESULT_OK) {
				File file = (File) new File(data.getData().getPath());
				addBundleFromFile(file);
			} else if (resultCode == FileReturner.RESULT_ERROR) {
				Toast.makeText(
						this,
						String.format(
								getString(R.string.error),
								data.getStringExtra(FileReturner.ERROR_MESSAGE_EXTRA)),
						Toast.LENGTH_LONG).show();
			}
			break;

		case ADD_FROM_QR:
			if (resultCode == RESULT_OK) {
				try {
					String contents = data.getStringExtra("SCAN_RESULT");
					String format = data.getStringExtra("SCAN_RESULT_FORMAT");
					if (format.equals("QR_CODE")) {
						addBundleFromUrl(contents);
					} else {
						Toast.makeText(this, R.string.barcode_not_uri,
								Toast.LENGTH_LONG).show();
					}
				} catch (Exception e) {
					Log.w(TAG, e);
					Toast.makeText(
							this,
							String.format(
									getString(R.string.failed_add_bundle),
									e.getMessage()), Toast.LENGTH_LONG).show();
				}
			}
			break;
		}
	}

	private void addBundleFromUrl(String url) {
		Intent intent = new Intent(this, DownloadUrlActivity.class);
		intent.putExtra(DownloadUrlActivity.URL_EXTRA, url);
		startActivityForResult(intent, ADD_FROM_FILE);
	}

	private void addBundleFromFile(File file) {
		try {
			ImageBundle bundle = firmwareManager_.addAppBundle(file
					.getAbsolutePath());
			listAdapter_.notifyDataSetChanged();
			Toast.makeText(
					this,
					String.format(getString(R.string.bundle_added),
							bundle.getName()), Toast.LENGTH_SHORT).show();
		} catch (Exception e) {
			Log.w(TAG, e);
			Toast.makeText(
					this,
					String.format(getString(R.string.failed_add_bundle),
							e.getMessage()), Toast.LENGTH_LONG).show();
		}
	}

	@Override
	public void onCreateContextMenu(ContextMenu menu, View v,
			ContextMenuInfo menuInfo) {
		super.onCreateContextMenu(menu, v, menuInfo);
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.list_item_context_menu, menu);
	}

	@Override
	public boolean onContextItemSelected(MenuItem item) {
		AdapterContextMenuInfo info = (AdapterContextMenuInfo) item
				.getMenuInfo();
		switch (item.getItemId()) {
		case R.id.remove_item:
			try {
				String name = bundles_[(int) info.id].getName();
				firmwareManager_.removeAppBundle(name);
				listAdapter_.notifyDataSetChanged();
				Toast.makeText(
						this,
						String.format(getString(R.string.bundle_removed), name),
						Toast.LENGTH_SHORT).show();
				return true;
			} catch (IOException e) {
				Log.w(TAG, e);
				Toast.makeText(
						this,
						String.format(getString(R.string.failed_remove_bundle),
								e.getMessage()), Toast.LENGTH_SHORT).show();
			}
		default:
			return super.onContextItemSelected(item);
		}
	}

	private void onClick(View v, int pos) {
		ImageBundle bundle = (ImageBundle) getListView().getItemAtPosition(pos);
		try {
			if (bundle.isActive()) {
				clearActiveBundle();
			} else {
				firmwareManager_.clearActiveBundle();
				firmwareManager_.setActiveAppBundle(bundle.getName());
				listAdapter_.notifyDataSetChanged();
				Toast.makeText(
						this,
						String.format(getString(R.string.bundle_set_active),
								bundle.getName()), Toast.LENGTH_SHORT).show();
			}
		} catch (IOException e) {
			Log.w(TAG, e);
		}
	}

	private void getAppBundles() {
		bundles_ = firmwareManager_.getAppBundles();
		Arrays.sort(bundles_, new Comparator<ImageBundle>() {
			@Override
			public int compare(ImageBundle b1, ImageBundle b2) {
				if (b1.getName() == null) {
					return -1;
				}
				if (b2.getName() == null) {
					return 1;
				}
				return b1.getName().compareTo(b2.getName());
			}
		});
	}
}