package ioio.lib.impl;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.DigitalOutputSpec;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.InvalidOperationException;
import ioio.lib.api.exception.InvalidStateException;
import android.util.Log;

/**
 * Represent and manage digital output pins on the IOIO.
 *
 * TODO(arshan): implement and add PinChangeListener
 * TODO(arshan): DigitalIO class that supports changing mode while in use.
 *
 * @author arshan
 */
public class IOIODigitalOutput extends IOIOPin implements IOIOPacketListener, DigitalOutput {

	IOIOImpl ioio;

	// Only true when we are confirmed active from the IOIO
	// looks like we can Q requests anyway as they are in order.
	boolean active = false;

	// Keep a local version of the state, not sure its necessary.
	// TODO(arshan): get rid of shadowState
	Boolean shadowState = false;

	// cache most used packets
	public final IOIOPacket setHi;
	public final IOIOPacket setLo;

	IOIODigitalOutput(IOIOImpl ioio, PacketFramerRegistry framerRegistry, DigitalOutputSpec spec, boolean startValue)
	throws ConnectionLostException, InvalidOperationException {
		super(spec.pin);
		this.shadowState = startValue;
		this.ioio = ioio;
		ioio.reservePin(pin);
		framerRegistry.registerFramer(Constants.SET_OUTPUT, SET_DIGITAL_OUTPUT_PACKET_FRAMER);

		setHi = new IOIOPacket(Constants.SET_VALUE, new byte[]{(byte)(pin<<2|1)});
		setLo = new IOIOPacket(Constants.SET_VALUE, new byte[]{(byte)(pin<<2)});

		ioio.registerListener(this);
		init(spec.mode);
	}

	private void init(DigitalOutputSpec.Mode mode) throws ConnectionLostException {
		// TODO(arshan): does this need a sanity check?
		IOIOPacket request_output =
			new IOIOPacket(
			  Constants.SET_OUTPUT,
			  new byte[]{(byte) (pin << 2
					  | (shadowState?1:0) << 1
					  | (DigitalOutputSpec.Mode.OPEN_DRAIN.equals(mode) ? 1:0))}
			);
		ioio.sendPacket(request_output);
	}

	@Override
    public void write(Boolean val) throws InvalidStateException, ConnectionLostException {
       if (isInvalid()) {
            throw new InvalidStateException("");
       }

		if (!active) {
			// TODO(arshan): need a policy for this, not likely to come up, but ...
			// throw new IOException("output not yet active");
			// or maybe it will come up
			Log.i("IOIO output", "using digital output before confirmation");
		}

		if (val != shadowState) {
			shadowState = val;
			if (val) {
				//Log.i("IOIO output", "pin " + pin + " is set high");
				ioio.sendPacket(setHi);
			}
			else {
				// Log.i("IOIO output", "pin " + pin + " is set low");
				ioio.sendPacket(setLo);
			}

		}
	}

	@Override
    public void handlePacket(IOIOPacket packet) {
		switch (packet.message) {
			case Constants.SET_OUTPUT:
			    if (packet.payload[0] >> 2 == pin) {
			        active = true;
			       // Log.i("IOIO","pin " + pin + " set as output");
			    }
		}
	}

    @Override
    public void close() {
        ioio.releasePin(pin);
        ioio.unregisterListener(this);
    }

    private static final PacketFramer SET_DIGITAL_OUTPUT_PACKET_FRAMER =
        PacketFramers.getNBytePacketFramerFor(Constants.SET_OUTPUT, 1);
}
