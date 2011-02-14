package ioio.lib;

/**
 * Interface for the IOIO proto.
 *
 * @author arshan
 */
public interface IOIO {

	public void connect();

	public void disconnect();

	public void abortConnection();

	public boolean isConnected();

	public void softReset();

	public void hardReset();

	public DigitalInput openDigitalInput(int pin);

	public DigitalOutput openDigitalOutput(int pin);

	public AnalogInput openAnalogInput(int pin);

	public PwmOutput openPwmOutput(int pin);

	public Uart openUart(int rx, int tx, int baud, int parity, float stopbits);
}
