package ioio.lib;
/**
 * A packet to or from the IOIO protocol.
 *
 * @author arshan
 */
public class IOIOPacket {

	public final int message;
	
	// TODO(arshan): change all byte[] for ByteBuffer?
	// dont know what the trade off is performance/convenience
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
        return "IOIO Packet(0x" + Integer.toHexString(message&0xFF) + ") : "+ toString(payload);
    }

    private String toString(byte[] arr) {
        if (arr == null) {
            return "null";
        }
        String msg = "";
        
        for (byte val : arr) {
            msg += " " + Integer.toHexString(val&0xFF);
        }
        return msg;
    }

 // utility method to log the packet
    public void log() {
       log("");
    }
    
    // utility method to log the packet
    public void log(String prefix) {
        IOIOLogger.log(prefix + toString());
    }
}