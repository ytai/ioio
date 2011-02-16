package ioio.lib;

import ioio.lib.IOIOException.OperationAbortedException;

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

	public DigitalInput openDigitalInput(int pin);

	public DigitalOutput openDigitalOutput(int pin, boolean enableOpenDrain);

	public AnalogInput openAnalogInput(int pin);

	public PwmOutput openPwmOutput(int pin, int module, int periodUs);

	public Uart openUart(int rx, int tx, int baud, int parity, float stopbits);
}
