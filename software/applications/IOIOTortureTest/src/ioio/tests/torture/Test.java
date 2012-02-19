package ioio.tests.torture;

import ioio.lib.api.exception.ConnectionLostException;

interface Test<E> {
	E run() throws ConnectionLostException, InterruptedException;
}
