/**
 * TODO(TF): What is the copyright info? all the files need it
 */
package ioio.lib.pic;

import android.accounts.OperationCanceledException;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

import ioio.lib.DigitalInputMode;
import ioio.lib.DigitalOutputMode;
import ioio.lib.IOIO;
import ioio.lib.IOIOException.ConnectionLostException;
import ioio.lib.IOIOException.InvalidOperationException;
import ioio.lib.IOIOException.OperationAbortedException;
import ioio.lib.IOIOException.OutOfResourceException;
import ioio.lib.IOIOException.SocketException;
import ioio.lib.Input;
import ioio.lib.Output;
import ioio.lib.PwmOutput;

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
    // pin 0 - onboard LED; pins 1-48 physical external pins
    private ModuleAllocator PINS = new ModuleAllocator(49);

    private final PacketFramerRegistry framerRegistry = new PacketFramerRegistry();

    private static final int CONNECT_WAIT_TIME_MS = 100;

    private static final DigitalInputMode DEFAULT_DIGITAL_INPUT_MODE = DigitalInputMode.PULL_DOWN;

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
	public void queuePacket(IOIOPacket pkt) throws ConnectionLostException {
		ioioConnection.sendToIOIO(pkt);
	}

	/**
	 * Blocking call that will not return until connected.
	 * hmm. this must throw an exception at some point, else very un-androidy
	 * @throws SocketException
	 * @throws OperationCanceledException if {@link #abortConnection()} is called
	 */
	@Override
    public void waitForConnect() throws OperationAbortedException, SocketException {
	    abortConnection = false;

	    // TODO(birmiwal): throw exception if already connected?
	    if (!isConnected()) {
	        try {
                ioioConnection.start(framerRegistry);
            } catch (BindException e) {
                e.printStackTrace();
                throw new SocketException("BindException: " + e.getMessage());
            } catch (IOException e) {
                e.printStackTrace();
                throw new SocketException("IOException: " + e.getMessage());
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
	public void softReset() throws ConnectionLostException {
		ioioConnection.sendToIOIO(Constants.SOFT_RESET_PACKET);
		listeners.disconnectListeners();
	}

	@Override
    public void hardReset() throws ConnectionLostException {
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
	    abortConnection = true;
	    ioioConnection.disconnect();
	}

	// TODO(TF): support other modes.
	@Override
    public Input<Boolean> openDigitalInput(int pin) throws ConnectionLostException, InvalidOperationException {
		return openDigitalInput(pin, DEFAULT_DIGITAL_INPUT_MODE);
	}

    @Override
    public Output<Boolean> openDigitalOutput(int pin, boolean startValue) throws ConnectionLostException, InvalidOperationException {
        return openDigitalOutput(pin, startValue, DigitalOutputMode.NORMAL);
    }

    @Override
    public Output<Boolean> openDigitalOutput(int pin, boolean startValue, DigitalOutputMode mode) throws ConnectionLostException, InvalidOperationException {
        return new DigitalOutput(this, framerRegistry, pin, mode, startValue);
    }

	@Override
    public Input<Float> openAnalogInput(int pin) throws ConnectionLostException, InvalidOperationException {
		return new AnalogInput(this, pin, framerRegistry);
	}

	@Override
    public PwmOutput openPwmOutput(int pin, boolean enableOpenDrain, int freqHz)
	throws OutOfResourceException, ConnectionLostException, InvalidOperationException {
		return new PwmOutputImpl(this, pin, freqHz, enableOpenDrain);
	}

	@Override
    public Uart openUart(int rx, int tx, int baud, int parity, float stopbits)
	throws ConnectionLostException, InvalidOperationException {
		return new Uart(this, nextAvailableUart(), rx, tx, baud, parity, stopbits);
	}

	/**
	 * @return the next available uart module
	 */
	private int nextAvailableUart() {
		return 0; // support just the one for now.
	}

    public void reservePin(int pin) throws InvalidOperationException {
        boolean allocated = PINS.requestAllocate(pin);
        if (!allocated) {
            throw new InvalidOperationException("Pin " + pin + " already open?");
        }
    }

    public void releasePin(int pin) {
        PINS.releaseModule(pin);
    }

    @Override
    public Input<Boolean> openDigitalInput(int pin, DigitalInputMode mode)
            throws ConnectionLostException, InvalidOperationException {
        return new DigitalInput(this, framerRegistry, pin, mode);
    }
}
