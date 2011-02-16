package ioio.lib;

import ioio.lib.IOIOException.ConnectionLostException;

/**
 * An interface for a resource that provides output to the IOIO board.
 * @param <T> the type of output
 *
 * @author birmiwal
 */
public interface Output<T> extends Closeable {
    public void write(T value) throws ConnectionLostException;
}
