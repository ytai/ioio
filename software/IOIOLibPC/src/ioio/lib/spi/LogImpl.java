package ioio.lib.spi;

import ioio.lib.spi.Log;
import ioio.lib.spi.Log.ILogger;

public class LogImpl implements ILogger {

	private int logLevel = Log.LOG_INFO;

	@Override
	public void i(String tag, String message) {
		if (logLevel >= Log.LOG_INFO) {
			System.out.println(tag + ": " + message);
		}
	}

	@Override
	public void i(String tag, String message, Throwable exception) {
		if (logLevel >= Log.LOG_INFO) {
			System.out.println(tag + ": " + message);
			exception.printStackTrace(System.out);
		}
	}

	@Override
	public void e(String tag, String message) {
		if (logLevel >= Log.LOG_ERROR) {
			System.err.println(tag + ": " + message);
		}
	}

	@Override
	public void e(String tag, String message, Throwable exception) {
		if (logLevel >= Log.LOG_ERROR) {
			System.err.println(tag + ": " + message);
			exception.printStackTrace(System.err);
		}
	}

	@Override
	public void d(String tag, String message) {
		if (logLevel >= Log.LOG_DEBUG) {
			System.out.println(tag + ": " + message);
		}

	}

	@Override
	public void d(String tag, String message, Throwable exception) {
		if (logLevel >= Log.LOG_DEBUG) {
			System.out.println(tag + ": " + message);
			exception.printStackTrace(System.out);
		}
	}

	@Override
	public void w(String tag, String message) {
		if (logLevel >= Log.LOG_WARN) {
			System.out.println(tag + ": " + message);
		}
	}

	@Override
	public void w(String tag, String message, Throwable exception) {
		if (logLevel >= Log.LOG_WARN) {
			System.out.println(tag + ": " + message);
			exception.printStackTrace(System.out);
		}
	}

	@Override
	public void v(String tag, String message) {
		if (logLevel >= Log.LOG_VERBOSE) {
			System.out.println(tag + ": " + message);
		}
	}

	@Override
	public void v(String tag, String message, Throwable exception) {
		if (logLevel >= Log.LOG_VERBOSE) {
			System.out.println(tag + ": " + message);
			exception.printStackTrace(System.out);
		}
	}
}
