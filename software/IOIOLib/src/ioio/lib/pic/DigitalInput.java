package ioio.lib.pic;

import android.util.Log;

import ioio.lib.DigitalInputMode;
import ioio.lib.IoioException.ConnectionLostException;
import ioio.lib.IoioException.InvalidOperationException;
import ioio.lib.IoioException.InvalidStateException;
import ioio.lib.Input;

/**
 * Represent and manage digital input pins on the IOIO.
 *
 * @author arshan
 */
public class DigitalInput extends IoioPin implements IoioPacketListener, Input<Boolean> {

	public static final int FLOATING = 0;
	public static final int PULL_UP = 1;
	public static final int PULL_DOWN = 2;

	private boolean active = false;
	private boolean state = false;

	IoioImpl ioio;

	DigitalInput(IoioImpl ioio, PacketFramerRegistry registry, int pin, DigitalInputMode inputMode)
	throws ConnectionLostException, InvalidOperationException {
		super(pin);
		this.ioio = ioio;
		ioio.reservePin(pin);
		ioio.registerListener(this);
		registry.registerFramer(Constants.SET_INPUT, SET_DIGITAL_INPUT_PACKET_FRAMER);
		registry.registerFramer(Constants.REPORT_DIGITAL_STATUS, REPORT_DIGITAL_STATUS_PACKET_FRAMER);
		registry.registerFramer(Constants.SET_CHANGE_NOTIFY, CHANGE_NOTIFY_HANDLER);
		init(inputMode);
	}

	private void init(DigitalInputMode mode) throws ConnectionLostException {
		ioio.sendPacket(new IoioPacket(
			Constants.SET_INPUT,
			new byte[]{ (byte)(pin << 2 | mode.getBitValue()) }
			));
		ioio.sendPacket(new IoioPacket(
			Constants.SET_CHANGE_NOTIFY,
			new byte[]{(byte)(pin<<2 | 1)}
		));
	}

	@Override
    public Boolean read() throws InvalidStateException {
       if (isInvalid()) {
            throw Constants.INVALID_STATE_EXCEPTION;
        }
		return state;
	}

	// TODO(arshan): consider centralizing this to a IOController
	// otherwise every ping has a case statement to run on every packet.
	// we have to register centrally anyway.
	@Override
    public void handlePacket(IoioPacket packet) {
		// TODO(arshan): is it active before the first report?
		switch(packet.message) {
		case Constants.SET_INPUT:
		    if (packet.payload[0] >> 2 == pin) {
		        active = true;
		        // Log.i("IOIO","pin " + pin + " set as input");
		    }
			break;
		case Constants.REPORT_DIGITAL_STATUS:
			if (active && packet.payload[0] >> 2 == pin) {
				state = ((packet.payload[0] & 0x1) == 0)? false : true;
				// Log.i("IOIO", "pin " + pin + " status is here : " + (state?"Hi":"Low"));
			}
			break;
		}

	}

    @Override
    public void close() {
        ioio.releasePin(pin);
        ioio.unregisterListener(this);
    }

    private static final PacketFramer CHANGE_NOTIFY_HANDLER = PacketFramers.getNBytePacketFramerFor(Constants.SET_CHANGE_NOTIFY, 1);

    private static final PacketFramer SET_DIGITAL_INPUT_PACKET_FRAMER =
        PacketFramers.getNBytePacketFramerFor(Constants.SET_INPUT, 1);

    private static final PacketFramer REPORT_DIGITAL_STATUS_PACKET_FRAMER =
        PacketFramers.getNBytePacketFramerFor(Constants.REPORT_DIGITAL_STATUS, 1);
}
