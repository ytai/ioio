package ioio.lib.spi;

import android.util.Log;
import ioio.lib.spi.Log.ILogger;

public class LogImpl implements ILogger {

	@Override
	public void write(int level, String tag, String message)
	{
		Log.println(level, tag, message);		
	}
	
}
