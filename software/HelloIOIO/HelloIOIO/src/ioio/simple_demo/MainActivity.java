package ioio.simple_demo;

import java.net.ServerSocket;
import java.net.Socket;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ToggleButton;

public class MainActivity extends Activity {
	private NetThread thread_ = new NetThread();
	private ToggleButton button_;
	private View main_view_;

	private class NetThread extends Thread {
		private Socket socket_;
		private ServerSocket server_;

		@Override
		public void run() {
			try {
				server_ = new ServerSocket(4356);
				while (true) {
					Log.i("SimpleDemo", "Accepting");
					socket_ = server_.accept();
					Log.i("SimpleDemo", "Accepted");
					int i;
					while ((i = socket_.getInputStream().read()) != -1) {
						handleIncoming((byte) i);
					}
					Log.i("SimpleDemo", "Closed");
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		public void write(int b) {
			try {
				socket_.getOutputStream().write(b);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		public void kill() {
			try {
				if (server_ != null) {
					server_.close();
				}
				if (socket_ != null) {
					socket_.close();
				}
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

	}

	public void handleIncoming(final byte b) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (b == 'n') {
					main_view_.setBackgroundColor(Color.WHITE);
				} else if (b == 'f') {
					main_view_.setBackgroundColor(Color.BLACK);
				}

			}
		});
	}

	@Override
	public void onAttachedToWindow() {
		main_view_ = findViewById(R.id.Main);
	}

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		button_ = (ToggleButton) findViewById(R.id.ToggleButton01);
		thread_.start();
	}

	public void toggleButtonClicked(View v) {
		try {
			if (button_.isChecked()) {
				thread_.write('n');
			} else {
				thread_.write('f');
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		thread_.kill();
	}

}