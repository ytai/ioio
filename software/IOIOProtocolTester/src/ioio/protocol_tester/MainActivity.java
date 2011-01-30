package ioio.protocol_tester;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends Activity {
	static final int IOIO_PORT = 4545;

	private class BlinkThread extends Thread {
		private OutputStream mOut;

		BlinkThread(OutputStream out) {
			mOut = out;
		}

		@Override
		public void run() {
			try {
				while (true) {
					mOut.write(0x03);
					mOut.write(0x00);
					Thread.sleep(20);
					mOut.write(0x03);
					mOut.write(0x01);
					Thread.sleep(980);
				}
			} catch (Exception e) {
			}
		}
	};

	private class ConnectionThread extends Thread {
		private TextView mText;
		private TextView mLog;
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
			mLog = (TextView) findViewById(R.id.log_text_view);
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
						BlinkThread blink = new BlinkThread(
								mSocket.getOutputStream());
						byte[] buf = new byte[1024];
						int size;
						blink.start();
						while ((size = in.read(buf)) != -1) {
							printBuf(buf, size);
						}
						blink.stop();
					} catch (IOException e) {
					} finally {
						mSocket.close();
						disconnected();
					}
				}
			} catch (IOException e) {
				Log.e("IOIOProtocolTester", "Exception caught", e);
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