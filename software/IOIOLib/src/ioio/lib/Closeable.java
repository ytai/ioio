package ioio.lib;

public interface Closeable {
    /**
     * Call this when the resource is no longer needed.
     * Calling this will make it available in the pool for resources for reallocation.
     */
    public void close();
}
