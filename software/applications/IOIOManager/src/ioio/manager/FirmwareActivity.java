package ioio.manager;

import ioio.manager.FirmwareManager.ImageFile;

import java.io.File;
import java.io.IOException;

import android.app.ListActivity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnLongClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView.AdapterContextMenuInfo;
import android.widget.BaseAdapter;
import android.widget.RadioButton;
import android.widget.TextView;
import android.widget.Toast;

public class FirmwareActivity extends ListActivity {
	private static final String TAG = "FirmwareActivity";
	private static final int ADD_FROM_FILE = 1;
	FirmwareManager firmwareManager_;
	private ListAdapter listAdapter_;
	private ioio.manager.FirmwareManager.Bundle[] bundles_;

	private class ListAdapter extends BaseAdapter {

		@Override
		public void notifyDataSetChanged() {
			bundles_ = firmwareManager_.getAppBundles();
			super.notifyDataSetChanged();
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			final int pos = position;
			if (convertView == null) {
				convertView = getLayoutInflater().inflate(R.layout.list_item,
						parent, false);
			}
			convertView.setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					FirmwareActivity.this.onClick(v, pos);
				}

			});
			ioio.manager.FirmwareManager.Bundle bundle = bundles_[position];
			((RadioButton) convertView.findViewById(R.id.checked))
					.setChecked(bundle.isActive());
			((TextView) convertView.findViewById(R.id.title)).setText(bundle
					.getName());
			ImageFile[] images = bundle.getImages();
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
		try {
			firmwareManager_ = new FirmwareManager(this);
			bundles_ = firmwareManager_.getAppBundles();
			listAdapter_ = new ListAdapter();
			setListAdapter(listAdapter_);
			registerForContextMenu(getListView());
		} catch (IOException e) {
			Log.w(TAG, e);
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
		default:
			return super.onOptionsItemSelected(item);
		}
	}

	private void clearActiveBundle() {
		try {
			firmwareManager_.clearActiveBundle();
			listAdapter_.notifyDataSetChanged();
			Toast.makeText(this, "Active bundle cleared", Toast.LENGTH_SHORT)
					.show();
		} catch (IOException e) {
			Log.w(TAG, e);
		}
	}

	private void addByScan() {
		Toast.makeText(this, "add by scan", Toast.LENGTH_SHORT).show();
	}

	private void addFromExternalStorage() {
		startActivityForResult(new Intent(this, SelectFileActivity.class),
				ADD_FROM_FILE);
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		switch (requestCode) {
		case ADD_FROM_FILE:
			if (resultCode == RESULT_OK) {
				try {
					File file = (File) data
							.getSerializableExtra(SelectFileActivity.SELECTED_FILE_EXTRA);
					firmwareManager_.addAppBundle(file.getAbsolutePath());
					listAdapter_.notifyDataSetChanged();
					Toast.makeText(this, "Bundle added", Toast.LENGTH_SHORT)
							.show();
				} catch (Exception e) {
					Log.w(TAG, e);
					Toast.makeText(this,
							"Failed to add bundle: " + e.getMessage(),
							Toast.LENGTH_LONG).show();
				}
			}
			break;
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
				firmwareManager_.removeAppBundle(bundles_[(int) info.id]
						.getName());
				listAdapter_.notifyDataSetChanged();
				Toast.makeText(this, "Bundle removed", Toast.LENGTH_SHORT)
						.show();
				return true;
			} catch (IOException e) {
				Log.w(TAG, e);
				Toast.makeText(this,
						"Failed to remove bundle: " + e.getMessage(),
						Toast.LENGTH_SHORT).show();
			}
		default:
			return super.onContextItemSelected(item);
		}
	}

	private void onClick(View v, int pos) {
		ioio.manager.FirmwareManager.Bundle bundle = (ioio.manager.FirmwareManager.Bundle) getListView()
				.getItemAtPosition(pos);
		try {
			firmwareManager_.clearActiveBundle();
			firmwareManager_.setActiveAppBundle(bundle.getName());
			listAdapter_.notifyDataSetChanged();
			Toast.makeText(this,
					"Bundle " + bundle.getName() + " set as active",
					Toast.LENGTH_SHORT).show();
		} catch (IOException e) {
			Log.w(TAG, e);
		}
	}
}