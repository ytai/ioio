package ioio.lib;

import java.io.IOException;

import android.util.Log;

/**
 * Represent and manage digital output pins on the IOIO.
 * 
 * TODO(TF): implement and add PinChangeListener
 * TODO(arshan): DigitalIO class that supports changing mode while in use.
 * 
 * @author arshan
 */
public class DigitalOutput extends IOIOPin implements IOIOPacketListener {

	public static final int SOURCE = 0;
	public static final int SINK = 1;
	
	IOIOImplPic24f ioio;

	// Only true when we are confirmed active from the IOIO
	// looks like we can Q requests anyway as they are in order.
	boolean active = false;

	// Keep a local version of the state, not sure its necessary.
	// TODO(TF): should we set changeNotify on the pin and only update this on set from the IOIO? 
	boolean shadowState = false;

	// cache most used packets
	public final IOIOPacket setHi;
	public final IOIOPacket setLo;
	
	DigitalOutput(IOIOImplPic24f ioio, int pin, int openDrain) {
		super(pin);
		this.ioio = ioio;
		
		setHi = new IOIOPacket(Constants.SET_VALUE, new byte[]{(byte)(pin<<2|1)});
		setLo = new IOIOPacket(Constants.SET_VALUE, new byte[]{(byte)(pin<<2)});
	
		ioio.registerListener(this);
		
		init(openDrain);
	}

	private void init(int openDrain) {
		// TODO(arshan): does this need a sanity check?
		IOIOPacket request_output = 
			new IOIOPacket(
			  Constants.SET_OUTPUT,
			  new byte[]{(byte) (pin << 2
					  | (shadowState?1:0) << 1
					  | openDrain)}
			);
		ioio.queuePacket(request_output);
	}

	public void write(boolean val) throws IOException{
		if (!active) {
			// TODO(arshan): need a policy for this, not likely to come up, but ... 
			// throw new IOException("output not yet active");
			// or maybe it will come up
			Log.i("IOIO output", "using digital output before confirmation");
		}
		
		if (val != shadowState) {
			shadowState = val;
			if (val) {
				Log.i("IOIO output", "pin " + pin + " is set high");
				ioio.queuePacket(setHi);
			}
			else {
				Log.i("IOIO output", "pin " + pin + " is set low");
				ioio.queuePacket(setLo);
			}
			
		}
	}
	
	public boolean read() {
		return shadowState;
	}
	
	public void handlePacket(IOIOPacket packet) {
		switch (packet.message) {
			case Constants.SET_OUTPUT:
				active = true;
				Log.i("IOIO","pin " + pin + " set as output");
		}
	}
}
