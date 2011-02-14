package ioio.lib;

/**
 * Interface for the IOIO proto.
 * 
 * ytai: probably want a few of those: disconnection, abortion, unsupported board. 
 * arshan: agreed, will add as needed, wanted to get basic functions in first.
 * 
 * TODO(TF): flesh out exceptions
 * 
 * @author arshan
 */
public class IOIOException extends Exception {

	private static final long serialVersionUID = 2372683427743695278L;
	
	public IOIOException() {
		super("IOIOException");
	}
	
	public IOIOException(String msg) {
		super(msg);
	}
}
