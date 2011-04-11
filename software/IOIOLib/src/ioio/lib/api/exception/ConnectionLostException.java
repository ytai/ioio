package ioio.lib.api.exception;


/**
 * The connection between android and the IOIO has been lost.
 */
public class ConnectionLostException extends Exception {
    private static final long serialVersionUID = 1L;

    public ConnectionLostException(String msg) {
        super(msg);
    }
    
    public ConnectionLostException(Exception e) {
    	super(e);
    }

	public ConnectionLostException() {
		super("Connection lost");
	}
}