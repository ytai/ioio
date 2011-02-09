package org.ioio;
/**
 * Represent the generic digital I/O for the IOIO board.
 * 
 * @author arshan
 *
 */
public class DigitalIO extends IOIOPin {

	boolean shadowState = false;

	private IOIOApi ioio;
	
	public DigitalIO(IOIOApi ioio, int pin) {
		super(pin);
		this.ioio = ioio;
	}
	
	public void write(boolean val) throws IOIOException {			
		shadowState = val;
		
	}
	
	public boolean read() throws IOIOException{
		return shadowState;
	}
	
	// Set this pin as an output
	public void setOutput(boolean val) {		
//		ioio.setPinValue(pin, val, this);
	}
	
	public void setOpenDrain(boolean val) {
//		ioio.setPinState(pin, isOutput(), val);
	}
	
	public void setPullUp(boolean val) {}
	 
}
