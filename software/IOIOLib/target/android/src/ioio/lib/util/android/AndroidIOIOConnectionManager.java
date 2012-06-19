package ioio.lib.util.android;

import java.util.Collection;

import android.content.ContextWrapper;
import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.util.IOIOConnectionManager;
import ioio.lib.util.IOIOConnectionRegistry;

public class AndroidIOIOConnectionManager extends IOIOConnectionManager {
	private final ContextWrapper contextWrapper_;
	private Collection<IOIOConnectionBootstrap> bootstraps_ = IOIOConnectionRegistry
			.getBootstraps();
	
	public AndroidIOIOConnectionManager(ContextWrapper wrapper, IOIOConnectionThreadProvider provider) {
		super(provider);
		contextWrapper_ = wrapper;
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
