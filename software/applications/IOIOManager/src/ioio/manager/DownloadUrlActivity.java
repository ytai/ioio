package ioio.manager;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.net.URLConnection;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnCancelListener;
import android.content.Intent;
import android.util.Log;

public class DownloadUrlActivity extends Activity implements Runnable,
		FileReturner {
	public static final String URL_EXTRA = "ioio.manager.EXTRA_URL";
	private ProgressDialog dialog_;
	private String url_;
	private Thread thread_;

	@Override
	protected void onStart() {
		super.onStart();
		Intent intent = getIntent();
		url_ = intent.getStringExtra(URL_EXTRA);
		thread_ = new Thread(this);
		thread_.start();
		dialog_ = ProgressDialog.show(this, "Loading",
				"Fetching content from: " + url_ + "\n\nPress back to abort",
				false, true, new OnCancelListener() {
					@Override
					public void onCancel(DialogInterface dialog) {
						setResult(RESULT_CANCELED);
						thread_.interrupt();
						finish();
					}
				});
	}

	@Override
	protected void onStop() {
		super.onStop();
		if (dialog_ != null) {
			dialog_.dismiss();
			dialog_ = null;
		}
	}
	
//	private void done(final int result, final Intent data) {
//		runOnUiThread(new Runnable() {
//			@Override
//			public void run() {
//				setResult(result, data);
//				finish();
//			}
//		});
//	}

	@Override
	public void run() {
		Log.i("xxx", "run");
		try {
			URL url = new URL(url_);
			URLConnection conn = url.openConnection();
			conn.setConnectTimeout(5000);
			conn.setReadTimeout(5000);
			conn.connect();
			InputStream inputStream = conn.getInputStream();
			File file = new File(getCacheDir(), url.getFile());
			OutputStream out = new FileOutputStream(file);
			int r;
			byte[] buf = new byte[64];
			while (-1 != (r = inputStream.read(buf))) {
				out.write(buf, 0, r);
			}
			out.close();
			Intent result = new Intent();
			result.putExtra(SELECTED_FILE_EXTRA, file);
			setResult(RESULT_OK, result);
		} catch (IOException e) {
			Intent result = new Intent();
			result.putExtra(ERROR_MESSAGE_EXTRA, e.getMessage());
			setResult(RESULT_ERROR, result);
			Log.w("xxx", "Thread caught exception.", e);
		} finally {
			Log.i("xxx", "Thread done.");
			finish();
		}
	}

}
