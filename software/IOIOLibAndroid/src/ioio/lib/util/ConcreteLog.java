package ioio.lib.util;

import android.util.Log;
import ioio.lib.spi.Logger;
import ioio.lib.spi.Logger.ILogger;

public class ConcreteLog implements ILogger {

	private int logLevel = Logger.LOG_INFO;

	@Override
	public void i(String tag, String message) {
		if (logLevel >= Logger.LOG_INFO) {
			Log.i(tag, message);
		}
	}

	@Override
	public void i(String tag, String message, Throwable exception) {
		if (logLevel >= Logger.LOG_INFO) {
			Log.i(tag, message, exception);
		}
	}

	@Override
	public void e(String tag, String message) {
		if (logLevel >= Logger.LOG_ERROR) {
			Log.e(tag, message);
		}
	}

	@Override
	public void e(String tag, String message, Throwable exception) {
		if (logLevel >= Logger.LOG_ERROR) {
			Log.e(tag, message, exception);
		}
	}

	@Override
	public void d(String tag, String message) {
		if (logLevel >= Logger.LOG_DEBUG) {
			Log.d(tag, message);
		}

	}

	@Override
	public void d(String tag, String message, Throwable exception) {
		if (logLevel >= Logger.LOG_DEBUG) {
			Log.d(tag, message, exception);
		}
	}

	@Override
	public void w(String tag, String message) {
		if (logLevel >= Logger.LOG_WARN) {
			Log.w(tag, message);
		}
	}

	@Override
	public void w(String tag, String message, Throwable exception) {
		if (logLevel >= Logger.LOG_WARN) {
			Log.w(tag, message, exception);
		}
	}

	@Override
	public void v(String tag, String message) {
		if (logLevel >= Logger.LOG_VERBOSE) {
			Log.v(tag, message);
		}
	}

	@Override
	public void v(String tag, String message, Throwable exception) {
		if (logLevel >= Logger.LOG_VERBOSE) {
			Log.v(tag, message, exception);
		}
	}

	@Override
	public void setLogLevel(int logLevel) {
		this.logLevel = logLevel;
	}

	@Override
	public int getLogLevel() {
		return this.logLevel;
	}
}
