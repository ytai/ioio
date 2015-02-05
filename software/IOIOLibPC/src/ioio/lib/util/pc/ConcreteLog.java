package ioio.lib.util.pc;

import ioio.lib.spi.Logger;
import ioio.lib.spi.Logger.ILogger;

public class ConcreteLog implements ILogger {

	private int logLevel = Logger.LOG_INFO;

	@Override
	public void i(String tag, String message) {
		if (logLevel >= Logger.LOG_INFO) {
			System.out.println(tag + ": " + message);
		}
	}

	@Override
	public void i(String tag, String message, Throwable exception) {
		if (logLevel >= Logger.LOG_INFO) {
			System.out.println(tag + ": " + message);
			exception.printStackTrace(System.out);
		}
	}

	@Override
	public void e(String tag, String message) {
		if (logLevel >= Logger.LOG_ERROR) {
			System.err.println(tag + ": " + message);
		}
	}

	@Override
	public void e(String tag, String message, Throwable exception) {
		if (logLevel >= Logger.LOG_ERROR) {
			System.err.println(tag + ": " + message);
			exception.printStackTrace(System.err);
		}
	}

	@Override
	public void d(String tag, String message) {
		if (logLevel >= Logger.LOG_DEBUG) {
			System.out.println(tag + ": " + message);
		}

	}

	@Override
	public void d(String tag, String message, Throwable exception) {
		if (logLevel >= Logger.LOG_DEBUG) {
			System.out.println(tag + ": " + message);
			exception.printStackTrace(System.out);
		}
	}

	@Override
	public void w(String tag, String message) {
		if (logLevel >= Logger.LOG_WARN) {
			System.out.println(tag + ": " + message);
		}
	}

	@Override
	public void w(String tag, String message, Throwable exception) {
		if (logLevel >= Logger.LOG_WARN) {
			System.out.println(tag + ": " + message);
			exception.printStackTrace(System.out);
		}
	}

	@Override
	public void v(String tag, String message) {
		if (logLevel >= Logger.LOG_VERBOSE) {
			System.out.println(tag + ": " + message);
		}
	}

	@Override
	public void v(String tag, String message, Throwable exception) {
		if (logLevel >= Logger.LOG_VERBOSE) {
			System.out.println(tag + ": " + message);
			exception.printStackTrace(System.out);
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
