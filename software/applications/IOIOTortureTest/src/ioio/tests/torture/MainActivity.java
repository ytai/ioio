package ioio.tests.torture;

import ioio.lib.api.IOIO;
import ioio.lib.api.IOIO.VersionType;
import ioio.lib.api.IOIOFactory;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends Activity {
	private IOIOThread ioio_thread_;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
	}

	@Override
	protected void onResume() {
		super.onResume();
		ioio_thread_ = new IOIOThread();
		ioio_thread_.start();
	}

	@Override
	protected void onPause() {
		super.onPause();
		ioio_thread_.abort();
		try {
			ioio_thread_.join();
		} catch (InterruptedException e) {
		}
	}

	class IOIOThread extends Thread {
		private IOIO ioio_;
		private boolean abort_ = false;
		private ResourceAllocator alloc_ = new ResourceAllocator();
		private TestProvider provider_;
		private TestThread[] workers_ = new TestThread[4];

		@Override
		public void run() {
			super.run();
			while (true) {
				synchronized (this) {
					if (abort_) {
						break;
					}
					ioio_ = IOIOFactory.create();
					provider_ = new TestProvider(MainActivity.this, ioio_,
							alloc_);
				}
				try {
					ioio_.waitForConnect();
					showVersions();
					//Debug.startMethodTracing();
					for (int i = 0; i < workers_.length; ++i) {
						workers_[i] = new TestThread(provider_);
						workers_[i].start();
					}
				} catch (ConnectionLostException e) {
				} catch (IncompatibilityException e) {
					Log.e("TortureTest", "Incompatibility detected", e);
					break;
				} catch (Exception e) {
					Log.e("TortureTest", "Unexpected exception caught", e);
					ioio_.disconnect();
					break;
				} finally {
					//Debug.stopMethodTracing();
					try {
						ioio_.waitForDisconnect();
						Log.i("TortureTest",
								"IOIO disconnected, killing workers");
						for (int i = 0; i < workers_.length; ++i) {
							if (workers_[i] != null) {
								workers_[i].interrupt();
								workers_[i].join();
							}
						}
						Log.i("TortureTest", "All workers dead");
					} catch (InterruptedException e) {
					}
				}
			}
		}

		private void showVersions() throws ConnectionLostException {
			final String versionText = "hw: "
					+ ioio_.getImplVersion(VersionType.HARDWARE_VER)
					+ "\n"
					+ "bl: "
					+ ioio_.getImplVersion(VersionType.BOOTLOADER_VER)
					+ "\n"
					+ "fw: "
					+ ioio_.getImplVersion(VersionType.APP_FIRMWARE_VER)
					+ "\n" + "lib: "
					+ ioio_.getImplVersion(VersionType.IOIOLIB_VER);
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					((TextView) findViewById(R.id.versions)).setText(versionText);
				}
			});
		}

		synchronized public void abort() {
			abort_ = true;
			if (ioio_ != null) {
				ioio_.disconnect();
			}
			interrupt();
		}
	}
}
