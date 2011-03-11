package ioio.lib;


/**
 * Utilities for pins on IOIO Board.
 *
 * @author birmiwal
 */
public class Pins {

    /**
     * Returns the pin number which provides access to the LED pin.
     *
     * @return the pin number; use this for {@link Ioio#openDigitalOutput(int, boolean)}
     */
    public int getPinForOnBoardLED(Ioio ioio) {
        return 0;
    }
}
