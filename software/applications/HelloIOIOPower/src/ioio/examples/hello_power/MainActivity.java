package ioio.examples.hello_power;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.IOIOFactory;
import ioio.lib.api.exception.ConnectionLostException;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import android.widget.ToggleButton;

/**
 * This is the main activity of the HelloIOIOPower example application.
 * 
 * It displays a toggle button on the screen, which enables control of the
 * on-board LED, as well as a text message that shows whether the IOIO is
 * connected.
 * 
 * Compared to the HelloIOIO example, this example does not use the
 * AbstractIOIOActivity utility class, thus has finer control of thread creation
 * and IOIO-connection process. For a simpler use cases, see the HelloIOIO
 * example.
 */
public class MainActivity extends Activity {
	/** The text displayed at the top of the page. */
	private TextView title_;
	/** The toggle button used to control the LED. */
	private ToggleButton button_;
	/** The thread that interacts with the IOIO. */
	private IOIOThread ioio_thread_;

	/**
	 * Called when the activity is first created. Here we normally initialize
	 * our GUI.
	 */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		title_ = (TextView) findViewById(R.id.title);
		button_ = (ToggleButton) findViewById(R.id.button);
	}

	/**
	 * Called when the application is resumed (also when first started). Here is
	 * where we'll create our IOIO thread.
	 */
	@Override
	protected void onResume() {
		super.onResume();
		ioio_thread_ = new IOIOThread();
		ioio_thread_.start();
	}

	/**
	 * Called when the application is paused. We want to disconnect with the
	 * IOIO at this point, as the user is no longer interacting with our
	 * application.
	 */
	@Override
	protected void onPause() {
		super.onPause();
		ioio_thread_.abort();
		try {
			ioio_thread_.join();
		} catch (InterruptedException e) {
		}
	}

	/**
	 * This is the thread that does the IOIO interaction.
	 * 
	 * It first creates a IOIO instance and wait for a connection to be
	 * established. Then it starts doing the main work of opening the LED pin
	 * and constantly updating it to match the toggle button's state.
	 * 
	 * Whenever a connection drops, it tries to reconnect, unless this is a
	 * result of abort().
	 */
	class IOIOThread extends Thread {
		private IOIO ioio_;
		private boolean abort_ = false;

		/** Thread body. */
		@Override
		public void run() {
			super.run();
			while (true) {
				synchronized (this) {
					if (abort_) {
						break;
					}
					ioio_ = IOIOFactory.create();
				}
				try {
					setText(R.string.wait_ioio);
					ioio_.waitForConnect();
					setText(R.string.ioio_connected);
					DigitalOutput led = ioio_.openDigitalOutput(0, true);
					while (true) {
						led.write(!button_.isChecked());
						sleep(10);
					}
				} catch (ConnectionLostException e) {
				} catch (Exception e) {
					Log.e("HelloIOIOPower", "Unexpected exception caught", e);
					ioio_.disconnect();
					break;
				} finally {
						if (ioio_ != null) {
							try {
								ioio_.waitForDisconnect();
							} catch (InterruptedException e) {
							}
						}
						synchronized (this) {
							ioio_ = null;
						}
				}
			}
		}

		/**
		 * Abort the connection.
		 * 
		 * This is a little tricky synchronization-wise: we need to be handle
		 * the case of abortion happening before the IOIO instance is created or
		 * during its creation.
		 */
		synchronized public void abort() {
			abort_ = true;
			if (ioio_ != null) {
				ioio_.disconnect();
			}
		}

		/**
		 * Set the text line on top of the screen.
		 * 
		 * @param id
		 *            The string ID of the message to present.
		 */
		private void setText(final int id) {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					title_.setText(getString(id));
				}
			});
		}
	}
}