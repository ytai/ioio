package ioio.lib.spi;

import ioio.lib.spi.Log.ILogger;

public class LogImpl implements ILogger {

	private static final char[] LEVELS = { '0', '1', 'V', 'D', 'I', 'W', 'E', 'F' };
	
	@Override
	public void write(int level, String tag, String message)
	{
		System.err.println("[" + LEVELS[level] + "/" + tag + "] " + message);
	}

}
