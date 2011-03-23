package ioio.api;

/**
 * Simple set of exceptions to be used by the Ioio interface.
 *
 * @author arshan
 */
public class PeripheralException extends Exception {

	public PeripheralException() {
		super("PeripheralException");
	}

	public PeripheralException(String msg) {
		super(msg);
	}

	/**
	 * The connection between android and the IOIO has been lost.
	 */
	public static class ConnectionLostException extends PeripheralException {
	    public ConnectionLostException(String msg) {
	        super(msg);
        }
	}

	/**
	 * The pending request has been aborted.
	 * java.lang.InterruptedException instead 
	 * change abort to interrupt()
	 */
    public static class OperationAbortedException extends PeripheralException {
        public OperationAbortedException(String msg) {
            super(msg);
        }
    }

    /**
     * The firmware version is not supported by IOIOLib.
     */
    public static class UnsupportedBoardException extends PeripheralException {
        public UnsupportedBoardException(String msg) {
            super(msg);
        }
    }

    /**
     * what is this for
     * @author arshan
     *
     */
    public static class InvalidStateException extends PeripheralException {
        public InvalidStateException(String msg) {
            super(msg);
        }
    }

    /**
     * The IOIO firmware does not have anymore of the requested resource.
     */
    public static class OutOfResourceException extends PeripheralException {
        public OutOfResourceException(String msg) {
            super(msg);
        }
    }

    /**
     * InvalidStateException
     */
    public static class InvalidOperationException extends PeripheralException {
        public InvalidOperationException(String msg) {
            super(msg);
        }
    }

}
