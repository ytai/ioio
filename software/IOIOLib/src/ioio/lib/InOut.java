package ioio.lib;

/**
 * An interface that provides input/output from/to the IOIO board.
 * @param <T> the type of IO
 *
 * @author birmiwal
 */
public interface InOut<T> extends Input<T>, Output<T> { }
