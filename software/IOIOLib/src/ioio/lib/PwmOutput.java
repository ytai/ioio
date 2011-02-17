package ioio.lib;

import ioio.lib.IOIOException.ConnectionLostException;
import ioio.lib.IOIOException.InvalidStateException;

/**
 * An interface to control the PWM output on an IOIO board.
 *
 * @author birmiwal
 */
public interface PwmOutput extends Closeable {
    /**
     * Sets the duty cycle of the output of the PWM.
     * @param dutyCycle valid values from 0 to 1.0.
     * @throws ConnectionLostException if the connection with the IOIO board is lost
     */
    public void setDutyCycle(float dutyCycle) throws ConnectionLostException, InvalidStateException;

    /**
     * Sets the width of the pulse (high).
     * If the pulseWidthUs is greater than the cycle-time, then an IllegalArgumentException is thrown.
     * @param pulseWidthUs time in micro-secs.
     */
    public void setPulseWidth(int pulseWidthUs) throws ConnectionLostException, InvalidStateException;
}
