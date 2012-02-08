package ioio.lib.util;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;

public class BaseIOIOLooper implements IOIOLooper {
	protected IOIO ioio_;

	@Override
	public final void setup(IOIO ioio) throws ConnectionLostException,
			InterruptedException {
		ioio_ = ioio;
		setup();
	}

	protected void setup() throws ConnectionLostException, InterruptedException {
	}

	@Override
	public void loop() throws ConnectionLostException, InterruptedException {
		Thread.sleep(20);
	}

	@Override
	public void disconnected() {
	}

	@Override
	public void incompatible() {
	}
}