package ioio.lib.util.android;

import java.util.Collection;

import android.content.ContextWrapper;
import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.util.IOIOConnectionManager;
import ioio.lib.util.IOIOConnectionRegistry;

/**
 * An extension of {@link IOIOConnectionManager} for the Android environment.
 * <p>
 * <i><b>Note</b>: This utility is not typically intended for use by end-users.
 * See {@link IOIOActivity} and {@link IOIOSerive} for high-level utilities,
 * which facilitate the creation of Android-based applications.</i>
 * <p>
 * The reason for this extension is that some Android-related connections (e.g.
 * OpenAccessory) are tightly coupled with the Android Activity life-cycle and
 * require to be notified when certain life-cycle events occur.
 * <p>
 * Use this class similarly to {@link IOIOConnectionManager}, but in addition
 * call {@link #create()}, {@link #destroy()}, {@link #start()}, {@link #stop()}
 * and {@link #restart()} from your activity's {@code onCreate()},
 * {@code onDestroy()}, {@code onStart()}, {@code onStop()} and
 * {@code onRestart()}, respectively.
 */
public class AndroidIOIOConnectionManager extends IOIOConnectionManager {
	private final ContextWrapper contextWrapper_;
	private Collection<IOIOConnectionBootstrap> bootstraps_ = IOIOConnectionRegistry
			.getBootstraps();

	public AndroidIOIOConnectionManager(ContextWrapper wrapper,
			IOIOConnectionThreadProvider provider) {
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
