package ioio.lib.pic;
/**
 * A packet to or from the IOIO protocol.
 *
 * @author arshan
 */
public class IOIOPacket {

	public final int message;
	public final byte[] payload;

	public IOIOPacket(int message, byte[] payload) {
		this.message = message;
		if (payload == null ) {
			// better as 0 length array?
			this.payload = null;
		}
		else {
			this.payload = new byte[payload.length];
			System.arraycopy(payload, 0, this.payload, 0, this.payload.length);
		}
	}

	@Override
	public boolean equals(Object obj) {
	    if (!(obj instanceof IOIOPacket)) {
	        return false;
	    }

	    IOIOPacket other = (IOIOPacket) obj;

	    return (other.message == message) && arrEq(other.payload, payload);
	}

    private boolean arrEq(byte[] arr1, byte[] arr2) {
        if (arr1 == null && arr2 == null) {
            return true;
        }

        if (arr1.length != arr2.length) {
            return false;
        }

        for (int i = 0; i < arr1.length; i++) {
            if (arr1[i] != arr2[i]) {
                return false;
            }
        }
        return true;
    }

    @Override
    public String toString() {
        return "IOIO Packet(" + message + ") : " + toString(payload);
    }

    private String toString(byte[] arr) {
        if (arr == null) {
            return "null";
        }
        String msg = "";
        for (byte val : arr) {
            msg += " " + val;
        }
        return msg;
    }
}