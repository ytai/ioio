package ioio.lib.spi;

public class Log {

	public static final int LOG_VERBOSE = 2;
	public static final int LOG_WARN = 5;
	public static final int LOG_DEBUG = 3;
	public static final int LOG_INFO = 4;
	public static final int LOG_ERROR = 6;
	public static final int LOG_NONE = 0;

	public interface ILogger {

		/** Logs a message to the console or logcat */
		public void i(String tag, String message);

		/** Logs a message to the console or logcat */
		public void i(String tag, String message, Throwable exception);

		/** Logs an error message to the console or logcat */
		public void e(String tag, String message);

		/** Logs an error message to the console or logcat */
		public void e(String tag, String message, Throwable exception);

		/** Logs a debug message to the console or logcat */
		public void d(String tag, String message);

		/** Logs a debug message to the console or logcat */
		public void d(String tag, String message, Throwable exception);
		
		/** Logs a debug message to the console or logcat */
		public void w(String tag, String message);

		/** Logs a debug message to the console or logcat */
		public void w(String tag, String message, Throwable exception);
		
		/** Logs a debug message to the console or logcat */
		public void v(String tag, String message);
		
		/** Logs a debug message to the console or logcat */
		public void v(String tag, String message, Throwable exception);

		/**
		 * Sets the log level. {@link #LOG_NONE} will mute all log output.
		 * 
		 * @param logLevel
		 *            {@link #LOG_NONE}.
		 */
		public void setLogLevel(int logLevel);

		/** Gets the log level. */
		public int getLogLevel();

	}
	
	private static ILogger log_;
	
	public static void i(String tag, String message) {
		if (log_.getLogLevel() >= Log.LOG_INFO) {
			log_.i(tag, message);
		}
	}

	
	public static void i(String tag, String message, Throwable exception) {
		if (log_.getLogLevel() >= Log.LOG_INFO) {
			log_.i(tag, message, exception);
		}
	}


	public static void e(String tag, String message) {
		if (log_.getLogLevel() >= Log.LOG_ERROR) {
			log_.e(tag, message);
		}
	}

	public static void e(String tag, String message, Throwable exception) {
		if (log_.getLogLevel() >= Log.LOG_ERROR) {
			log_.e(tag, message, exception);
		}
	}

	
	public static void d(String tag, String message) {
		if (log_.getLogLevel() >= Log.LOG_DEBUG) {
			log_.d(tag, message);
		}

	}

	
	public static void d(String tag, String message, Throwable exception) {
		if (log_.getLogLevel() >= Log.LOG_DEBUG) {
			log_.d(tag, message, exception);
		}
	}

	
	public static void w(String tag, String message) {
		if (log_.getLogLevel()>= Log.LOG_WARN) {
			log_.w(tag, message);
		}
	}

	
	public static void w(String tag, String message, Throwable exception) {
		if (log_.getLogLevel() >= Log.LOG_WARN) {
			log_.w(tag, message, exception);
		}
	}

	public static void v(String tag, String message) {
		if (log_.getLogLevel() >= Log.LOG_VERBOSE) {
			log_.v(tag, message);
		}
	}

	public static void v(String tag, String message, Throwable exception) {
		if (log_.getLogLevel() >= Log.LOG_VERBOSE) {
			log_.v(tag, message, exception);
		}
	}
	
	public static void setLogLevel(int logLevel) {
		log_.setLogLevel(logLevel);
	}

	public static int getLogLevel() {
		return log_.getLogLevel();
	}

	
	public static void addLogBootstrap(String classPath)
	{
		try {
			Log.log_ = (ILogger) Class.forName(classPath).newInstance();
		} catch (InstantiationException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
	}

}
