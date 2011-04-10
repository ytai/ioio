package ioio.api.exception;

/**
 * InvalidStateException
 */
public class InvalidOperationException extends  Exception {
    private static final long serialVersionUID = 1L;
    public InvalidOperationException(String msg) {
        super(msg);
    }
}