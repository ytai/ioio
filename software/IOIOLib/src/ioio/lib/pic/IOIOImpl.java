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
import ioio.lib.Ioio;
import ioio.lib.IoioException.ConnectionLostException;
import ioio.lib.IoioException.InvalidOperationException;
import ioio.lib.IoioException.OperationAbortedException;
import ioio.lib.IoioException.OutOfResourceException;
import ioio.lib.IoioException.SocketException;
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
public class IoioImpl extends Service implements Ioio {
    // pin 0 - onboard LED; pins 1-48 physical external pins
    private ModuleAllocator PINS = getNewPinAllocation();

    private ModuleAllocator getNewPinAllocation() {
        return new ModuleAllocator(49);
    }

    private final PacketFramerRegistry framerRegistry = new PacketFramerRegistry();

    private static final int CONNECT_WAIT_TIME_MS = 100;

    // TODO(arshan): lets move this into something like IoioConfig
    private static final DigitalInputMode DEFAULT_DIGITAL_INPUT_MODE = DigitalInputMode.FLOATING;

	private final IoioConnection ioioConnection;

	private boolean abortConnection = false;

	ListenerManager listeners;

	public IoioImpl(IoioConnection ioio_connection, ListenerManager listeners) {
	    this.ioioConnection = ioio_connection;
	    this.listeners = listeners;
	}

	public IoioImpl(ListenerManager listeners) {
	    this(new IoioConnection(listeners), listeners);
	}

	/**
	 * Strict singleton? or just for convenience
	 * @param iostream
	 */
	public IoioImpl() {
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
	public void sendPacket(IoioPacket pkt) throws ConnectionLostException {
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
                PINS = getNewPinAllocation();
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

	public void registerListener(IoioPacketListener listener){
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
    public Uart openUart(int rx, int tx, int baud, int parity, int stopbits)
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

    // This is getting weird, every interested party registers a framer and listener
    // for the same packets. I think we should strip the complexity, consider 
    // more extensibility when things are more in place.
    public PacketFramerRegistry getFramerRegistry() {
        return this.framerRegistry;
    }
}
