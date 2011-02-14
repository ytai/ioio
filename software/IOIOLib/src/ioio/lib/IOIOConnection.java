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

	public boolean startService() throws IOException, BindException{
		ssocket = new ServerSocket(this.port);
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

		IOIOImplPic24f.log("Hardware ID : " + asInt(contents, 3, 4));
		IOIOImplPic24f.log("Bootload ID : " + asInt(contents, 3, 7));
		IOIOImplPic24f.log("Firmware ID : " + asInt(contents, 3, 10));

		IOIOImplPic24f.log("ESTABLISH packet verified");
		return true;
	}

	/**
	 * poll the input stream until the bytes of the buffer are filled.
	 * @param buffer
	 */

	private boolean readFully(byte[] buffer) throws IOException {
		return readFully(buffer, 0);
	}

	private boolean readFully(byte[] buffer, int offset) throws IOException {
		int val = 0;
		int current = offset;
		while (current < buffer.length) {
			val = in.read(buffer, current, buffer.length-current);
			if (val == -1) {
				return false;
			}
			current += val;
		}
		return true;
	}

	private int readByte() throws IOException{
		// !!! its gone? WTF. I swear I was getting no-block, just return EOF, and then
		// it would still function for return from IOIO.
		boolean workaround = false;

		if (!workaround) {
			return in.read();
		}
		else {
		// Ahem, getting -1's instead of blocking?
		// TODO(arshan): This seems to be gone, leaving it for some more debugging, then delete.
		int val;
		val = in.read();
		while (val == -1) {
			sleep(1);
			// yield();
			val = in.read();
		}
		return val;
		}
	}

	// Utility to allocate the array and return it with bytes filled.
	private byte[] readBytes(int size) throws IOException {
		byte[] bytes = new byte[size];
		if (readFully(bytes)) {
			return bytes;
		}
		throw new IOException("stream is broke");
	}

	private int asInt(byte[] bytes, int len, int offset) {
		int result = 0;
		// TODO(arshan) unroll
		for (int x = 0; x < len; x++) {
			result |= (0xFF & bytes[x + offset]) << (x * 8);
		}
		return result;
	}

	private void startOutgoingHandler() {
		if (outgoingHandler != null) {
			outgoingHandler.halt();
		}
		// clear any pending ...
		synchronized (outgoing) {
			outgoing.clear();
			outgoingHandler = new OutgoingHandler();
			outgoingHandler.start();
		}
	}

	public class OutgoingHandler extends Thread {
		boolean running = true;

		@Override
        public void run() {
			IOIOPacket packet;
			IOIOImplPic24f.log("started outgoing handler thread");
			try {
			while (running) {
				try {
					packet = outgoing.take();
					IOIOImplPic24f.log("Sending message " + packet.message);
					out.write(packet.message);
					if (packet.payload != null) {
						out.write(packet.payload);
					}
				}
				catch (InterruptedException ie)	{
					if (!running) {
						IOIOImplPic24f.log("outgoing thread exiting");
						return; }
				}
			}
			}catch (IOException e) {
				// TODO(arshan): reset the connection.
				e.printStackTrace();
			}
		}

		public synchronized void halt() {
				IOIOImplPic24f.log("halting outgoing thread");
				running = false;
				this.notifyAll();
		}
	}
	/**
	 *
	 */
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

		while (true) {
			IOIOImplPic24f.log("waiting for connection");
			if (reconnect()) {
			     resetListeners();
				// state here must be CONNECTED
				IOIOImplPic24f.log("initial connection");
				try {
					in = socket.getInputStream();
					out = socket.getOutputStream();
					state = CONNECTED;
				} catch (IOException e) {
					e.printStackTrace();
				}

				try {
					// Handle any incoming packets
					int message_type;
					while (state == CONNECTED) {

						message_type = readByte();

						// TODO(arshan): how do we re-sync if things have gone bad.
						// ytai: it is in the protocol spec: you close the socket (or i/o streams),
						// reset your internal state, and wait for ioio to reconnect.
						switch (message_type) {

						case Constants.ESTABLISH_CONNECTION: // 13 byte payload
							if (verifyEstablishPacket(new IOIOPacket(message_type, readBytes(13)))) {
								startOutgoingHandler();
							} else {
								state = SHUTTINGDOWN;
							}
							// TODO(arshan): pass on to listeners?
							break;

						case Constants.REPORT_ANALOG_FORMAT: // variable
							analogPinCount = readByte();
							int groups = (analogPinCount/4) + 1;
							analogPinBytes = (groups * 5) + (analogPinCount % 4) + 1;
							byte[] payload = new byte[analogPinCount+1];
							payload[0] = (byte)analogPinCount;
							readFully(payload, 1);
							handleIOIOPacket(new IOIOPacket(message_type, payload));
							break;

						case Constants.REPORT_ANALOG_STATUS: // variable
							handleIOIOPacket(new IOIOPacket(message_type, readBytes(analogPinBytes)));
							break;

						case Constants.REPORT_PERIODIC_DIGITAL: // variable
							break;

						case Constants.UART_CONFIGURE: // 3 byte payload
							handleIOIOPacket(new IOIOPacket(message_type, readBytes(3)));
							break;

						case Constants.SET_PERIODIC_SAMPLE: // 2 byte payload
						case Constants.UART_SET_RX:
						case Constants.UART_SET_TX:
						case Constants.UART_TX_STATUS:
							handleIOIOPacket(new IOIOPacket(message_type, readBytes(2)));
							break;

						case Constants.SET_OUTPUT: // 1 byte payload
						case Constants.SET_INPUT:
						case Constants.SET_CHANGE_NOTIFY:
						case Constants.SET_ANALOG_INPUT:
						case Constants.REPORT_DIGITAL_STATUS:
							handleIOIOPacket(new IOIOPacket(message_type, readBytes(1)));
							break;

						case Constants.SOFT_RESET: // 0 byte payload
							handleIOIOPacket(new IOIOPacket(message_type, null));
							break;

						// TODO(TF): we are avoiding this with a while loop above, theres a bug somewhere.
						case EOF: // The universal signal of end connection
							IOIOImplPic24f.log("Connection broken by EOF");
							state = SHUTTINGDOWN;
							break;

						default:
							// TODO(ytai): if we had a standard header, that included number of payload bytes ...
							IOIOImplPic24f.log("Unknown message type : " + message_type);
						    state = SHUTTINGDOWN; // conservative now, try to recover later.
						}
					}

					if (state == SHUTTINGDOWN) {
						IOIOImplPic24f.log("Connection is shutting down");
						outgoingHandler.halt();
						in.close();
						out.close();
						socket.close();
						socket = null;
					}

				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}

				// We're all done
				state = DISCONNECTED;
			}
		}
	}

	private void figureAnalogBytes() {

	}

	private void sleep(int ms) {
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
		IOIOImplPic24f.log("handle packet : " + packet.message);
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
}