package org.ioio;

// ytai: the package convention we use is ioio.something
// in this case ioio.lib or ioio.ioio_lib will work.

// ytai: you set the build target to 2.3. we want to support and device with
// API level 3 or more. better make a habit of building for those, assuming that
// the later ones are compatible.
/**
 * Interface to the IOIO board.
 * @author arshan
 *
 */
public interface IOIOApi {

	// ytai: add a constant for the on-board LED pin (number 0).
	public void connect();
	
	public void disconnect();

	public void softReset();
	
	public void hardReset();
	// ytai: wouldn't it be better to name them openXXX rather than getXXX, in order to:
	// a. emphasize the fact that you're acquiring a resource here that has been free so far an isn't anymore.
	// b. pair nicely with close().
	public DigitalInput getDigitalInput(int pin);
	
	public DigitalOutput getDigitalOutput(int pin);
	
	public AnalogInput getAnalogInput(int pin);

	// ytai: nit: camel case convention recommends getPwmOutput (i.e. don't capitalize the entire acronym).
	// same for UART. but I don't really care that much...
	public PWMOutput getPWMOutput(int pin);
	
	public UART openUART(int rx, int tx, int baud, int parity, int stopbits);
	
}
