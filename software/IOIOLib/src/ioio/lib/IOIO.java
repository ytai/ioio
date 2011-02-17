package ioio.lib;

import ioio.lib.IOIOException.OperationAbortedException;
import ioio.lib.IOIOException.OutOfResourceException;
import ioio.lib.pic.Uart;

/**
 * An interface for controlling the IOIO board.
 *
 * @author arshan
 * @author birmiwal
 */
public interface IOIO {

	public void waitForConnect() throws OperationAbortedException;

	public void disconnect();

	public void abortConnection();

	public boolean isConnected();

	public void softReset();

	public void hardReset();

	public Input<Boolean> openDigitalInput(int pin);

	public Output<Boolean> openDigitalOutput(int pin, boolean enableOpenDrain);

	public Input<Float> openAnalogInput(int pin);

	public PwmOutput openPwmOutput(int pin, boolean enableOpenDrain, int freqHz) throws OutOfResourceException;

	/** TODO: test support for this */
	public Uart openUart(int rx, int tx, int baud, int parity, float stopbits);
}
