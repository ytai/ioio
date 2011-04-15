package ioio.lib.impl;

import java.io.InputStream;
import java.io.OutputStream;

import ioio.lib.api.exception.ConnectionLostException;

public interface IOIOConnection {
	void waitForConnect() throws ConnectionLostException;

	void disconnect();

	InputStream getInputStream() throws ConnectionLostException;

	OutputStream getOutputStream() throws ConnectionLostException;
}
