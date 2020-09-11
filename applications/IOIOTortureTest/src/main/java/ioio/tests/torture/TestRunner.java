package ioio.tests.torture;

import ioio.lib.api.exception.ConnectionLostException;

interface TestRunner {
	void run() throws ConnectionLostException, InterruptedException;
	String testClassName();
}
