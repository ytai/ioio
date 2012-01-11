package ioio.tests.torture;

import ioio.lib.android.AbstractIOIOActivity;
import ioio.lib.api.IOIO.VersionType;
import ioio.lib.api.exception.ConnectionLostException;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends AbstractIOIOActivity {
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
	}

	class IOIOThread extends AbstractIOIOActivity.IOIOThread {
		private static final String TAG = "TortureTest";
		private ResourceAllocator alloc_ = new ResourceAllocator();
		private TestProvider provider_;
		private TestThread[] workers_ = new TestThread[8];

		@Override
		protected void setup() throws ConnectionLostException,
				InterruptedException {
			provider_ = new TestProvider(MainActivity.this, ioio_, alloc_);
			showVersions();
			for (int i = 0; i < workers_.length; ++i) {
				workers_[i] = new TestThread(provider_);
				workers_[i].start();
			}
		}

		@Override
		protected void loop() throws ConnectionLostException,
				InterruptedException {
		}

		@Override
		protected void disconnected() {
			Log.i(TAG, "IOIO disconnected, killing workers");
			for (int i = 0; i < workers_.length; ++i) {
				if (workers_[i] != null) {
					workers_[i].interrupt();
				}
			}
			try {
				for (int i = 0; i < workers_.length; ++i) {
					if (workers_[i] != null) {
						workers_[i].join();
					}
				}
				Log.i(TAG, "All workers dead");
			} catch (InterruptedException e) {
				Log.w(TAG, "Interrupted. Some workers may linger.");
			}
		}

		@Override
		protected void incompatible() {
			Log.e(TAG, "Incompatibility detected");
		}

		private void showVersions() throws ConnectionLostException {
			final String versionText = "hw: "
					+ ioio_.getImplVersion(VersionType.HARDWARE_VER) + "\n"
					+ "bl: " + ioio_.getImplVersion(VersionType.BOOTLOADER_VER)
					+ "\n" + "fw: "
					+ ioio_.getImplVersion(VersionType.APP_FIRMWARE_VER) + "\n"
					+ "lib: " + ioio_.getImplVersion(VersionType.IOIOLIB_VER);
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					((TextView) findViewById(R.id.versions))
							.setText(versionText);
				}
			});
		}
	}

	@Override
	protected IOIOThread createIOIOThread() {
		return new IOIOThread();
	}
}
