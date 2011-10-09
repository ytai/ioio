package ioio.lib.util;

import java.io.InputStream;
import java.io.OutputStream;

import android.os.Looper;
import android.util.Log;

import ioio.lib.api.IOIOConnection;
import ioio.lib.api.exception.ConnectionLostException;

public class IOIOConnectionMultiplexer implements IOIOConnection {
	enum ConnectionState {
		WAITING, CONNECTED, FAILED
	}

	private static final String TAG = "IOIOConnectionMultiplexer";

	private final ConnectionThread[] threads_;
	private IOIOConnection chosen_ = null;
	private boolean disconnect_ = false;

	public IOIOConnectionMultiplexer(IOIOConnection[] connections) {
		threads_ = new ConnectionThread[connections.length];
		for (int i = 0; i < connections.length; ++i) {
			threads_[i] = new ConnectionThread(connections[i]);
		}
	}

	@Override
	public void waitForConnect() throws ConnectionLostException {
		for (ConnectionThread thread : threads_) {
			thread.start();
		}
		try {
			waitForChosenConnection();
			Log.d(TAG, "Joining");
			joinAll();
			Log.d(TAG, "All threads joined!");
		} catch (InterruptedException e) {
			throw new ConnectionLostException(e);
		}
		// all threads should have exited by now.
	}

	private synchronized void waitForChosenConnection()
			throws ConnectionLostException, InterruptedException {
		while (!disconnect_) {
			if (allDead()) {
				throw new ConnectionLostException();
			}
			chosen_ = getFirstConnected();
			if (chosen_ != null) {
				Log.d(TAG, "Connected");
				// disconnect all the others
				for (ConnectionThread thread : threads_) {
					if (thread.getConnection() != chosen_) {
						thread.getConnection().disconnect();
					}
				}
				return;
			}
			try {
				wait();
			} catch (InterruptedException e) {
			}
		}
		if (disconnect_) {
			throw new ConnectionLostException();
		}
	}

	private void joinAll() throws InterruptedException {
		for (Thread t : threads_) {
			t.join();
			Log.d(TAG, "joined 1");
		}
	}

	private IOIOConnection getFirstConnected() {
		for (ConnectionThread thread : threads_) {
			if (thread.getConnectionState() == ConnectionState.CONNECTED) {
				return thread.getConnection();
			}
		}
		return null;
	}

	private boolean allDead() {
		for (ConnectionThread thread : threads_) {
			if (thread.getConnectionState() != ConnectionState.FAILED) {
				return false;
			}
		}
		return true;
	}

	@Override
	public synchronized void disconnect() {
		if (disconnect_) {
			return;
		}
		disconnect_ = true;
		for (ConnectionThread thread : threads_) {
			thread.getConnection().disconnect();
		}
		notifyAll();
	}

	@Override
	public InputStream getInputStream() throws ConnectionLostException {
		return chosen_.getInputStream();
	}

	@Override
	public OutputStream getOutputStream() throws ConnectionLostException {
		return chosen_.getOutputStream();
	}

	private class ConnectionThread extends Thread {
		private ConnectionState state_ = ConnectionState.WAITING;

		private final IOIOConnection connection_;

		ConnectionThread(IOIOConnection con) {
			connection_ = con;
		}

		@Override
		public void run() {
			Looper.prepare();
			try {
				Log.v(TAG, "Waiting for connection: " + connection_);
				connection_.waitForConnect();
				Log.v(TAG, "Connection: " + connection_ + " connected");
				state_ = ConnectionState.CONNECTED;
			} catch (ConnectionLostException e) {
				Log.v(TAG, "Connection: " + connection_ + " disconnected / failed");
				state_ = ConnectionState.FAILED;
			}
			synchronized (IOIOConnectionMultiplexer.this) {
				IOIOConnectionMultiplexer.this.notifyAll();
			}
			Log.v(TAG, "ConnectionThread exiting");
		}

		ConnectionState getConnectionState() {
			return state_;
		}

		IOIOConnection getConnection() {
			return connection_;
		}
	}

}
