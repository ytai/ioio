package ioio.lib.pic;

import android.util.Log;

/**
 * Utility logger.
 *
 * @author birmiwal
 *
 */
public class IoioLogger {
    static final String TAG = "IOIO";
    static long last_log = 0;

    public static void log(String msg) {
    	long current = System.currentTimeMillis();
    	Log.i(IoioLogger.TAG, msg + " @" + current + " (+" +(current-last_log)+ ")");
    	last_log = current;
    }
}
