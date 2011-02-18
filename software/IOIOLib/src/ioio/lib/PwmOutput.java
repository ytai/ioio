package ioio.lib;

import ioio.lib.IOIOException.ConnectionLostException;
import ioio.lib.IOIOException.InvalidStateException;

/**
 * An interface to control the PWM output on a IOIO board.
 *
 * Typical usage: (Simplified, no exception handling)
 * PwmOutput pwmOutput = ioio.openPwmOutput(...);
 * pwmOutput.setPulseWidth(15000);
 * // Use pwm for a while...
 * pwmOutput.close();  // release the resource.
 *
 * @author birmiwal
 */
public interface PwmOutput extends Closeable {
    /**
     * Sets the duty cycle of the output of the PWM.
     *
     * This is identical of calling setPulseWidth with (pulseWidthUs = 1000000*dutyCycle/freqHz)
     *
     * @param dutyCycle valid values from 0.0 to 1.0.
     * @throws ConnectionLostException in case connection was lost before running this method.
     * @throws InvalidStateException In case the pin has been closed.
     */
    public void setDutyCycle(float dutyCycle) throws ConnectionLostException, InvalidStateException;

    /**
     * Sets the width of the pulse (high) in micro seconds.
     *
     * This is identical of calling setDutyCycle with (dutyCycle = pulseWidthUs*freqHz/1000000)
     * If the pulseWidthUs is greater than the cycle-time, then an IllegalArgumentException is thrown.
     *
     * @param pulseWidthUs time in micro seconds.
     * @throws ConnectionLostException in case connection was lost before running this method.
     * @throws InvalidStateException In case the pin has been closed.
     */
    public void setPulseWidth(int pulseWidthUs) throws ConnectionLostException, InvalidStateException;
}
