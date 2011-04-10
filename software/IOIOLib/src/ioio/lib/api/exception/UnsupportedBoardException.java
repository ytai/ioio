package ioio.lib.api.exception;


/**
 * The firmware version is not supported by IOIOLib.
 */
public class UnsupportedBoardException extends  Exception {
    private static final long serialVersionUID = 1L;
    public UnsupportedBoardException(String msg) {
        super(msg);
    }
}