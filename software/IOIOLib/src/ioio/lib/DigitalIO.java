package ioio.lib;
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
	
	// ytai: should be the user's concern to cache outputs if they wish I think.
	// arshan: for efficiency we could keep shadowState here, and use the onChange. 
	public boolean read() throws IOIOException{
		return shadowState;
	}
	
	// ytai: how does this work? you get an output pin, set it to input,
	// and then want a DigitalInput, but can't have it...
	// arshan: override in Input to not allow setting output. 
	
	// ytai: what was wrong about having them completely separate?
	// half the methods of this interface will fail on input and the other will
	// fail on output. nothing in common really.
	// arshan: yah, I see your point, but I like the idea of supporting a DigitalIO, 
	// for the usecase of drive to value, then set as input and read. In which case
	// the DigitalIO would be the intersection of I and O. 
	
	// Set this pin as an output
	public void setOutput(boolean val) {		
//		ioio.setPinValue(pin, val, this);
	}
	
	// ytai: at the protocol level, open-drain for output and pull-up for input
	// are established when you set the pin as input/output. no separate message.
	// i think it is reasonable to determine it on construction and have it final.
	// arshan: good point, of course we can easily hide that from user at this api level. 
	//         I can change the state of the pin on demand
	public void setOpenDrain(boolean val) {
//		ioio.setPinState(pin, isOutput(), val);
	}
	
	public void setPullUp(boolean val) {}
	 
}
