package ioio.util;

import android.app.Activity;

/**
 * a cheap hack of a class to make an android app appear a bit like an arduino app
 * 
 * ok, this could be _really_ cool, if we bring in processing classes too, support
 * to screen things like ... 
 * 
 * text(...)
 * circle(...) 
 * etc.
 * 
 * @author arshan
 *
 */
public class ArduinoSyntax extends Activity{
    
    public void onCreate() {
        setup();
    }

    
    public static int OUTPUT = 3;
    public static int INPUT = 2;
    // make these a class BoolInt that has a toInt() method that makes sense
    public static boolean HIGH = true;
    public static boolean LOW = false;
    
    public void pinMode(int pin, int mode){}
    
    public void digitalWrite(int pin, boolean val) {}
    
    public boolean digitalRead(int pin) { return HIGH;}
    
    public void onStart() {
        while (true) {
            loop();
            // put a safety throttle here.
        }
    }

    
    // These get overridden where appropriate
    public void setup() {}
    
    public void loop() {}
    
    // Looks weird but remember we're trying to hide java where we can.
    public void delay(long ms) {
        long started_at = System.currentTimeMillis();
        long remaining = ms;
        while (ms > System.currentTimeMillis() - started_at) {
            try {
                Thread.sleep(ms);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
    
    
    public void setupScrollingText() {
        
    }
    
    public void text(String text) {
        
    }
    
    /**
     * see the docs, TBI
     * @author arshan
     *
     */
    public class Serial {
        
        Serial() {}
        
    }
}
