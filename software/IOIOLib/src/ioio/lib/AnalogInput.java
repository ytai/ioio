package ioio.lib;

// TBI
public class AnalogInput extends IOIOPin implements IOIOPacketListener {

	IOIO ioio;
	float voltage = 0;
	boolean active = false;
	
	public AnalogInput(IOIO ioio, int pin) {
		super(pin);
		this.ioio = ioio;
		init();
	}
	
	private void init() {
		ioio.queuePacket(new IOIOPacket(
				IOIOApi.SET_ANALOG_INPUT,
				new byte[]{(byte)pin}
		));
	}

	public float read() {
		return voltage;
	}

	public void handlePacket(IOIOPacket packet) {
		switch (packet.message){
		case IOIOApi.SET_ANALOG_INPUT:
			active = true;
			break;
		}
	}
	
	
	
}
