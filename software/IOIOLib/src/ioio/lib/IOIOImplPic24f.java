/**
 * TODO(TF): What is the copyright info? all the files need it
 */
package ioio.lib;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;


/**
 * High level interface and vars to the IOIO.
 *
 * TODO(TF): make sure the right set of methods are extracted to the interface
 * TODO(TF): manage resources, ie. uart modules, pwm modules, pin allocation, mcu usage
 * TODO(arshan): refactor this to IOIOPic24 or something more descriptive.
 *
 * @author arshan
 */
public class IOIOImplPic24f extends Service implements IOIO {

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
	static private IOIOImplPic24f singleton = null;

	// Cache some packets that are static
	private static final IOIOPacket HARD_RESET_PACKET = new IOIOPacket(Constants.HARD_RESET, Constants.IOIO_MAGIC);
	private static final IOIOPacket SOFT_RESET_PACKET = new IOIOPacket(Constants.SOFT_RESET, null);

	private final IOIOConnection ioio_connection;

	public IOIOImplPic24f(IOIOConnection ioio_connection) {
	    this.ioio_connection = ioio_connection;
	}

	/**
	 * Strict singleton? or just for convenience
	 * @param iostream
	 */
	public IOIOImplPic24f() {
		this(new IOIOConnection());
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

	static long last_log = 0;
	public static void log(String msg) {
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
	    // TODO(birmiwal): throw exception if already connected?
	    if (!isConnected()) {
  	        // TODO(arshan): this wants to move, sorting out good flow.
	        ioio_connection.start();
	    }
	    // TODO(birmiwal): make this better
	    while (!ioio_connection.isConnected()) {
	        try {
                Thread.sleep(10);
            } catch (InterruptedException e) {
                // do nothing for now
            }
	    }
	}

	/**
	 *
	 */
	public void softReset() {
		ioio_connection.sendToIOIO(SOFT_RESET_PACKET);
		ioio_connection.resetListeners();
	}

	/**
	 *
	 */
	public void hardReset() {
		// request a reset
		ioio_connection.sendToIOIO(HARD_RESET_PACKET);
	}

	// Singleton? TBD
	static public IOIOImplPic24f getInstance() {
		if (singleton == null) {
			singleton = new IOIOImplPic24f();
		}
		return singleton;
	}

	public void registerListener(IOIOPacketListener listener){
		ioio_connection.registerListener(listener);
	}

	public void disconnect() {
	}

	// TODO(TF): support other modes.
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
		return new Uart(this, nextAvailableUart(), rx, tx, baud, parity, stopbits);
	}

	/**
	 * @return the next available uart module
	 */
	private int nextAvailableUart() {
		return 0; // support just the one for now.
	}


}
