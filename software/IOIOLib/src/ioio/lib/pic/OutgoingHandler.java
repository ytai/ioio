package ioio.lib.pic;

import ioio.lib.IoioException.ConnectionLostException;

import java.io.IOException;
import java.io.OutputStream;

/**
 * Handles outgoing packets.
 *
 * @author birmiwal
 */
public class OutgoingHandler {
    private final OutputStream out;

	public OutgoingHandler(OutputStream out) {
        this.out = out;
	}

	public void sendPacket(IoioPacket packet) throws ConnectionLostException {
        IoioLogger.log("outgoing thread Sending message: " + packet.toString());
	    try {
            out.write(packet.message);
            if (packet.payload != null) {
                out.write(packet.payload);
            }
        } catch (IOException e) {
            throw new ConnectionLostException("IO Exception sending packet: " + e.getMessage());
        }
	}
}