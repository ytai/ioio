package ioio.lib;

// ytai: probably want a few of those: disconnection, abortion, unsupported board.
public class IOIOException extends Exception {

	public IOIOException() {
		super("IOIOException");
	}
	
	public IOIOException(String msg) {
		super(msg);
	}

	/*
	 * 
	 */
	private static final long serialVersionUID = 2372683427743695278L;

}
