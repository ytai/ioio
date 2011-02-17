package ioio.lib;

import ioio.lib.IOIOException.ConnectionLostException;
import ioio.lib.IOIOException.InvalidStateException;

/**
 * An interface for a resource that provides input from the IOIO board.
 * @param <T> the type of input
 *
 * @author birmiwal
 */
public interface Input<T> extends Closeable {
    public T read() throws ConnectionLostException, InvalidStateException;
}
