package util;

import ioio.lib.IoioFactory;
import ioio.lib.pic.IoioImpl;
import dalvik.system.VMRuntime;
import android.util.Log;

/**
 * Watchdog utility class
 * @author arshan
 */

public class Watchdog {

    Thread thr;
    Timer timer;
    
    public static final int ABORT = 0;
    public static final int INTERRUPT_THREAD = 1;
    public static final int EXIT_VM = 2;
    
    int mode = ABORT;
    
    public Watchdog() {
        init();
    }
    
    public Watchdog(int mode) {
        this.mode = mode;
        init();
    }
    
    private void init() {
        thr = Thread.currentThread();
    }
    
    
    /**
     * will 
     * @param ms
     */
    public void timer(long ms) {
        timer = new Timer(ms,thr);
        timer.start();
    }
    
    public void cancel() {
        synchronized (timer) {
            timer.notifyAll();
        }
    }
    
    protected class Timer extends Thread {
        Thread target;
        long time;
        
        public Timer(long ms, Thread thr) {
            this.time = ms;
            this.target = thr;
        }
        
        public void run() {
            try {
                sleep(time);
                switch (mode) {
                    case ABORT:
                        // TODO(arshan): the getinstance should be at ioio.lib.X
                        IoioFactory.makeIoio().abortConnection();
                        break;
                    case INTERRUPT_THREAD:
                        thr.interrupt(); // this probably wont work
                        break;
                    case EXIT_VM:
                        // TODO: meaningful exit code
                        Runtime.getRuntime().exit(1);
                        break;
                    
                }                           
            } catch (InterruptedException e) {
                // nothing we got cancelled.
            }            
        }
    }
}
