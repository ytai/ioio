package ioio.lib.util;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;

public abstract class IOIOLooperAlt implements IOIOLooper {

	private IOIO ioio;

	@Override
	public void setup(IOIO ioio) throws ConnectionLostException,
			InterruptedException {
		this.ioio = ioio;
	}

	@Override
	public void loop() throws ConnectionLostException, InterruptedException {
		Thread.sleep(20);
		loop(this.ioio);
	}

	protected abstract void loop(IOIO ioio) throws ConnectionLostException, InterruptedException;

	@Override
	public void disconnected() {
		disconnected(this.ioio);

	}
	protected void disconnected(IOIO ioio) {}
	
	@Override
	public void incompatible() {}

	@Override
	public void incompatible(IOIO ioio) {}


}
