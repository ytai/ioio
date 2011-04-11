package ioio.lib.api.exception;


/**
 * what is this for
 * @author arshan
 *
 */
public class InvalidStateException extends  RuntimeException {
    private static final long serialVersionUID = 1L;
    public InvalidStateException(String msg) {
        super(msg);
    }
}