package ioio.lib.spi;

import android.util.Log;
import ioio.lib.spi.Log.ILogger;

public class LogImpl implements ILogger {

	@Override
	public void i(String tag, String message) {
			Log.i(tag, message);
	}

	@Override
	public void i(String tag, String message, Throwable exception) {
			Log.i(tag, message, exception);
	}

	@Override
	public void e(String tag, String message) {
			Log.e(tag, message);
	}

	@Override
	public void e(String tag, String message, Throwable exception) {
			Log.e(tag, message, exception);
	}

	@Override
	public void d(String tag, String message) {
			Log.d(tag, message);
	}

	@Override
	public void d(String tag, String message, Throwable exception) {
			Log.d(tag, message, exception);
	}

	@Override
	public void w(String tag, String message) {
			Log.w(tag, message);
	}

	@Override
	public void w(String tag, String message, Throwable exception) {
			Log.w(tag, message, exception);
	}

	@Override
	public void v(String tag, String message) {
			Log.v(tag, message);
	}

	@Override
	public void v(String tag, String message, Throwable exception) {
			Log.v(tag, message, exception);
	}
}
