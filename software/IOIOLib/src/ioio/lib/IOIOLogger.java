package ioio.lib;

import android.util.Log;

/**
 * Utility logger.
 *
 * @author birmiwal
 *
 */
public class IOIOLogger {
    static final String TAG = "IOIO";
    static long last_log = 0;

    public static void log(String msg) {
    	long current = System.currentTimeMillis();
    	Log.i(IOIOLogger.TAG, msg + " @" + current + " (+" +(current-last_log)+ ")");
    	last_log = current;
    }
}
