package ioio.tests.torture;

import ioio.lib.api.IOIO.VersionType;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.AbstractIOIOActivity;
import ioio.lib.util.AbstractIOIOAdkActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AbstractIOIOAdkActivity {
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
	}

	@Override
	protected AbstractIOIOActivity.IOIOThread createIOIOThread() {
		return new IOIOThread();
	}


	class IOIOThread extends AbstractIOIOActivity.IOIOThread {
		private ResourceAllocator alloc_ = new ResourceAllocator();
		private TestProvider provider_;
		private TestThread[] workers_ = new TestThread[8];
		
		@Override
		protected void setup() throws ConnectionLostException {
			provider_ = new TestProvider(MainActivity.this, ioio_,
					alloc_);
			showVersions();
			//Debug.startMethodTracing();
			for (int i = 0; i < workers_.length; ++i) {
				workers_[i] = new TestThread(provider_);
				workers_[i].start();
			}
			try {
				ioio_.waitForDisconnect();
			} catch (InterruptedException e) {
			}
			for (int i = 0; i < workers_.length; ++i) {
				if (workers_[i] != null) {
					workers_[i].interrupt();
					try {
						workers_[i].join();
					} catch (InterruptedException e) {
					}
				}
			}
			throw new ConnectionLostException();
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
	}
}
