package ioio.manager;

import java.io.File;
import java.io.FileFilter;
import java.io.FilenameFilter;
import java.util.Stack;

import android.app.ListActivity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.graphics.Typeface;
import android.net.Uri;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

public class SelectFileActivity extends ListActivity implements FileReturner {
	public static final String EXTRA_START_DIR = "ioio.manager.START_DIR";
	public static final String EXTRA_SUFFIXES = "ioio.manager.SUFFIXES";
	private File currentDir_;
	private String[] suffixes_;
	private FileAdapter adapter_;
	private Stack<File> fileStack_ = new Stack<File>();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Intent intent = getIntent();
		suffixes_ = intent.getStringArrayExtra(EXTRA_SUFFIXES);
		currentDir_ = new File(intent.getStringExtra(EXTRA_START_DIR));
		adapter_ = new FileAdapter(this, R.layout.file_list_item);
		fileStack_.clear();
		setListAdapter(adapter_);
		refreshFiles();
	}

	@Override
	protected void onListItemClick(ListView l, View v, int position, long id) {
		super.onListItemClick(l, v, position, id);
		File file = adapter_.getItem(position);
		if (position == 0 && !fileStack_.empty()) {
			currentDir_ = fileStack_.pop();
			refreshFiles();
		} else if (file.isDirectory()) {
			fileStack_.push(currentDir_);
			currentDir_ = file;
			refreshFiles();
		} else {
			Intent intent = new Intent(Intent.ACTION_VIEW,
					new Uri.Builder().scheme("file")
							.path(file.getAbsolutePath()).build());
			setResult(RESULT_OK, intent);
			finish();
		}
	}

	private void refreshFiles() {
		adapter_.clear();

		if (!fileStack_.empty()) {
			adapter_.add(null);
		}

		File[] subdirs = currentDir_.listFiles(new FileFilter() {
			@Override
			public boolean accept(File f) {
				return f.isDirectory() && f.canRead();
			}
		});
		for (File f : subdirs) {
			adapter_.add(f);
		}

		File[] files = currentDir_.listFiles(new FilenameFilter() {
			@Override
			public boolean accept(File dir, String filename) {
				for (String s : suffixes_) {
					if (filename.endsWith(s)) {
						return true;
					}
				}
				return false;
			}
		});
		for (int i = 0; i < files.length; ++i) {
			adapter_.add(files[i]);
		}
		adapter_.notifyDataSetChanged();
	}

	private class FileAdapter extends ArrayAdapter<File> {
		public FileAdapter(Context context, int textViewResourceId) {
			super(context, textViewResourceId);
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			if (convertView == null) {
				convertView = getLayoutInflater().inflate(
						R.layout.file_list_item, parent, false);
			}
			File item = getItem(position);
			TextView textView = (TextView) convertView
					.findViewById(R.id.fileListTextView);
			Resources res = getResources();
			if (item == null) {
				textView.setText("..");
				textView.setTextColor(res.getColor(R.color.fileListUp));
				textView.setTypeface(Typeface.DEFAULT_BOLD);
			} else {
				textView.setText(item.getName());
				if (item.isDirectory()) {
					textView.setTextColor(res.getColor(R.color.fileListDir));
					textView.setTypeface(Typeface.DEFAULT);
				} else {
					textView.setTextColor(res.getColor(R.color.fileListFile));
					textView.setTypeface(Typeface.DEFAULT_BOLD);
				}
			}
			return convertView;
		}

	}
}
