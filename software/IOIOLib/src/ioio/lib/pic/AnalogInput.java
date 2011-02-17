package ioio.lib.pic;

import android.util.Log;

import ioio.lib.IOIOException.ConnectionLostException;
import ioio.lib.IOIOException.InvalidStateException;
import ioio.lib.Input;

/**
 * Represent and manage analog input pins on the IOIO.
 *
 * @author arshan
 */
public class AnalogInput extends IOIOPin implements IOIOPacketListener, Input<Float> {

	IOIOImpl ioio;
	int value = 0;

	int counter = 0;

	boolean active = false;
	private int reportPin = 0;

	public AnalogInput(IOIOImpl ioio, int pin) throws ConnectionLostException {
		super(pin);
		this.ioio = ioio;
		ioio.registerListener(this);
        init();
	}

	private void init() throws ConnectionLostException {
		ioio.queuePacket(new IOIOPacket(
				Constants.SET_ANALOG_INPUT,
				new byte[]{(byte)pin}
		));
	}

	// TODO(TF): decide on units, mV? let the user set them?
	@Override
    public Float read() throws InvalidStateException {
	    if (isInvalid()) {
	        throw Constants.INVALID_STATE_EXCEPTION;
	    }
		return value / 1023.0f;
	}

	@Override
    public void handlePacket(IOIOPacket packet) {
		switch (packet.message){
		case Constants.SET_ANALOG_INPUT:
			if (packet.payload[0] == pin) {
				active = true;
			}
			break;

		case Constants.REPORT_ANALOG_FORMAT:
            Log.i("IOIO-ANALOG", "analog format packet: " + packet.toString());
			// Record where in the paylod my pin number is for future use.
			for (int x = 1; x < packet.payload.length; x++) {
				if (packet.payload[x] == pin) {
					reportPin = x-1;
					break;
				}
			}
			break;

		case Constants.REPORT_ANALOG_STATUS:
            if (packet.payload == null || packet.payload.length == 0) {
                Log.d("IOIO-ANALOG", "payload is strange");
                return;
            }
			// TODO(arshan): make these class vars.
			int offset = (reportPin / 4) * 5;
			int rem  = reportPin % 4;

			// MSB
			value = (packet.payload[offset + rem + 1]) << 2;
			// LSB
			value |= (packet.payload[offset] & (0x3 << (rem*2))) >> (rem * 2);
	        value &= 0x3ff;
			break;
		}
	}

    @Override
    public void close() {
        // TODO(TF): Implement this
    }
}
