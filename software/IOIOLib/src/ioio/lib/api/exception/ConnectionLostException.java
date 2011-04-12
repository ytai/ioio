package ioio.lib.api.exception;


/**
 * The connection between Android and the IOIO has been lost.
 */
public class ConnectionLostException extends Exception {
	private static final long serialVersionUID = 7422862446246046772L;

	public ConnectionLostException(Exception e) {
    	super(e);
    }

	public ConnectionLostException() {
		super("Connection lost");
	}
}