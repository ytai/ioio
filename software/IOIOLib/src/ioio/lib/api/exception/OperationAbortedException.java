package ioio.lib.api.exception;


/**
 * The pending request has been aborted.
 * java.lang.InterruptedException instead 
 * change abort to interrupt()
 */
public class OperationAbortedException extends  Exception {
    private static final long serialVersionUID = 1L;
    public OperationAbortedException(String msg) {
        super(msg);
    }
}