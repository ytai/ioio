package ioio.lib;

import ioio.lib.IOIOException.ConnectionLostException;
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

	public Input<Boolean> openDigitalInput(int pin) throws ConnectionLostException;

    public Output<Boolean> openDigitalOutput(int pin, boolean enableOpenDrain) throws ConnectionLostException;

    public Output<Boolean> openDigitalOutput(int pin, boolean enableOpenDrain, boolean startValue) throws ConnectionLostException;

	public Input<Float> openAnalogInput(int pin) throws ConnectionLostException;

	public PwmOutput openPwmOutput(int pin, boolean enableOpenDrain, int freqHz) throws OutOfResourceException, ConnectionLostException;

	/** TODO: test support for this */
	public Uart openUart(int rx, int tx, int baud, int parity, float stopbits) throws ConnectionLostException;
}
