package ioio.util;

import ioio.lib.IoioException.ConnectionLostException;
import ioio.lib.IoioException.InvalidStateException;
import ioio.lib.pic.DigitalOutput;

/**
 * Simple output pin wrapper for a single color LED.
 * TODO(arshan): add the ctor that uses PwmOutput for a var drive Led
 * TODO(arshan): 3 color LEDs too :)
 * @author arshan
 */
public class Led {

    DigitalOutput state;
    
    public void Led(DigitalOutput pin) {
        state = pin;
    }
    
    public void setState(boolean val) {
        try {
            state.write(val);
        } catch (ConnectionLostException e) {
           
        } catch (InvalidStateException e) {
           
        }
    }
}
