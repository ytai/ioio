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

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

/**
 * High level interface and vars.
 * @author arshan
 */
public class IOIO extends Service implements IOIOApi {

	// Magic bytes for the IOIO, spells 'IOIO'
	public final byte[] IOIO_MAGIC = { 0x49,  0x4F, 0x49, 0x4F };

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
	
	// Outgoing messages
	private static final int HARD_RESET 	= 0x00;
	private static final int SOFT_RESET 	= 0x01;
	private static final int SET_OUTPUT 	= 0x02;
	private static final int SET_VALUE 		= 0x03;
	private static final int SET_INPUT 		= 0x04;
	private static final int SET_CHANGE_NOTIFY = 0x05;
	private static final int SET_PERIODIC_SAMPLE = 0x06;
	private static final int RESERVED1 = 0x07;
	private static final int SET_PWM = 0x08;
	private static final int SET_DUTYCYCLE = 0x09;
	private static final int SET_PERIOD = 0x0A;
	private static final int SET_ANALOG_INPUT = 0x0B;
	private static final int UART_TX = 0x0C;
	
	// Cache some packets that are static
	private IOIOPacket HARD_RESET_PACKET = new IOIOPacket(HARD_RESET, IOIO_MAGIC);
	private IOIOPacket SOFT_RESET_PACKET = new IOIOPacket(SOFT_RESET, null);
	
	// Incoming messages (where different)
	private static final int ESTABLISH_CONNECTION = 0x00;
	private static final int REPORT_DIGITAL_STATUS = 0x03;
	private static final int REPORT_PERIODIC_DIGITAL = 0x07;
	private static final int REPORT_ANALOG_FORMAT = 0x08;
	private static final int REPORT_ANALOG_STATUS = 0x09;
	private static final int UART_RX = 0x0C;
	
	private IOIOConnection ioio_connection;
	/**
	 * Strict singleton? or just for convenience
	 * @param iostream
	 */
	public IOIO() {
		ioio_connection = new IOIOConnection();
		// TODO(arshan): this wants to move, sorting out good flow.
		ioio_connection.start();
	}
	
	

	

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}
	
	public boolean isConnected() {
		return ioio_connection.isConnected();
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
		// ytai: there are two separate scenarios here:
		//       1. the program has just started. the user is expected to connect the IOIO to the
		//          phone. in this case, and infinite timeout with the option to abort() (perhaps
		//          during OnDestroy() etc. is probably the most reasonable behavior.
		//       2. during the operation connection got lost. in this case, you should get an
		//          IOException from the socket, and could internally abort all blocking calls,
		//          throwing a IOIOConnectionLostExcption etc.
		public static final int IOIO_TIMEOUT = 3000; 

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
		
		/**
		 * Attempt a handshake, return true if successful.
		 * 
		 * This is a hard reset followed by listening for the 
		 * establish connection message. This should be amongst the 
		 * few synchronous transactions.
		 * @return
		 */
		private boolean handshake() throws IOException {		
			byte[] establish_packet = new byte[14];
			if (readFully(establish_packet)) {			
				Log.i("IOIO", "got EST packet");
				return verifyEstablishPacket(establish_packet);
			}					
			Log.e("IOIO", "FAILED HANDSHAKE");
			return false;			
		}

		// Will formalize once we have a working hello world.
		private boolean verifyEstablishPacket(byte[] contents) {
			if (contents[0] != ESTABLISH_CONNECTION ||
				contents[1] != IOIO_MAGIC[0] ||
				contents[2] != IOIO_MAGIC[1] ||
				contents[3] != IOIO_MAGIC[2] ||
				contents[4] != IOIO_MAGIC[3] 				
			) return false;
			
			// ytai: verify that the hardware/firmware/bootloader versions are ones
			// this library supports, for forward compatibility.
			Log.i("IOIO", "ESTABLISH packet verified");
			return true;
		}

		/**
		 * poll the input stream until the bytes of the buffer are filled.
		 * @param buffer
		 */
		private boolean readFully(byte[] buffer) throws IOException {
			int val = 0;
			int current = 0;
			while (val != buffer.length) {	
				val = in.read(buffer, val, buffer.length-val);
				if (val == -1) {
					return false;
				}
				current += val;
			}
			return true;
		}

		/**
		 * 
		 */
		public void run() {						
			setTimeout(IOIO_TIMEOUT);
			
			try {
				startService();
			} catch (IOException e) {
				Log.e("IOIOConnection", "Could not open serversocket");
				e.printStackTrace();
				return; 
			} 
			
			while (true) {
				if (reconnect()) {					
					// state here must be CONNECTED
					Log.i("IOIOConnection", "initial connection");
					try {
						in = socket.getInputStream();
						out = socket.getOutputStream();
					} catch (IOException e) {
						e.printStackTrace();
					}
					
					// Try the handshake
					// TODO(arshan): find a softer way of doing this, maybe new message in proto
					// This is to check that we can hardreset and then reestablish
					try {
						// ytai: why is this special? why not treat it within the main state
						// machine for incoming messages?
						// mostly for testing. though I could see the argument that since we know
						// this should always be the first message on connect, we should handle the first
						// one out of band.
						if (! handshake()) {
							Log.e("IOIOConnection", "handshake FAILED");
							state = DISCONNECTED;
						}
					} catch (IOException e) {
						e.printStackTrace();
					}					
					
					try {
						// ytai: IncomingMessageHandler class?
						// Handle any incoming packets
						int message_type;
						while (state == CONNECTED) {														
							message_type = in.read();
							// TODO(arshan): how do we re-sync if things have gone bad.
							// ytai: it is in the protocol spec: you close the socket (or i/o streams),
							// reset your internal state, and wait for ioio to reconnect.
							switch (message_type) {
							
							case ESTABLISH_CONNECTION:
								// this means the IOIO has reset.
								break;
							
							case REPORT_DIGITAL_STATUS:							
								break;
								
							case REPORT_ANALOG_FORMAT:
								break;
								
							case REPORT_ANALOG_STATUS:
								break;
								
							case REPORT_PERIODIC_DIGITAL:
								break;
																
							default:
								Log.i("IOIOConnection", "Unknown message type : " + message_type);
								state = SHUTTINGDOWN; // conservative now, try to recover later.
							}
						}
						
						if (state == SHUTTINGDOWN) {
							Log.i("IOIOConnection", "Connection is shutting down");
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
			
		}
	
		/**
		 * queue a packet to be sent to the IOIO at the next
		 * opportunity. 
		 * NOTE: this is not necessarily immediate.
		 * @param packet
		 */
		public void sendToIOIO(IOIOPacket packet) {
			
			
		}
	}
	
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
	
	// poormans resource pooling
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
		return new DigitalInput(this, pin);
	}

	public DigitalOutput openDigitalOutput(int pin) {		
		return new DigitalOutput(this, pin);
	}

	public AnalogInput openAnalogInput(int pin) {
		// TODO Auto-generated method stub
		return null;
	}

	public PwmOutput openPwmOutput(int pin) {
		// TODO Auto-generated method stub
		return null;
	}

	public Uart openUart(int rx, int tx, int baud, int parity, int stopbits) {
		// TODO Auto-generated method stub
		return null;
	}
}
