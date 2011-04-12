package ioio.lib.api.exception;

/**
 * The IOIO firmware does not have anymore of the requested resource.
 */
public class OutOfResourceException extends RuntimeException {
	private static final long serialVersionUID = -4482605241361881899L;

	public OutOfResourceException(String msg) {
        super(msg);
    }
}