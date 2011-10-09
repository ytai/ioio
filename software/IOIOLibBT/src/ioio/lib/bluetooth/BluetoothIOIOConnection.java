package ioio.lib.bluetooth;

import ioio.lib.api.IOIOConnection;
import ioio.lib.api.exception.ConnectionLostException;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

public class BluetoothIOIOConnection implements IOIOConnection {
	private static final String TAG = "BluetoothIOIOConnection";
	private BluetoothSocket socket_ = null;
	private boolean disconnect_ = false;
	private boolean socket_owned_by_connect_ = true;

	private BluetoothDevice findIOIO(BluetoothAdapter adapter)
			throws ConnectionLostException {
		try {
			Set<BluetoothDevice> bondedDevices = adapter.getBondedDevices();
			for (BluetoothDevice device : bondedDevices) {
				if (device.getName().startsWith("IOIO")) {
					return device;
				}
			}
		} catch (SecurityException e) {
			Log.e(TAG,
					"Did you forget to declare uses-permission of android.permission.BLUETOOTH?");
			throw e;
		}
		Log.w(TAG, "IOIO device not bound (paired)");
		throw new ConnectionLostException();
	}

	@Override
	public void waitForConnect() throws ConnectionLostException {
		final BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
		try {
			synchronized (this) {
				if (disconnect_) {
					throw new ConnectionLostException();
				}
				BluetoothDevice ioioDevice = findIOIO(adapter);
				// socket_ =
				// ioioDevice.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"));
				socket_ = ioioDevice
						.createInsecureRfcommSocketToServiceRecord(UUID
								.fromString("00001101-0000-1000-8000-00805F9B34FB"));
				socket_owned_by_connect_ = false;
			}
			// keep trying to connect as long as we're not aborting
			while (!disconnect_) {
				try {
					socket_.connect();
					break;  // if we got here, we're connected
				} catch (IOException e) {
					try {
						Thread.sleep(1000);
					} catch (InterruptedException e1) {
					}
				}
			}
		} catch (IOException e) {
			synchronized (this) {
				disconnect_ = true;
				throw new ConnectionLostException(e);
			}
		} catch (ConnectionLostException e) {
			disconnect_ = true;
			throw e;
		}
	}

	@Override
	public synchronized void disconnect() {
		if (disconnect_) {
			return;
		}
		Log.d(TAG, "Client initiated disconnect");
		disconnect_ = true;
		if (!socket_owned_by_connect_) {
			try {
				socket_.close();
			} catch (IOException e) {
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
