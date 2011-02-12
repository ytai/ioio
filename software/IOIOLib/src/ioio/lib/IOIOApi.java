package ioio.lib;

/**
 * Interface to the IOIO board.
 * 
 * @author arshan
 */
public interface IOIOApi {
	
	public void connect();
	
	public void disconnect();

	public void softReset();
	
	public void hardReset();

	public DigitalInput openDigitalInput(int pin);
	
	public DigitalOutput openDigitalOutput(int pin);
	
	public AnalogInput openAnalogInput(int pin);

	// ytai: nit: camel case convention recommends getPwmOutput (i.e. don't capitalize the entire acronym).
	// same for UART. but I don't really care that much...
	public PwmOutput openPwmOutput(int pin);
	
	public Uart openUart(int rx, int tx, int baud, int parity, int stopbits);
	
}
