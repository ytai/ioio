package ioio.lib.pic;

import java.io.IOException;
import java.io.InputStream;

/**
 * Handlers incoming packets from the IOIO board.
 *
 * @author birmiwal
 */
public class IncomingHandler extends Thread {

    private final InputStream in;
    private final ConnectionStateCallback stateCb;
    // Connection specific vars for analog status.
    int analogPinCount = 0;
    int analogPinBytes = 0;
    private final IOIOPacketListener packetHandler;


    public IncomingHandler(InputStream in, ConnectionStateCallback stateCb, IOIOPacketListener packetHandler) {
        this.in = in;
        this.stateCb = stateCb;
        this.packetHandler = packetHandler;
    }

    @Override
    public void run() {
        IOIOLogger.log("starting incoming handler");
		// state here must be CONNECTED
		try {
			// Handle any incoming packets
			while (!Thread.currentThread().isInterrupted()) {
				int message_type = Bytes.readByte(in);
				// IOIOLogger.log("read message of type: " + message_type);

				// TODO(arshan): how do we re-sync if things have gone bad.
				// ytai: it is in the protocol spec: you close the socket (or i/o streams),
				// reset your internal state, and wait for ioio to reconnect.
				switch (message_type) {

				case Constants.ESTABLISH_CONNECTION: // 13 byte payload
					if (IncomingHandler.verifyEstablishPacket(new IOIOPacket(message_type, Bytes.readBytes(in, 13)))) {
					    stateCb.stateChanged(ConnectionState.CONNECTION_VERIFIED);
					} else {
					    IOIOLogger.log("setting state to shutting down");
					    return;
					}
					// TODO(arshan): pass on to listeners?
					break;

				case Constants.REPORT_ANALOG_FORMAT: // variable
					analogPinCount = Bytes.readByte(in);
					int groups = (analogPinCount+3)/4;
					analogPinBytes = (analogPinCount * 10 + 7)/8; //(groups * 5) + (analogPinCount % 4) + 1;
					byte[] payload = new byte[analogPinCount+1];
					payload[0] = (byte)analogPinCount;
					Bytes.readFully(in, payload, 1);
					packetHandler.handlePacket(new IOIOPacket(message_type, payload));
					break;

				case Constants.REPORT_ANALOG_STATUS: // variable
				    packetHandler.handlePacket(new IOIOPacket(message_type, Bytes.readBytes(in, analogPinBytes)));
					break;

				case Constants.REPORT_PERIODIC_DIGITAL: // variable
					break;

				case Constants.UART_CONFIGURE: // 3 byte payload
				    packetHandler.handlePacket(new IOIOPacket(message_type, Bytes.readBytes(in, 3)));
					break;

				case Constants.SET_PERIODIC_SAMPLE: // 2 byte payload
				case Constants.UART_SET_RX:
				case Constants.UART_SET_TX:
				case Constants.UART_TX_STATUS:
				    packetHandler.handlePacket(new IOIOPacket(message_type, Bytes.readBytes(in, 2)));
					break;

				case Constants.SET_OUTPUT: // 1 byte payload
				case Constants.SET_INPUT:
				case Constants.SET_CHANGE_NOTIFY:
				case Constants.SET_ANALOG_INPUT:
				case Constants.REPORT_DIGITAL_STATUS:
				    packetHandler.handlePacket(new IOIOPacket(message_type, Bytes.readBytes(in, 1)));
					break;

				case Constants.SOFT_RESET: // 0 byte payload
				    packetHandler.handlePacket(new IOIOPacket(message_type, null));
					break;

				// TODO(TF): we are avoiding this with a while loop above, theres a bug somewhere.
				case IOIOConnection.EOF: // The universal signal of end connection
					IOIOLogger.log("Connection broken by EOF");
					return;

				default:
					// TODO(ytai): if we had a standard header, that included number of payload bytes ...
					IOIOLogger.log("Unknown message type : " + message_type);
					return;
				}
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {
            stateCb.stateChanged(ConnectionState.SHUTTING_DOWN);
		}
	}

    public void halt() {
        safeClose(in);
        if (Thread.currentThread() != this) {
            interrupt();
        }
    }

    private void safeClose(InputStream in) {
        if (in != null) {
            try {
                in.close();
            } catch (IOException e) {
                // do nothing
                e.printStackTrace();
            }
        }
    }

    // Will formalize once we have a working hello world.
    static boolean verifyEstablishPacket(IOIOPacket packet) {
    	byte[] contents = packet.payload;

    	if (packet.message != Constants.ESTABLISH_CONNECTION ||
    		contents[0] != Constants.IOIO_MAGIC[0] ||
    		contents[1] != Constants.IOIO_MAGIC[1] ||
    		contents[2] != Constants.IOIO_MAGIC[2] ||
    		contents[3] != Constants.IOIO_MAGIC[3]
    	) return false;

    	// TODO(arshan): verify that the hardware/firmware/bootloader versions are ones
    	// this library supports, for forward compatibility.

    	IOIOLogger.log("Hardware ID : " + Bytes.asInt(contents, 3, 4));
    	IOIOLogger.log("Bootload ID : " + Bytes.asInt(contents, 3, 7));
    	IOIOLogger.log("Firmware ID : " + Bytes.asInt(contents, 3, 10));

    	IOIOLogger.log("ESTABLISH packet verified");
    	return true;
    }
}