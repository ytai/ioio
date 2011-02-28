package ioio.lib;

/**
 * Simple set of exceptions to be used by the Ioio interface.
 *
 * @author arshan
 */
public class IoioException extends Exception {

	public IoioException() {
		super("IOIOException");
	}

	public IoioException(String msg) {
		super(msg);
	}

	public static class ConnectionLostException extends IoioException {
	    public ConnectionLostException(String msg) {
	        super(msg);
        }
	}

    public static class OperationAbortedException extends IoioException {
        public OperationAbortedException(String msg) {
            super(msg);
        }
    }

    public static class UnsupportedBoardException extends IoioException {
        public UnsupportedBoardException(String msg) {
            super(msg);
        }
    }

    public static class InvalidStateException extends IoioException {
        public InvalidStateException(String msg) {
            super(msg);
        }
    }

    public static class OutOfResourceException extends IoioException {
        public OutOfResourceException(String msg) {
            super(msg);
        }
    }

    public static class InvalidOperationException extends IoioException {
        public InvalidOperationException(String msg) {
            super(msg);
        }
    }

    // TODO(arshan): why arent we using java's socketexception? 
    public static class SocketException extends IoioException {
        public SocketException(String msg) {
            super(msg);
        }
    }
}
