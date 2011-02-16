package ioio.lib;

import ioio.lib.IOIOException.ConnectionLostException;

public interface Output<T> extends Closeable {
    public void write(T value) throws ConnectionLostException;
}
