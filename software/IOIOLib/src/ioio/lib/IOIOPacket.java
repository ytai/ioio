package ioio.lib;
/**
 * reflect the state of a packet from or to the IOIO
 * @author arshan
 *
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
}