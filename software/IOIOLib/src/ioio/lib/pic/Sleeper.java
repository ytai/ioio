package ioio.lib.pic;

/**
 * Utility class for an exception-less sleep.
 *
 * @author birmiwal
 */
public class Sleeper {
    public static void sleep(int ms) {
    	try {
    		Thread.sleep(ms);
    	} catch (InterruptedException e) {
    		e.printStackTrace();
    	}
    }
}
