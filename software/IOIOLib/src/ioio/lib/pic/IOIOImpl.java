/**
 * TODO(TF): What is the copyright info? all the files need it
 */
package ioio.lib.pic;

import android.accounts.OperationCanceledException;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

import ioio.lib.IOIO;
import ioio.lib.IOIOException.OperationAbortedException;
import ioio.lib.IOIOException.OutOfResourceException;

import java.io.IOException;
import java.net.BindException;


/**
 * High level interface and vars to the IOIO.
 *
 * TODO(TF): make sure the right set of methods are extracted to the interface
 * TODO(TF): manage resources, ie. uart modules, pwm modules, pin allocation, mcu usage
 * TODO(arshan): refactor this to IOIOPic24 or something more descriptive.
 *
 * @author arshan
 */
public class IOIOImpl extends Service implements IOIO {

	private static final int CONNECT_WAIT_TIME_MS = 100;

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
	static private IOIOImpl singleton = null;

	private final IOIOConnection ioioConnection;

	private boolean abortConnection = false;

	ListenerManager listeners;

	public IOIOImpl(IOIOConnection ioio_connection, ListenerManager listeners) {
	    this.ioioConnection = ioio_connection;
	    this.listeners = listeners;
	}

	public IOIOImpl(ListenerManager listeners) {
	    this(new IOIOConnection(listeners), listeners);
	}

	/**
	 * Strict singleton? or just for convenience
	 * @param iostream
	 */
	public IOIOImpl() {
	    this(new ListenerManager());
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	@Override
    public boolean isConnected() {
		return ioioConnection.isVerified();
	}

	// queue an outgoing packet
	public void queuePacket(IOIOPacket pkt) {
		ioioConnection.sendToIOIO(pkt);
	}

	/**
	 * Blocking call that will not return until connected.
	 * hmm. this must throw an exception at some point, else very un-androidy
	 * @throws IOException
	 * @throws BindException
	 * @throws OperationCanceledException if {@link #abortConnection()} is called
	 */
	@Override
    public void waitForConnect() throws OperationAbortedException {
	    abortConnection = false;

	    // TODO(birmiwal): throw exception if already connected?
	    if (!isConnected()) {
	        try {
                ioioConnection.start();
            } catch (BindException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
	    }
	    // TODO(birmiwal): make this better
	    while (!isConnected() && !abortConnection) {
	        Sleeper.sleep(CONNECT_WAIT_TIME_MS);
	    }

	    if (!isConnected()) {
	        throw new OperationAbortedException("operation aborted while in connect()");
	    }

	    Sleeper.sleep(CONNECT_WAIT_TIME_MS);
	}

	@Override
	public void abortConnection() {
	    abortConnection = true;
	    ioioConnection.disconnect();
	}

	@Override
	public void softReset() {
		ioioConnection.sendToIOIO(Constants.SOFT_RESET_PACKET);
		listeners.resetListeners();
	}

	@Override
    public void hardReset() {
		// request a reset
		ioioConnection.sendToIOIO(Constants.HARD_RESET_PACKET);
	}

	// Singleton? TBD
    public static IOIO getInstance() {
		if (singleton == null) {
			singleton = new IOIOImpl();
		}
		return singleton;
	}

	public void registerListener(IOIOPacketListener listener){
	    listeners.registerListener(listener);
	}

	@Override
    public void disconnect() {
	    ioioConnection.disconnect();
	    try {
            ioioConnection.join();
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
	}

	// TODO(TF): support other modes.
	@Override
    public DigitalInput openDigitalInput(int pin) {
		return new DigitalInput(this, pin, DigitalInput.PULL_DOWN);
	}

	@Override
    public DigitalOutput openDigitalOutput(int pin, boolean enableOpenDrain) {
		return new DigitalOutput(this, pin, enableOpenDrain);
	}

	@Override
    public AnalogInput openAnalogInput(int pin) {
		return new AnalogInput(this, pin);
	}

	@Override
    public PwmOutputImpl openPwmOutput(int pin, boolean enableOpenDrain, int freqHz) throws OutOfResourceException {
		return new PwmOutputImpl(this, pin, freqHz, enableOpenDrain);
	}

	@Override
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
