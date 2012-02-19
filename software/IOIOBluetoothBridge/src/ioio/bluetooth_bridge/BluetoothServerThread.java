package ioio.bluetooth_bridge;

import java.io.IOException;
import java.io.InputStream;

import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

public class BluetoothServerThread extends Thread {
	private BluetoothServerSocket mServer;
	private BluetoothSocket mSocket;
	private boolean mStop = false;
	private boolean mConnected = false;
	private byte[] mBuf = new byte[1024];

	protected void connected() {
	}

	protected void disconnected() {
	}

	protected void process(byte[] buf, int size) {
	}

	public BluetoothServerThread(BluetoothServerSocket server) {
		mServer = server;
	}

	@Override
	public final void run() {
		while (!mStop) {
			try {
				Log.i("ServerThread", "accepting");
				mSocket = mServer.accept();
				Log.i("ServerThread", "accepted");
				mConnected = true;
				connected();
				InputStream in = mSocket.getInputStream();
				int size;
				while ((size = in.read(mBuf)) != -1) {
					process(mBuf, size);
				}
			} catch (IOException e) {
				Log.e("ServerThread", "Exception caught", e);
			} finally {
				try {
					mConnected = false;
					mSocket.close();
				} catch (IOException e) {
				}
				disconnected();
			}
		}
	}

	public final boolean isConnected() {
		return mConnected;
	}
	
	public final void write(byte[] data, int size) throws IOException {
		mSocket.getOutputStream().write(data, 0, size);
	}

	public final void kill() {
		try {
			mStop = true;
			mSocket.getOutputStream().close();
			mSocket.getInputStream().close();
			mServer.close();
		} catch (Exception e) {
		}
		try {
			join();
		} catch (InterruptedException e) {
		}
	}
}
