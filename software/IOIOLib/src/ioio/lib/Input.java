package ioio.lib;

import ioio.lib.IoioException.ConnectionLostException;
import ioio.lib.IoioException.InvalidStateException;

import java.io.Closeable;

/**
 * An interface for reading from an input pin.
 *
 * Typical usage: (Simplified, no exception handling)
 * Input<Float> analogInput = ioio.openAnalogInput(...);
 * float potentiometerValue = analogInput.read();
 * Input<Boolean> digitalInput = ioio.openDigitalInput(...);
 * boolean switchValue = digitalInput.read();
 * // play with them...
 * analogInput.close();  // release the pins.
 * digitalInput.close();
 *
 * @param <T> the type of input
 *
 * @author birmiwal
 */
public interface Input<T> extends Closeable {
    /**
     * @return The current value of the pin.
     * @throws ConnectionLostException in case connection was lost before running this method.
     * @throws InvalidStateException In case the pin has been closed.
     */
    public T read() throws ConnectionLostException, InvalidStateException;
}
