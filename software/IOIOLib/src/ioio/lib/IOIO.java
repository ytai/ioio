package ioio.lib;

import ioio.lib.IOIOException.OperationAbortedException;
import ioio.lib.IOIOException.OutOfResourceException;
import ioio.lib.pic.PwmOutputImpl;
import ioio.lib.pic.Uart;

/**
 * Interface for the IOIO proto.
 *
 * @author arshan
 */
public interface IOIO {

	public void waitForConnect() throws OperationAbortedException;

	public void disconnect();

	public void abortConnection();

	public boolean isConnected();

	public void softReset();

	public void hardReset();

	public Input<Boolean> openDigitalInput(int pin);

	public InOut<Boolean> openDigitalOutput(int pin, boolean enableOpenDrain);

	public Input<Float> openAnalogInput(int pin);

	public PwmOutputImpl openPwmOutput(int pin, int periodUs) throws OutOfResourceException;

	/** TODO: test support for this */
	public Uart openUart(int rx, int tx, int baud, int parity, float stopbits);
}
