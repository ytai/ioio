package ioio.lib.impl;

import ioio.lib.api.exception.ConnectionLostException;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

import android.util.Log;

public class SocketIOIOConnection implements IOIOConnection {
	private int port_;
	private ServerSocket server_ = null;
	private Socket socket_ = null;
	private boolean disconnect_ = false;
	private boolean server_owned_by_connect_ = true;
	private boolean socket_owned_by_connect_ = true;
	
	public SocketIOIOConnection(int port) {
		port_ = port;
	}

	@Override
	public void waitForConnect() throws ConnectionLostException {
		try {
			synchronized (this) {
				if (disconnect_) {
					throw new ConnectionLostException();
				}
				Log.d("SocketIOIOConnection", "Creating server socket");
				server_ = new ServerSocket(port_);
				server_owned_by_connect_ = false;
			}
			Log.d("SocketIOIOConnection", "Waiting for TCP connection");
			socket_ = server_.accept();
			Log.d("SocketIOIOConnection", "TCP connected");
			synchronized (this) {
				if (disconnect_) {
					socket_.close();
					throw new ConnectionLostException();
				}
				socket_owned_by_connect_ = false;
			}
		} catch (IOException e) {
			synchronized (this) {
				disconnect_ = true;
				if (server_owned_by_connect_ && server_ != null) {
					try {
						server_.close();
					} catch (IOException e1) {
						Log.e("SocketIOIOConnection", "Unexpected exception", e1);
					}
				}
				if (socket_owned_by_connect_ && socket_ != null) {
					try {
						socket_.close();
					} catch (IOException e1) {
						Log.e("SocketIOIOConnection", "Unexpected exception", e1);
					}
				}
				throw new ConnectionLostException();
			}
		}
	}

	@Override
	synchronized public void disconnect() {
		if (disconnect_) {
			return;
		}
		Log.d("SocketIOIOConnection", "Client initiated disconnect");
		disconnect_ = true;
		if (!server_owned_by_connect_) {
			try {
				server_.close();
			} catch (IOException e1) {
				Log.e("SocketIOIOConnection", "Unexpected exception", e1);
			}
		}
		if (!socket_owned_by_connect_) {
			try {
				socket_.close();
			} catch (IOException e1) {
				Log.e("SocketIOIOConnection", "Unexpected exception", e1);
			}
		}
	}

	@Override
	public InputStream getInputStream() throws ConnectionLostException {
		try {
			return socket_.getInputStream();
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	public OutputStream getOutputStream() throws ConnectionLostException {
		try {
			return socket_.getOutputStream();
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

}
