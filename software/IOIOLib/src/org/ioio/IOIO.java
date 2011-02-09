/**
 * 
 */
package org.ioio;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;

import android.util.Log;

/**
 * High level interface and vars.
 * @author arshan
 */
public class IOIO implements IOIOApi {

	
	// for convenience, might not stay long
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
	
	// Incoming messages (where different)
	private static final int ESTABLISH_CONNECTION = 0x00;
		
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
	
	public boolean isConnected() {
		return ioio_connection.isConnected();
	}
	
	/**
	 * Blocking call that will not return until connected. 
	 * hmm. this must throw an exception at some point, else very un-androidy
	 */
	public void connect() {
		
	}
	
	public void softReset() {
		
	}
	
	public void hardReset() {
		
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
		public final byte[] IOIO_MAGIC = { 0x4F,  0x49, 0x4F, 0x49 };
		
		// mS timeout in which we consider the ioio not connected if no response.
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
		
		public IOIOConnection(ServerSocket ssocket) {
			this.ssocket = ssocket;
		}
		
		public boolean startService() throws IOException {
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
			out.write(HARD_RESET);
			out.write(IOIO_MAGIC);
			
			byte[] establish_packet = new byte[14];
			if (readFully(establish_packet)) {				
				return verifyEstablishPacket(establish_packet);
			}			
			return false;			
		}
		
		private boolean verifyEstablishPacket(byte[] contents) {
			if (contents[0] != ESTABLISH_CONNECTION ||
					contents[1] != IOIO_MAGIC[0] ||
					contents[2] != IOIO_MAGIC[1] ||
					contents[3] != IOIO_MAGIC[2] ||
					contents[4] != IOIO_MAGIC[3] 				
			) return false;
			
			return true;
		}

		/**
		 * poll the input stream until the bytes of the buffer are filled.
		 * @param buffer
		 */
		private boolean readFully(byte[] buffer) throws IOException {
			int val = 0;
			for (int x = 0; x < buffer.length; x++) {
				val = in.read();
				if (val == -1) return false;
				buffer[x] = (byte) (0xFF & val);				
			}
			return true;
		}
		
		public void run() {			
			
			setTimeout(IOIO_TIMEOUT);
			try {
				startService();
			} catch (IOException e) {
				Log.e("IOIOConnection", "Could not open serversocket");
				e.printStackTrace();
			}
			
			while (true) {
				if (reconnect()) {
					state = CONNECTED;
					Log.i("IOIOConnection", "initial connection");
					try {
						in = socket.getInputStream();
						out = socket.getOutputStream();
					} catch (IOException e) {
						e.printStackTrace();
					}
					
					// Try the handshake
					// TODO(arshan): find a softer way of doing this, maybe new message in proto
					try {
						if (! handshake()) {
							Log.e("IOIOConnection", "handshake FAILED");
							state = DISCONNECTED;
						}
					} catch (IOException e) {
						e.printStackTrace();
					}					
					
					try {
						// Handle any incoming packets
						int message_type;
						while (state == CONNECTED) {							
							
							message_type = in.read();
							// TODO(arshan): how do we re-sync if things have gone bad.
							switch (message_type) {
							case ESTABLISH_CONNECTION:
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
					// TODO Auto-generated catch block
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
				}
				catch (SocketTimeoutException toe) {
					
				}
				catch (IOException e) {
					
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
		
		private int message;
		private byte[] payload;

		public IOIOPacket(int message, byte[] payload) {
			
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
		// TODO Auto-generated method stub
		
	}

	public DigitalInput getDigitalInput(int pin) {
		return new DigitalInput(this, pin);
	}

	public DigitalOutput getDigitalOutput(int pin) {		
		return new DigitalOutput(this, pin);
	}

	public AnalogInput getAnalogInput(int pin) {
		// TODO Auto-generated method stub
		return null;
	}

	public PWMOutput getPWMOutput(int pin) {
		// TODO Auto-generated method stub
		return null;
	}

	public UART openUART(int rx, int tx, int baud, int parity, int stopbits) {
		// TODO Auto-generated method stub
		return null;
	}
}
