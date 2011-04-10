package ioio.api.exception;


/**
 * The IOIO firmware does not have anymore of the requested resource.
 */
public class OutOfResourceException extends  Exception {
    private static final long serialVersionUID = 1L;
    public OutOfResourceException(String msg) {
        super(msg);
    }
}