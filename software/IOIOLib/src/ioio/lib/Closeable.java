package ioio.lib;

/**
 * An interface for a resource that needs to be closed to be freed. It is necessary to call {@link #close()}
 * for the resource to be available for reallocation later.
 *
 * @author birmiwal
 */
public interface Closeable {
    /**
     * Call this when the resource is no longer needed.
     * Calling this will make it available in the pool for resources for reallocation.
     * This method will do nothing if pin is already closed or invalidated (by disconnection).
     */
    public void close();
}
