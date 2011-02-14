/**
 * 
 */
package ioio.lib;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.BindException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.util.LinkedList;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

/**
 * High level interface and vars.
 * @author arshan
 */
public class IOIO extends Service implements IOIOApi {

	private static final String TAG = "IOIO";
	
	// Where the onboard LED is connected.
	public static final int LED_PIN = 0;
	
	
	// for convenience, might not stay long
	// ytai: why singleton? i think there should be:
	// a. one class (this one) that implements the protocol and exposes IOIOApi.
	//    has a non-public (package scope) ctor that gets a InputStream and OutputStream
	//    for the connection.
	// b. a factory/glue/bootstrap class that listens on TCP and instantiates the IOIO class with
	//    its input and output streams when a connection comes in.
	// the upside is that this class become independent from the physical connection.
	// for testing purposes we can give it mock input and output stream.
	// for development we can stick a protocol logger between this layer and the TCP socket, etc.
	static private IOIO singleton = null;
	
	
	// Cache some packets that are static
	private IOIOPacket HARD_RESET_PACKET = new IOIOPacket(HARD_RESET, IOIO_MAGIC);
	private IOIOPacket SOFT_RESET_PACKET = new IOIOPacket(SOFT_RESET, null);
	
	
	private IOIOConnection ioio_connection;
	/**
	 * Strict singleton? or just for convenience
	 * @param iostream
	 */
	public IOIO() {
		ioio_connection = new IOIOConnection();
		init();
		// TODO(arshan): this wants to move, sorting out good flow.
		ioio_connection.start();
	}
	
	private void init() {
		ioio_connection.init();
	}

	

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}
	
	public boolean isConnected() {
		return ioio_connection.isConnected();
	}
	
	// queue an outgoing packet
	public void queuePacket(IOIOPacket pkt) {
		ioio_connection.sendToIOIO(pkt);
	}
	
	long last_log = 0;
	public void log(String msg) {
		long current = System.currentTimeMillis();
		Log.i(TAG, msg + " @" + current + " (+" +(current-last_log)+ ")");
		last_log = current;
	}
	
	/**
	 * Blocking call that will not return until connected. 
	 * hmm. this must throw an exception at some point, else very un-androidy
	 */
	// ytai: put a abort() method here that guarantees to immediately exist all currently
	// blocking calls with an AbortedException or something.
	public void connect() {
		
	}

	/**
	 * 
	 */
	public void softReset() {
		ioio_connection.sendToIOIO(SOFT_RESET_PACKET);
		init();
	}
	
	/**
	 * 
	 */
	public void hardReset() {
		// request a reset
		ioio_connection.sendToIOIO(HARD_RESET_PACKET);
	}
	
	// Singleton? TBD
	static public IOIO getInstance() {
		if (singleton == null) {
			singleton = new IOIO();
		}
		return singleton;
	}

	
	/**
	 * Simple thread to maintain connection to IOIO, buffers all IO
	 * independent of the rest of operations.
	 * 
	 */
	public class IOIOConnection extends Thread {
				
		private static final String TAG = "IOIOConnection";
		
		BlockingQueue<IOIOPacket> outgoing = new LinkedBlockingQueue<IOIOPacket>();
		OutgoingHandler outgoingHandler;
		
		LinkedList<IOIOPacketListener> listeners;
		
		// Didnt find documentation but looking at firmware this seems right.
		public static final int IOIO_PORT = 4545;
		
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
		
		// Connection state.
		int state = 0;
		
		public static final int CONNECTED = 1;
		public static final int SHUTTINGDOWN = 2;
		public static final int DISCONNECTED = 3;
		
		public IOIOConnection() {
			this.port = IOIO_PORT;			
		}
		
		public IOIOConnection(int port) {
			this.port = port;
		}		

		void init() {
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
			
			if (packet.message != ESTABLISH_CONNECTION ||
				contents[0] != IOIO_MAGIC[0] ||
				contents[1] != IOIO_MAGIC[1] ||
				contents[2] != IOIO_MAGIC[2] ||
				contents[3] != IOIO_MAGIC[3] 				
			) return false;
			
			// TODO(arshan): verify that the hardware/firmware/bootloader versions are ones
			// this library supports, for forward compatibility.
			
			log("Hardware ID : " + asInt(contents, 3, 4));
			log("Bootload ID : " + asInt(contents, 3, 7));
			log("Firmware ID : " + asInt(contents, 3, 10));
					
			log("ESTABLISH packet verified");
			return true;
		}

		/**
		 * poll the input stream until the bytes of the buffer are filled.
		 * @param buffer
		 */
		private boolean readFully(byte[] buffer) throws IOException {
			int val = 0;
			int current = 0;
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
			// TODO(TF): sort this out, read should block unless we are getting EOF.
			int val;
			val = in.read();
			while (val == -1) {
				// sleep(1); 
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
			
			public void run() {
				IOIOPacket packet;
				log("started outgoing handler thread");
				try {
				while (running) {					
					try {
						packet = outgoing.take();
						log("Sending message " + packet.message);						
						out.write(packet.message);
						if (packet.payload != null) {
							out.write(packet.payload);
						}						
					} 
					catch (InterruptedException ie)	{	
						if (!running) { 
							log("outgoing thread exiting");
							return; }
					}				
				}
				}catch (IOException e) {
					// TODO(arshan): reset the connection.
					e.printStackTrace();
				}
			}
			
			public synchronized void halt() {
					log("halting outgoing thread");
					running = false;
					this.notifyAll();				
			}
		}
		/**
		 * 
		 */
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
				log("waiting for connection");
				if (reconnect()) {					
					// state here must be CONNECTED
					log("initial connection");
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
							
							case ESTABLISH_CONNECTION: // 13 byte payload
								if (verifyEstablishPacket(new IOIOPacket(message_type, readBytes(13)))) {									
									startOutgoingHandler();									
								} else {
									state = SHUTTINGDOWN;
								}
								// TODO(arshan): pass on to listeners? 
								break;
							
							case REPORT_ANALOG_FORMAT: // variable
								int numPins = readByte();
								break;
								
							case REPORT_ANALOG_STATUS: // variable 
								break;
								
							case REPORT_PERIODIC_DIGITAL: // variable
								break;
								
							case UART_CONFIGURE: // 3 byte payload
								handleIOIOPacket(new IOIOPacket(message_type, readBytes(3)));								
								break;
								
							case SET_PERIODIC_SAMPLE: // 2 byte payload
							case UART_SET_RX:
							case UART_SET_TX:
							case UART_TX_STATUS:
								handleIOIOPacket(new IOIOPacket(message_type, readBytes(2)));								
								break;
								
							case SET_OUTPUT: // 1 byte payload
							case SET_INPUT:
							case SET_CHANGE_NOTIFY:
							case SET_ANALOG_INPUT:
							case REPORT_DIGITAL_STATUS:
								handleIOIOPacket(new IOIOPacket(message_type, readBytes(1)));								
								break;

							case SOFT_RESET: // 0 byte payload
								handleIOIOPacket(new IOIOPacket(message_type, null));								
								break;
									
							// TODO(TF): we are avoiding this with a while loop above, theres a bug somewhere.
							case EOF: // The universal signal of end connection
								log("Connection broken by EOF");
								state = SHUTTINGDOWN;
								break;
								
							default:
								// TODO(ytai): if we had a standard header, that included number of payload bytes ...
								log("Unknown message type : " + message_type);
							    state = SHUTTINGDOWN; // conservative now, try to recover later.
							}
						}
						
						if (state == SHUTTINGDOWN) {
							log("Connection is shutting down");
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
			log("handle packet : " + packet.message);
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
	

	public void registerListener(IOIOPacketListener listener){
		ioio_connection.registerListener(listener);		
	}
	
	// TODO(TF): fix mem management throughout, try to allocate nearly nothing
	public class BytePool{
		
		public byte[] getByteBuffer(int size) {
			return new byte[size];
		}
		
		public void returnByteBuffer(byte[] b) {
			
		}
	}

	public void disconnect() {
	}

	public DigitalInput openDigitalInput(int pin) {
		return new DigitalInput(this, pin, DigitalInput.PULL_DOWN);
	}

	public DigitalOutput openDigitalOutput(int pin) {		
		return new DigitalOutput(this, pin, DigitalOutput.SOURCE);
	}

	public AnalogInput openAnalogInput(int pin) {
		return new AnalogInput(this,pin);
	}

	public PwmOutput openPwmOutput(int pin) {
		// TODO Auto-generated method stub
		return null;
	}

	public Uart openUart(int rx, int tx, int baud, int parity, float stopbits) {
		return new Uart(this, rx, tx, baud, parity, stopbits);
	}
	

}
