package ioio.lib.spi;

public class Logger {

	public static final int LOG_VERBOSE = 5;
	public static final int LOG_WARN = 4;
	public static final int LOG_DEBUG = 3;
	public static final int LOG_INFO = 2;
	public static final int LOG_ERROR = 1;
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

	public static ILogger log;
	
	public static ILogger addLogBootstrap(String classPath)
	{
		try {
			Logger.log = (ILogger) Class.forName(classPath).newInstance();
		} catch (InstantiationException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
		return Logger.log;
	}
}
