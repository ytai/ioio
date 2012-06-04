package ioio.lib.spi;

public class Log {
	public static int println(int priority, String tag, String msg)  {
		return android.util.Log.println(priority, tag, msg);
	}
	
	public static void e(String tag, String message) {
		android.util.Log.e(tag, message);
	}

	public static void e(String tag, String message, Throwable tr) {
		android.util.Log.e(tag, message, tr);
	}

	public static void i(String tag, String message) {
		android.util.Log.i(tag, message);
	}

	public static void d(String tag, String message) {
		android.util.Log.d(tag, message);
	}

	public static void v(String tag, String message) {
		android.util.Log.v(tag, message);
	}
}
