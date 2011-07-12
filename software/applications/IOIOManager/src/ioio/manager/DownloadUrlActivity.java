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
import android.net.Uri;

public class DownloadUrlActivity extends Activity implements Runnable,
		FileReturner {
	public static final String URL_EXTRA = "URL";
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
		dialog_ = ProgressDialog.show(this, getString(R.string.loading),
				String.format(getString(R.string.fetching_content), url_), false, true,
				new OnCancelListener() {
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

	@Override
	public void run() {
		try {
			URL url = new URL(url_);
			URLConnection conn = url.openConnection();
			conn.setConnectTimeout(5000);
			conn.setReadTimeout(5000);
			conn.connect();
			InputStream inputStream = conn.getInputStream();
			String basename = url.getFile();
			basename = basename.substring(basename.lastIndexOf('/') + 1);
			File file = new File(getCacheDir(), basename);
			OutputStream out = new FileOutputStream(file);
			int r;
			byte[] buf = new byte[64];
			while (-1 != (r = inputStream.read(buf))) {
				out.write(buf, 0, r);
			}
			out.close();
			Uri.Builder builder = new Uri.Builder();
			builder.scheme("file");
			builder.path(file.getAbsolutePath());
			Intent intent = new Intent(Intent.ACTION_VIEW, builder.build());
			setResult(RESULT_OK, intent);
		} catch (IOException e) {
			Intent result = new Intent();
			result.putExtra(ERROR_MESSAGE_EXTRA, e.getMessage());
			setResult(RESULT_ERROR, result);
		} finally {
			finish();
		}
	}

}
