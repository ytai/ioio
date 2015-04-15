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
	public static final String EXTRA_START_DIR = "START_DIR";
	public static final String EXTRA_SUFFIXES = "SUFFIXES";
	private File currentDir_;
	private String[] suffixes_;
	private FileAdapter adapter_;
	private Stack<File> fileStack_ = new Stack<File>();

	@SuppressWarnings("unchecked")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if (savedInstanceState == null) {
			Intent intent = getIntent();
			suffixes_ = intent.getStringArrayExtra(EXTRA_SUFFIXES);
			currentDir_ = new File(intent.getStringExtra(EXTRA_START_DIR));
			fileStack_.clear();
		} else {
			suffixes_ = savedInstanceState.getStringArray(EXTRA_SUFFIXES);
			currentDir_ = new File(savedInstanceState.getString(EXTRA_START_DIR));
			fileStack_ = (Stack<File>) getLastNonConfigurationInstance();
		}
		adapter_ = new FileAdapter(this, R.layout.file_list_item);
		setListAdapter(adapter_);
		refreshFiles();
	}
	
	@Override
	public Object onRetainNonConfigurationInstance() {
		return fileStack_;
	}
	
	@Override
	protected void onSaveInstanceState(Bundle outState) {
		super.onSaveInstanceState(outState);
		outState.putStringArray(EXTRA_SUFFIXES, suffixes_);
		outState.putString(EXTRA_START_DIR, currentDir_.getAbsolutePath());
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
