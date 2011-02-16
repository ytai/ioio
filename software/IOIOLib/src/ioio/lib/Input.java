package ioio.lib;

import ioio.lib.IOIOException.ConnectionLostException;

public interface Input<T> extends Closeable {
    public T read() throws ConnectionLostException;
}
