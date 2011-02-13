package ioio.lib;

import java.io.IOException;

import android.util.Log;

// TODO(arshan): implement and add PinChangeListener
public class DigitalOutput extends IOIOPin implements IOIOPacketListener {

	public static final int SOURCE = 0;
	public static final int SINK = 1;
	
	IOIO ioio;

	// Only true when we are confirmed active from the IOIO,
	// Im pretty sure ytai will object, but seems prudent for the async behaviour
	boolean active = false;
	
	boolean shadowState = false;

	// cache most used packets
	public final IOIOPacket setHi;
	public final IOIOPacket setLo;
	
	// package private
	DigitalOutput(IOIO ioio, int pin, int openDrain) {
		super(pin);
		this.ioio = ioio;
		
		setHi = new IOIOPacket(IOIOApi.SET_VALUE, new byte[]{(byte)(pin<<2|1)});
		setLo = new IOIOPacket(IOIOApi.SET_VALUE, new byte[]{(byte)(pin<<2)});
	
		ioio.registerListener(this);
		
		init(openDrain);
	}

	private void init(int openDrain) {
		// TODO(arshan): does this need a sanity check?
		IOIOPacket request_output = 
			new IOIOPacket(
			  IOIOApi.SET_OUTPUT,
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
			case IOIOApi.SET_OUTPUT:
				active = true;
				Log.i("IOIO","pin " + pin + " set as output");
		}
	}
}
