package ioio.lib.spi;

import java.io.PrintWriter;
import java.io.StringWriter;

public class Log
{

	public static final int VERBOSE = 2;
	public static final int WARN = 5;
	public static final int DEBUG = 3;
	public static final int INFO = 4;
	public static final int ERROR = 6;
	public static final int NONE = 0;
	public static final int ASSERT = 7;

	public interface ILogger
	{
		public void write(int level, String tag, String message);
	}

	private static ILogger log_;

	public static void i(String tag, String message)
	{
		write(INFO, tag, message);
	}

	public static void i(String tag, String message, Throwable exception)
	{
		write(INFO, tag, message, exception);
	}

	public static void e(String tag, String message)
	{
		write(ERROR, tag, message);
	}

	public static void e(String tag, String message, Throwable exception)
	{
		write(ERROR, tag, message, exception);
	}

	public static void d(String tag, String message)
	{
		write(DEBUG, tag, message);
	}

	public static void d(String tag, String message, Throwable exception)
	{
		write(DEBUG, tag, message, exception);
	}

	public static void w(String tag, String message)
	{
		write(WARN, tag, message);
	}

	public static void w(String tag, String message, Throwable exception)
	{
		write(WARN, tag, message, exception);
	}

	public static void v(String tag, String message)
	{
		write(VERBOSE, tag, message);
	}

	public static void v(String tag, String message, Throwable exception)
	{
		write(VERBOSE, tag, message, exception);
	}
	
	private static void write(int level, String tag, String message) {
		log_.write(level, tag, message);
	}

	private static void write(int level, String tag, String message, Throwable exception) {
		StringWriter writer = new StringWriter();
		PrintWriter printWriter = new PrintWriter(writer);
		printWriter.println(message);
		exception.printStackTrace(new PrintWriter(writer));
		writer.flush();
		write(level, tag, writer.toString());
	}

	static
	{

		try
		{
			Log.log_ = (ILogger) Class.forName("ioio.lib.spi.LogImpl").newInstance();
		}
		catch (Exception e)
		{
			throw new RuntimeException("Cannot instantiate the LogImpl class. This is likely a result of failing to " 
									+ "include a proper platform-specific IOIOLib* library.", e);
		}

	}

}
