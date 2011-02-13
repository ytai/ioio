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
	// ytai: this only applies for input pins (and the write would be done behind-the-scenes
	//       by the protocol listener. let's first un-tangle the solution to the common-interface
	//       problem below...
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
	// ytai: good use-case, which indeed raises a good design challenge. however,
	//       from this design it seems like the code would look somthing like
	//       (not using the exact names):
	//       DigitalOutput out = ioio.openPinAsDigitalOutput(18);
	//       out.set(true);
	//       out.set(false);
	//       out.setMode(INPUT);
	//       boolean value = out.get()  // huh? reading from output?
	//      
	//       Perhaps the generic Pin interface should have a set of methods like:
	//       DigitalInput reopenAsDigitalInput(PullMode pull);
	//       DigitalOutput repoenAsDigitalOutput(boolean openDrain);
	//       AnalogInput reopenAsAnalogInput()
	//       etc.
	//       This is somewhat like close() and openXXX() in one operation.
	//
	//       Another approach is to stuff EVERYTHING in one interface and having a setMode()
	//       In this case, whatever mode you're in, most methods will throw a WrongModeException
	//       except for the few that match the mode. It is not very clean, but I don't
	//       Completely disqualify this option.
	
	// Set this pin as an output
	public void setOutput(boolean val) {		
//		ioio.setPinValue(pin, val, this);
	}
	
	// ytai: at the protocol level, open-drain for output and pull-up for input
	// are established when you set the pin as input/output. no separate message.
	// i think it is reasonable to determine it on construction and have it final.
	// arshan: good point, of course we can easily hide that from user at this api level. 
	//         I can change the state of the pin on demand
	// ytai: if the default is false, it might put your circuit at an invalid state
	//       until you set it to true.
	//       if the default is true, people will often forget to set it to false and
	//       will be frustrated with why things aren't working.
	//       so the best would be to force this value in the ctor (as well as initial
	//       value for output).
	//       the firmware makes sure that changing the pin settings will be done in the
	//       right order, so that there isn't even a short period of invalid state.
	public void setOpenDrain(boolean val) {
//		ioio.setPinState(pin, isOutput(), val);
	}
	
	public void setPullUp(boolean val) {}
	 
}
