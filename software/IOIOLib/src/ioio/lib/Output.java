package ioio.lib;

import ioio.lib.IOIOException.ConnectionLostException;
import ioio.lib.IOIOException.InvalidStateException;

/**
 * An interface for a resource that provides output to the IOIO board.
 * @param <T> the type of output
 *
 * @author birmiwal
 */
public interface Output<T> extends Closeable {
    public void write(T value) throws ConnectionLostException, InvalidStateException;
    public T getLastWrittenValue() throws InvalidStateException;
}
