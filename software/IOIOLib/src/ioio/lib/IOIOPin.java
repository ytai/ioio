package ioio.lib;

public class IOIOPin {
	
	protected int pin;
	
	public IOIOPin(int pin) {
		this.pin = pin;
	}
	/**
	 * Call when you are finished using the pin.
	 * This will return it to the pool of available connections.
	 */
	public void close() {
		
	}
}
