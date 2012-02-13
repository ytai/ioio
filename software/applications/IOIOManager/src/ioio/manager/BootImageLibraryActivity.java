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
import android.app.ExpandableListActivity;
import android.content.ActivityNotFoundException;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
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
import android.widget.BaseExpandableListAdapter;
import android.widget.ExpandableListView;
import android.widget.ExpandableListView.ExpandableListContextMenuInfo;
import android.widget.TextView;
import android.widget.Toast;

public class BootImageLibraryActivity extends ExpandableListActivity {
	private static final String TAG = "BootImageLibraryActivity";
	public static final String ACTION_SELECT = "SELECT";
	public static final String ACTION_EDIT = "EDIT";
	public static final String EXTRA_BUNDLE_NAME = "BUNDLE_NAME";
	public static final String EXTRA_IMAGE_NAME = "IAMGE_NAME";
	private static final int ADD_FROM_FILE = 0;
	private static final int ADD_FROM_QR = 1;

	private ListAdapter adapter_;
	private FirmwareManager firmwareManager_;
	private ImageBundle[] bundles_;
	private boolean isSelect_ = false;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.boot_image_library);
		if (getIntent().getAction().equals(ACTION_SELECT)) {
			isSelect_ = true;
			setTitle(R.string.select_image);
		} else {
			setTitle(R.string.boot_image_library_title);
		}
		try {
			firmwareManager_ = FirmwareManager.getInstance(this);
			getImageBundles();
			adapter_ = new ListAdapter();
			setListAdapter(adapter_);
			registerForContextMenu(getExpandableListView());
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
			builder.setMessage(R.string.external_approval_add_boot);
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
	public boolean onChildClick(ExpandableListView parent, View v,
			int groupPosition, int childPosition, long id) {
		ImageBundle bundle = (ImageBundle) adapter_.getGroup(groupPosition);
		ImageFile selected = (ImageFile) adapter_.getChild(groupPosition,
				childPosition);
		Intent intent = new Intent(ACTION_SELECT, new Uri.Builder()
				.scheme("file").path(selected.getFile().getAbsolutePath())
				.build());
		intent.putExtra(EXTRA_BUNDLE_NAME, bundle.getName());
		intent.putExtra(EXTRA_IMAGE_NAME, selected.getName());
		setResult(RESULT_OK, intent);
		finish();
		return true;
	}

	private class ListAdapter extends BaseExpandableListAdapter {
		private static final int MAX_IMAGES_PER_BUNDLE = 16384;

		@Override
		public void notifyDataSetChanged() {
			getImageBundles();
			super.notifyDataSetChanged();
		}

		@Override
		public boolean isChildSelectable(int groupPosition, int childPosition) {
			return isSelect_;
		}

		@Override
		public boolean hasStableIds() {
			return false;
		}

		@Override
		public View getGroupView(int groupPosition, boolean isExpanded,
				View convertView, ViewGroup parent) {
			if (convertView == null) {
				convertView = getLayoutInflater().inflate(
						R.layout.boot_image_list_group, null, false);
			}
			((TextView) convertView).setText(bundles_[groupPosition].getName());
			return convertView;
		}

		@Override
		public long getGroupId(int groupPosition) {
			return groupPosition;
		}

		@Override
		public int getGroupCount() {
			return bundles_.length;
		}

		@Override
		public Object getGroup(int groupPosition) {
			return bundles_[groupPosition];
		}

		@Override
		public int getChildrenCount(int groupPosition) {
			return bundles_[groupPosition].getImages().length;
		}

		@Override
		public View getChildView(int groupPosition, int childPosition,
				boolean isLastChild, View convertView, ViewGroup parent) {
			if (convertView == null) {
				convertView = getLayoutInflater().inflate(
						R.layout.boot_image_list_item, null, false);
			}
			((TextView) convertView.findViewById(R.id.programmerListItemTitle))
					.setText(((ImageFile) getChild(groupPosition, childPosition))
							.getName());
			return convertView;
		}

		@Override
		public long getChildId(int groupPosition, int childPosition) {
			return groupPosition * MAX_IMAGES_PER_BUNDLE + childPosition;
		}

		@Override
		public Object getChild(int groupPosition, int childPosition) {
			ImageFile[] images = bundles_[groupPosition].getImages();
			Arrays.sort(images, new Comparator<ImageFile>() {
				@Override
				public int compare(ImageFile i1, ImageFile i2) {
					return i1.getName().compareTo(i2.getName());
				}
			});
			return images[childPosition];
		}
	};

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.image_library_options, menu);
		return true;
	}

	@Override
	public boolean onContextItemSelected(MenuItem item) {
		ExpandableListContextMenuInfo info = (ExpandableListContextMenuInfo) item
				.getMenuInfo();
		switch (item.getItemId()) {
		case R.id.remove_item:
			try {
				String name = bundles_[(int) info.id].getName();
				firmwareManager_.removeImageBundle(name);
				adapter_.notifyDataSetChanged();
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
		default:
			return super.onOptionsItemSelected(item);
		}
	}

	@Override
	public void onCreateContextMenu(ContextMenu menu, View v,
			ContextMenuInfo menuInfo) {
		super.onCreateContextMenu(menu, v, menuInfo);
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.list_item_context_menu, menu);
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
				new String[] { ".ioioimg" });
		startActivityForResult(intent, ADD_FROM_FILE);
	}

	private void getImageBundles() {
		bundles_ = firmwareManager_.getImageBundles();
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
			ImageBundle bundle = firmwareManager_.addImageBundle(file
					.getAbsolutePath());
			adapter_.notifyDataSetChanged();
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
}
