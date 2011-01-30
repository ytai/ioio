package ioio.bluetooth_bridge;

import java.io.IOException;
import java.net.ServerSocket;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends Activity {
	static final int IOIO_PORT = 4545;

	private class ConnectionThread extends ServerThread {
		private TextView mText;
		private TextView mLog;

		public ConnectionThread() throws IOException {
			super(new ServerSocket(IOIO_PORT));
			mText = (TextView) findViewById(R.id.text_view);
			mLog = (TextView) findViewById(R.id.log_text_view);
		}

		protected void connected() {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					mText.setText(":-)");
				}
			});
		}

		protected void disconnected() {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					mText.setText(":-(");
				}
			});
		}

		protected void process(byte[] buf, int size) {
			printBuf(buf, size);
		}

		private class LogPrinter implements Runnable {
			private String mStr;

			LogPrinter(String s) {
				mStr = s;
			}

			public void run() {
				StringBuffer log = new StringBuffer(mLog.getText());
				log.append(mStr);
				if (log.length() > 200) {
					log.delete(0, log.length() - 200);
				}
				mLog.setText(log.toString());
			}

		}

		private void printBuf(byte[] buf, int size) {
			StringBuffer sb = new StringBuffer();
			for (int i = 0; i < size; ++i) {
				sb.append(Integer.toHexString(buf[i]));
				sb.append(' ');
			}
			runOnUiThread(new LogPrinter(sb.toString()));
		}
	}

	private ConnectionThread mConnectionThread;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		try {
			mConnectionThread = new ConnectionThread();
			mConnectionThread.start();
		} catch (IOException e) {
		}
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		mConnectionThread.kill();
	}
}