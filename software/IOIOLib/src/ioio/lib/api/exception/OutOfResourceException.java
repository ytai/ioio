package ioio.lib.api.exception;

/**
 * The IOIO board does not have anymore of the requested resource. This
 * exceptions do not need to be handled if the client guarantees that the limits
 * on concurrent resource usage are never exceeded.
 */
public class OutOfResourceException extends RuntimeException {
	private static final long serialVersionUID = -4482605241361881899L;

	public OutOfResourceException(String msg) {
		super(msg);
	}
}