package ioio.lib.pic;

import java.io.IOException;
import java.io.OutputStream;
import java.util.concurrent.BlockingQueue;

public class OutgoingHandler extends Thread {
	boolean running = true;

	private final BlockingQueue<IOIOPacket> outgoing;

    private final OutputStream out;

	public OutgoingHandler(BlockingQueue<IOIOPacket> outgoing, OutputStream out) {
        this.outgoing = outgoing;
        this.out = out;
	}

	@Override
    public void run() {
		IOIOPacket packet;
		IOIOLogger.log("started outgoing handler thread");
		try {
			while (running) {
				try {
					packet = outgoing.take();
					IOIOLogger.log("Sending message: " + packet.toString());
					out.write(packet.message);
					if (packet.payload != null) {
						out.write(packet.payload);
					}
				}
				catch (InterruptedException ie)	{
					if (!running) {
						IOIOLogger.log("outgoing thread exiting");
						return;
					}
				}
			}
		} catch (IOException e) {
			// TODO(arshan): reset the connection.
			e.printStackTrace();
		}
	}

	public synchronized void halt() {
		IOIOLogger.log("halting outgoing thread");
		running = false;
		this.notifyAll();
	}
}