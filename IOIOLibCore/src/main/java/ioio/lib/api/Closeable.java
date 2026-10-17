package ioio.lib.api;

/**
 * A resource that can be closed to release any associated IOIO-side state.
 */
public interface Closeable {
    /**
     * Closes this resource and frees any associated resources.
     */
    void close();
}
