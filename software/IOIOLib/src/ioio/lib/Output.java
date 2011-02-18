package ioio.lib;

import ioio.lib.IOIOException.ConnectionLostException;
import ioio.lib.IOIOException.InvalidStateException;

/**
 * An interface for controlling an output pin.
 *
 * Typical usage: (Simplified, no exception handling)
 * Output<Boolean> out = ioio.openDigitalOutput(...);
 * out.write(true);  // set the pin output to HIGH.
 * // enjoy it while it lasts...
 * out.close();  // release the pin.
 *
 * @param <T> the type of output (Boolean for digital output).
 *
 * @author birmiwal
 */
public interface Output<T> extends Closeable {
    /**
     * Write the value to the allocated pin.
     *
     * @param value The value to set.
     * @throws ConnectionLostException in case connection was lost before running this method.
     * @throws InvalidStateException In case the pin has been closed.
     */
    public void write(T value) throws ConnectionLostException, InvalidStateException;

    /**
     * @return The last state written to this pin (using write() method, or the initial value if write() hasn't been called yet).
     * @throws InvalidStateException In case the pin has been closed.
     */
    public T getLastWrittenValue() throws InvalidStateException;
}
