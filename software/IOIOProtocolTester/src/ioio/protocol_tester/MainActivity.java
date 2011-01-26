package ioio.protocol_tester;

import java.io.IOException;
import java.io.InputStream;
import java.net.ServerSocket;
import java.net.Socket;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends Activity {
	static final int IOIO_PORT = 4545;

	private class ConnectionThread extends Thread {
		private TextView mText;
		private ServerSocket mServer = null;
		private Socket mSocket = null;
		private boolean mStop = false;

		private void connected() {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					mText.setText(":-)");
				}
			});
		}

		private void disconnected() {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					mText.setText(":-(");
				}
			});
		}

		@Override
		public void run() {
			mText = (TextView) findViewById(R.id.text_view);
			disconnected();
			try {
				mServer = new ServerSocket(IOIO_PORT);
				while (!mStop) {
					try {
						Log.i("IOIOProtocolTester", "accepting");
						mSocket = mServer.accept();
						Log.i("IOIOProtocolTester", "accepted");
						connected();
						InputStream in = mSocket.getInputStream();
						while (in.read() != -1);
					} finally {
						disconnected();
					}
				}
			} catch (IOException e) {
				if (!mStop) {
					Log.e("IOIOProtocolTester", "Exception caught", e);
				}
			}
		}

		public void kill() {
			try {
				mStop = true;
				mSocket.shutdownOutput();
				mSocket.shutdownInput();
			} catch (Exception e) {
			}
			try {
				mServer.close();
			} catch (Exception e) {
			}
			try {
				join();
			} catch (InterruptedException e) {
			}
		}
	}

	private ConnectionThread mConnectionThread = new ConnectionThread();

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		mConnectionThread.start();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		mConnectionThread.kill();
	}
}