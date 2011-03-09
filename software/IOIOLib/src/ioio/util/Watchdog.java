package ioio.util;

import ioio.lib.IoioFactory;
import ioio.lib.pic.IoioImpl;
import dalvik.system.VMRuntime;
import android.util.Log;

/**
 * Watchdog utility class
 * 
 * Watchdog wd = new Watchdog();
 * 
 * wd.timer(2000);
 * doMyPotentiallyFreezingTask();
 * wd.cancel();
 * 
 * @author arshan
 */

public class Watchdog {

    private Thread thr;
    private Timer timer;
    
    public static final int ABORT = 0;
    public static final int INTERRUPT_THREAD = 1;
    public static final int STOP_THREAD = 2;
    public static final int EXIT_VM = 3;
    
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
     * Will start a count down for the indicated milliseconds, and then 
     * send the abort signal unless cancel is called.
     * @param ms
     */
    public void timer(long ms) {
        timer = new Timer(ms, 0);
        timer.start();
    }
    
    // Not sure the ns makes a difference on android ... but there you go.
    public void timer(long ms, int ns) {
        timer = new Timer(ms, ns);
        timer.start();
    }
    
    public void cancel() {
        if (timer == null) return;
        synchronized (timer) {
            timer.notifyAll();
            timer = null;
        }
    }
    
    protected class Timer extends Thread {
       
        long ms;
        int ns;
        
        public Timer(long ms, int ns) {
            setPriority(MAX_PRIORITY);
            this.ms = ms;          
            this.ns = ns;
        }
        
        public void run() {
            try {
                sleep(ms, ns);
                switch (mode) {
                    case ABORT:
                        IoioFactory.makeIoio().abortConnection();
                        break;
                    case INTERRUPT_THREAD:
                        thr.interrupt(); // this probably wont work
                        break;
                    case STOP_THREAD: // soooo deprecated
                        thr.stop();
                        break;
                    case EXIT_VM:
                        // TODO: meaningful exit code
                        Runtime.getRuntime().exit(1);
                        break;
                    
                }                           
            } catch (InterruptedException e) {
              
            }            
        }
    }
}
