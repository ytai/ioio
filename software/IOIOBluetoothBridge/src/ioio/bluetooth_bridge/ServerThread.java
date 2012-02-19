package ioio.bluetooth_bridge;

import java.io.IOException;
import java.io.InputStream;
import java.net.ServerSocket;
import java.net.Socket;

import android.util.Log;

abstract class ServerThread extends Thread {
	private ServerSocket mServer;
	private Socket mSocket;
	private boolean mStop = false;
	private byte[] mBuf = new byte[1024];

	protected void connected() {
	}

	protected void disconnected() {
	}

	protected void process(byte[] buf, int size) {
	}

	public ServerThread(ServerSocket server) {
		mServer = server;
	}

	@Override
	public final void run() {
		while (!mStop) {
			try {
				Log.i("ServerThread", "accepting");
				mSocket = mServer.accept();
				Log.i("ServerThread", "accepted");
				connected();
				InputStream in = mSocket.getInputStream();
				int size;
				while ((size = in.read(mBuf)) != -1) {
					process(mBuf, size);
				}
			} catch (IOException e) {
				Log.e("ServerThread", "Exception caught", e);
			} finally {
				if (!mSocket.isClosed()) {
					try {
						mSocket.close();
					} catch (IOException e) {
					}
				}
				disconnected();
			}
		}
	}
	
	public final boolean isConnected() {
		return mSocket.isConnected();
	}
	
	public final void write(byte[] data, int size) throws IOException {
		mSocket.getOutputStream().write(data, 0, size);
	}

	public final void kill() {
		try {
			mStop = true;
			if (!mSocket.isOutputShutdown()) {
				mSocket.shutdownOutput();
			}
			if (!mSocket.isInputShutdown()) {
				mSocket.shutdownInput();
			}
			if (!mServer.isClosed()) {
				mServer.close();
			}
		} catch (Exception e) {
		}
		try {
			join();
		} catch (InterruptedException e) {
		}
	}
}
