package ioio.tests.torture;

import ioio.lib.api.exception.ConnectionLostException;

interface TestRunner {
	public void run() throws ConnectionLostException, InterruptedException;
	public String testClassName();
}
