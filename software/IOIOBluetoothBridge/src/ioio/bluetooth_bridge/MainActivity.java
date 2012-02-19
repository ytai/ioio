package ioio.bluetooth_bridge;

import java.io.IOException;
import java.net.ServerSocket;
import java.util.UUID;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends Activity {
	static final int IOIO_PORT = 4545;

	private class BTConnectionThread extends BluetoothServerThread {
		private TextView mText;

		public BTConnectionThread() throws IOException {
			super(
					BluetoothAdapter
							.getDefaultAdapter()
							.listenUsingRfcommWithServiceRecord(
									"IOIO Bridge",
									UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")));
			mText = (TextView) findViewById(R.id.bluetooth_text_view);
		}

		protected void connected() {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					mText.setText(R.string.bluetooth_connected);
					mText.setTextColor(getResources().getColor(R.color.connected_color));
				}
			});
		}

		protected void disconnected() {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					mText.setText(R.string.bluetooth_disconnected);
					mText.setTextColor(getResources().getColor(R.color.disconnected_color));
				}
			});
		}
		
		protected void process(byte[] buf, int size) {
			try {
				if (mIOIOConnectionThread.isConnected()) {
					mIOIOConnectionThread.write(buf, size);
				}
			} catch (IOException e) {
			}
		}
	}

	private class IOIOConnectionThread extends ServerThread {
		private TextView mText;

		public IOIOConnectionThread() throws IOException {
			super(new ServerSocket(IOIO_PORT));
			mText = (TextView) findViewById(R.id.ioio_text_view);
		}

		protected void connected() {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					mText.setText(R.string.ioio_connected);
					mText.setTextColor(getResources().getColor(R.color.connected_color));
				}
			});
		}

		protected void disconnected() {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					mText.setText(R.string.ioio_disconnected);
					mText.setTextColor(getResources().getColor(R.color.disconnected_color));
				}
			});
		}

		protected void process(byte[] buf, int size) {
			try {
				if (mBTConnectionThread.isConnected()) {
					mBTConnectionThread.write(buf, size);
				}
			} catch (IOException e) {
			}
		}
	}

	private IOIOConnectionThread mIOIOConnectionThread;
	private BTConnectionThread mBTConnectionThread;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		try {
			mIOIOConnectionThread = new IOIOConnectionThread();
			mIOIOConnectionThread.start();
			mBTConnectionThread = new BTConnectionThread();
			mBTConnectionThread.start();
		} catch (IOException e) {
		}
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		mIOIOConnectionThread.kill();
		mBTConnectionThread.kill();
	}
}