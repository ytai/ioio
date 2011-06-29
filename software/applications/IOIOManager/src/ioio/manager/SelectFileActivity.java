package ioio.manager;

import java.io.File;
import java.io.FilenameFilter;

import android.app.ListActivity;
import android.content.Intent;
import android.os.Environment;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;

public class SelectFileActivity extends ListActivity implements FileReturner {
	private File[] files_;

	@Override
	protected void onStart() {
		super.onStart();
		File directory = Environment.getExternalStorageDirectory();
		files_ = directory.listFiles(new FilenameFilter() {
			@Override
			public boolean accept(File dir, String filename) {
				return filename.endsWith(".ioiozip");
			}
		});
		String[] names = new String[files_.length];
		for (int i = 0; i < files_.length; ++i) {
			names[i] = files_[i].getName();
		}

		setListAdapter(new ArrayAdapter<String>(this, R.layout.file_list_item,
				names));
		getListView().setOnItemClickListener(new OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {
				Intent intent = new Intent();
				intent.putExtra(SELECTED_FILE_EXTRA, files_[position]);
				setResult(RESULT_OK, intent);
				finish();
			}
		});
	}
}
