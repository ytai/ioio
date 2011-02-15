package ioio.lib;

import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.BindException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

/**
 * Simple thread to maintain connection to IOIO, buffers all IO
 * independent of the rest of operations.
 *
 */
public class IOIOConnection extends Thread {

	private static final String TAG = "IOIOConnection";

	BlockingQueue<IOIOPacket> outgoing = new LinkedBlockingQueue<IOIOPacket>();
	OutgoingHandler outgoingHandler;

	List<IOIOPacketListener> listeners;

	// mS timeout in which we consider the ioio not connected if no response.
	// ytai: i would just leave it to the client to abort(). from a GUI perspective
	// it might more sense to leave it to the human user to press "cancel" when he
	// gives up. he might be in the process of connecting the ioio and will be annoyed
	// with automatic timeout. if a certain app does want this behaviour, they can always
	// setup a thread that sleeps for 3 seconds, then fires abort() unless stopped.
	// arshan: not sure that 3seconds is the right time, but think we should have some
	// upper bound where if no connection is made we eject any pending requests of IOIO,
	// and any blocking calls in the api throw exceptions
	public static final int IOIO_TIMEOUT = 3000;

	public static final int EOF = -1;

	ServerSocket ssocket;
	Socket socket;
	int port;

	InputStream in;
	OutputStream out;

	// Connection specific vars for analog status.
    int analogPinCount = 0;
    int analogPinBytes = 0;

	// Connection state.
	int state = 0;

	public static final int CONNECTED = 1;
	public static final int SHUTTINGDOWN = 2;
	public static final int DISCONNECTED = 3;

	private boolean disconnectionRequested;

	public IOIOConnection() {
		this(Constants.IOIO_PORT);
	}

	public IOIOConnection(int port) {
		this.port = port;
	}

	void resetListeners() {
		listeners = new LinkedList<IOIOPacketListener>();
	}

	IOIOConnection(InputStream in, OutputStream out) {
		this.in = in;
		this.out = out;
	}

	public IOIOConnection(ServerSocket ssocket) {
		this.ssocket = ssocket;
	}

	public boolean startService() throws IOException, BindException {
	    if (ssocket == null) {
	        ssocket = new ServerSocket(port);
	    }
		return true;
	}

	// Will formalize once we have a working hello world.
	private boolean verifyEstablishPacket(IOIOPacket packet) {
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

	private void startOutgoingHandler(OutputStream out) {
		if (outgoingHandler != null) {
			outgoingHandler.halt();
		}
		// clear any pending ...
		synchronized (outgoing) {
			outgoing.clear();
			outgoingHandler = new OutgoingHandler(outgoing, out);
			outgoingHandler.start();
		}
	}

	@Override
    public void run() {
		setTimeout(IOIO_TIMEOUT);

		try {
			startService();
		} catch (IOException e) {
			Log.e(TAG, "Could not open serversocket");
			e.printStackTrace();
			return;
		}

		while (!disconnectionRequested) {
			IOIOLogger.log("waiting for connection");
			if (!reconnect()) {
			    continue;
			}
		     resetListeners();
			// state here must be CONNECTED
			IOIOLogger.log("initial connection");
			try {
				in = socket.getInputStream();
				out = socket.getOutputStream();
			} catch (IOException e) {
				e.printStackTrace();
			}

			try {
				// Handle any incoming packets
				int message_type;
				while (!disconnectionRequested && state == CONNECTED) {

					message_type = Bytes.readByte(in);
					// IOIOLogger.log("read message of type: " + message_type);

					// TODO(arshan): how do we re-sync if things have gone bad.
					// ytai: it is in the protocol spec: you close the socket (or i/o streams),
					// reset your internal state, and wait for ioio to reconnect.
					switch (message_type) {

					case Constants.ESTABLISH_CONNECTION: // 13 byte payload
						if (verifyEstablishPacket(new IOIOPacket(message_type, Bytes.readBytes(in, 13)))) {
							startOutgoingHandler(out);
						} else {
						    IOIOLogger.log("setting state to shutting down");
							state = SHUTTINGDOWN;
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
						handleIOIOPacket(new IOIOPacket(message_type, payload));
						break;

					case Constants.REPORT_ANALOG_STATUS: // variable
						handleIOIOPacket(new IOIOPacket(message_type, Bytes.readBytes(in, analogPinBytes)));
						break;

					case Constants.REPORT_PERIODIC_DIGITAL: // variable
						break;

					case Constants.UART_CONFIGURE: // 3 byte payload
						handleIOIOPacket(new IOIOPacket(message_type, Bytes.readBytes(in, 3)));
						break;

					case Constants.SET_PERIODIC_SAMPLE: // 2 byte payload
					case Constants.UART_SET_RX:
					case Constants.UART_SET_TX:
					case Constants.UART_TX_STATUS:
						handleIOIOPacket(new IOIOPacket(message_type, Bytes.readBytes(in, 2)));
						break;

					case Constants.SET_OUTPUT: // 1 byte payload
					case Constants.SET_INPUT:
					case Constants.SET_CHANGE_NOTIFY:
					case Constants.SET_ANALOG_INPUT:
					case Constants.REPORT_DIGITAL_STATUS:
						handleIOIOPacket(new IOIOPacket(message_type, Bytes.readBytes(in, 1)));
						break;

					case Constants.SOFT_RESET: // 0 byte payload
						handleIOIOPacket(new IOIOPacket(message_type, null));
						break;

					// TODO(TF): we are avoiding this with a while loop above, theres a bug somewhere.
					case EOF: // The universal signal of end connection
						IOIOLogger.log("Connection broken by EOF");
						state = SHUTTINGDOWN;
						break;

					default:
						// TODO(ytai): if we had a standard header, that included number of payload bytes ...
						IOIOLogger.log("Unknown message type : " + message_type);
					    state = SHUTTINGDOWN; // conservative now, try to recover later.
					}
				}

				if (state == SHUTTINGDOWN) {
					handleShutdown();
				}

			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			// We're all done
			state = DISCONNECTED;
		}
	}

    private void handleShutdown() throws IOException {
        IOIOLogger.log("Connection is shutting down");
        outgoingHandler.halt();
        in.close();
        out.close();
        socket.close();
        socket = null;
    }

	private void figureAnalogBytes() {

	}

	static void sleep(int ms) {
		try {
			Thread.sleep(ms);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	public void setTimeout(int timeout) {
		if (ssocket != null && timeout > 0) {
			try {
				ssocket.setSoTimeout(timeout);
			} catch (SocketException e) {
				e.printStackTrace();
			}
		}
	}

	/**
	 * Attempt to reconnect ( or connect if first time ).
	 *
	 * @return true if successful
	 */
	public boolean reconnect() {
			try {
				if (socket != null) {
					socket.close();
					socket = null;
				}
				socket = ssocket.accept();
				state = CONNECTED;
			}
			catch (SocketTimeoutException toe) {
				toe.printStackTrace();
			}
			catch (IOException e) {
				e.printStackTrace();
			}
			return isConnected();
	}

	public boolean isConnected() {
		return state == CONNECTED;
	}

	/**
	 * Handle an incoming packet from the IOIO.
	 */
	public void handleIOIOPacket(IOIOPacket packet) {
		// send to my listeners
		// IOIOLogger.log("handle packet : " + packet.message);
		// TODO(arshan): add some filters for message types? we've already case'd it above
		for (IOIOPacketListener listener: listeners) {
			listener.handlePacket(packet);
		}
	}

	public void registerListener(IOIOPacketListener listener) {
		if (!listeners.contains(listener)) {
			listeners.add(listener);
		}
	}

	/**
	 * queue a packet to be sent to the IOIO at the next
	 * opportunity.
	 * NOTE: this is not necessarily immediate.
	 * @param packet
	 */
	public void sendToIOIO(IOIOPacket packet) {
		outgoing.offer(packet);
	}

    public void disconnect() {
        disconnectionRequested = true;
        try {
            handleShutdown();
            ssocket.close();
            ssocket = null;
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
}