package ioio.lib.util.android;

import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.util.IOIOApplicationHelper;
import ioio.lib.util.IOIOConnectionRegistry;
import ioio.lib.util.IOIOLooperProvider;

import android.content.ContextWrapper;

public class IOIOAndroidApplicationHelper extends IOIOApplicationHelper {
	private final ContextWrapper contextWrapper_;

	public IOIOAndroidApplicationHelper(ContextWrapper wrapper,
			IOIOLooperProvider provider) {
		super(provider);
		contextWrapper_ = wrapper;
	}

	static {
		IOIOConnectionRegistry
				.addBootstraps(new String[] {
						"ioio.lib.android.accessory.AccessoryConnectionBootstrap",
						"ioio.lib.android.bluetooth.BluetoothIOIOConnectionBootstrap" });
	}

	public void create() {
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			if (bootstrap instanceof ContextWrapperDependent) {
				((ContextWrapperDependent) bootstrap).onCreate(contextWrapper_);
			}
		}
	}

	public void destroy() {
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			if (bootstrap instanceof ContextWrapperDependent) {
				((ContextWrapperDependent) bootstrap).onDestroy();
			}
		}
	}

	@Override
	public void start() {
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			if (bootstrap instanceof ContextWrapperDependent) {
				((ContextWrapperDependent) bootstrap).open();
			}
		}
		super.start();
	}

	@Override
	public void stop() {
		super.stop();
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			if (bootstrap instanceof ContextWrapperDependent) {
				((ContextWrapperDependent) bootstrap).close();
			}
		}
	}

	public void restart() {
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			if (bootstrap instanceof ContextWrapperDependent) {
				((ContextWrapperDependent) bootstrap).reopen();
			}
		}
	}
}
