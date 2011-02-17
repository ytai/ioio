package ioio.lib.pic;

import java.io.IOException;
import java.io.OutputStream;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

/**
 * Handles outgoing packets.
 *
 * @author birmiwal
 */
public class OutgoingHandler extends Thread {
	private final BlockingQueue<IOIOPacket> outgoing = new LinkedBlockingQueue<IOIOPacket>();
    private final OutputStream out;

	public OutgoingHandler(OutputStream out) {
        this.out = out;
	}

	@Override
    public void run() {
		IOIOPacket packet;
		IOIOLogger.log("started outgoing handler thread");
		try {
			while (!Thread.currentThread().isInterrupted()) {
			    IOIOLogger.log("outgoing thread waiting for a packet to send");
				packet = outgoing.take();
				IOIOLogger.log("outgoing thread Sending message: " + packet.toString());
				out.write(packet.message);
				if (packet.payload != null) {
					out.write(packet.payload);
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		} catch (InterruptedException ie) {
		    ie.printStackTrace();
		}
        IOIOLogger.log("outgoing thread exiting");
        return;
	}

	public void addToQueue(IOIOPacket packet) {
        IOIOLogger.log("offering packet for send: " + packet.toString());
	    outgoing.offer(packet);
	}

	public synchronized void halt() {
		IOIOLogger.log("halting outgoing thread");
		if (Thread.currentThread() != this) {
		    interrupt();
		}
	}
}