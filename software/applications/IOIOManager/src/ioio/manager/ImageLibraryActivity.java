package ioio.manager;

import ioio.manager.FirmwareManager.ImageBundle;
import ioio.manager.FirmwareManager.ImageFile;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.Comparator;

import android.app.ExpandableListActivity;
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.ExpandableListView;
import android.widget.TextView;
import android.widget.Toast;

public class ImageLibraryActivity extends ExpandableListActivity {
	private static final String TAG = "ImageLibraryActivity";
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
		setContentView(R.layout.image_library);
		if (getIntent().getAction().equals(ACTION_SELECT)) {
			isSelect_ = true;
		}
		try {
			firmwareManager_ = new FirmwareManager(this); // TODO: share?
			getImageBundles();
			adapter_ = new ListAdapter();
			setListAdapter(adapter_);
		} catch (IOException e) {
			Log.w(TAG, e);
		}
	}

	@Override
	public boolean onChildClick(ExpandableListView parent, View v,
			int groupPosition, int childPosition, long id) {
		ImageBundle bundle = (ImageBundle) adapter_.getGroup(groupPosition);
		ImageFile selected = (ImageFile) adapter_.getChild(groupPosition,
				childPosition);
		Intent intent = new Intent(ACTION_SELECT,
				new Uri.Builder().scheme("file")
				.path(selected.getFile().getAbsolutePath())
				.build());
		intent.putExtra(EXTRA_BUNDLE_NAME, bundle.getName());
		intent.putExtra(EXTRA_IMAGE_NAME, selected.getName());
		setResult(RESULT_OK,
				intent);
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

		public View getGroupView(int groupPosition, boolean isExpanded,
				View convertView, ViewGroup parent) {
			if (convertView == null) {
				convertView = getLayoutInflater().inflate(
						R.layout.image_list_group, null, false);
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
						R.layout.image_list_item, null, false);
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

	private void addByScan() {
		Intent intent = new Intent("com.google.zxing.client.android.SCAN");
		intent.putExtra("SCAN_MODE", "QR_CODE_MODE");
		try {
			startActivityForResult(intent, ADD_FROM_QR);
		} catch (ActivityNotFoundException e) {
			Toast.makeText(this, R.string.install_zxing,
					Toast.LENGTH_SHORT).show();
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
						getString(R.string.error_colon)
								+ data.getStringExtra(FileReturner.ERROR_MESSAGE_EXTRA),
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
						Toast.makeText(this,
								R.string.barcode_not_uri,
								Toast.LENGTH_LONG).show();
					}
				} catch (Exception e) {
					Log.w(TAG, e);
					Toast.makeText(this,
							getString(R.string.failed_add_bundle) + e.getMessage(),
							Toast.LENGTH_LONG).show();
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
			Toast.makeText(this, getString(R.string.bundle_added) + bundle.getName(),
					Toast.LENGTH_SHORT).show();
		} catch (Exception e) {
			Log.w(TAG, e);
			Toast.makeText(this, getString(R.string.failed_add_bundle) + e.getMessage(),
					Toast.LENGTH_LONG).show();
		}
	}
}
