package org.ioio;

/**
 * Interface to the IOIO board.
 * @author arshan
 *
 */
public interface IOIOApi {

	public void connect();
	
	public void disconnect();

	public void softReset();
	
	public void hardReset();
	
	public DigitalInput getDigitalInput(int pin);
	
	public DigitalOutput getDigitalOutput(int pin);
	
	public AnalogInput getAnalogInput(int pin);

	public PWMOutput getPWMOutput(int pin);
	
	public UART openUART(int rx, int tx, int baud, int parity, int stopbits);
	
}
