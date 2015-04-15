/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */
package ioio.lib.android.bluetooth;

import ioio.lib.api.IOIOConnection;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.FixedReadBufferedInputStream;

import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Build;
import android.util.Log;

public class BluetoothIOIOConnection implements IOIOConnection {
	private static final String TAG = "BluetoothIOIOConnection";
	private BluetoothSocket socket_ = null;
	private boolean disconnect_ = false;
	private final BluetoothDevice device_;
	private final String name_;
	private final String address_;
	private InputStream inputStream_;
	private OutputStream outputStream_;

	public BluetoothIOIOConnection(BluetoothDevice device) {
		device_ = device;
		name_ = device.getName();
		address_ = device.getAddress();
	}

	@Override
	public void waitForConnect() throws ConnectionLostException {
		synchronized (this) {
			if (disconnect_) {
				throw new ConnectionLostException();
			}
			try {
				socket_ = createSocket(device_);
			} catch (IOException e) {
				throw new ConnectionLostException(e);
			}
		}
		// keep trying to connect as long as we're not aborting
		while (true) {
			try {
				Log.v(TAG, "Attempting to connect to Bluetooth device: " + name_);
				inputStream_ = socket_.getInputStream();
				outputStream_ = socket_.getOutputStream();
				socket_.connect();
				Log.v(TAG, "Established connection to device " + name_
						+ " address: " + address_);
				break; // if we got here, we're connected
			} catch (Exception e) {
				if (disconnect_) {
					throw new ConnectionLostException(e);
				}
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e1) {
				}
			}
		}
		// Success! Wrap the streams with a properly sized buffers.
		inputStream_ = new FixedReadBufferedInputStream(inputStream_, 64);
		outputStream_ = new BufferedOutputStream(outputStream_, 1024);
	}

	public static BluetoothSocket createSocket(final BluetoothDevice device)
			throws IOException {
		if (Build.VERSION.SDK_INT >= 10 ) {
			// We're trying to create an insecure socket, which is only
			// supported in API 10 and up. Otherwise, we try a secure socket
			// which is in API 7 and up.
			return device.createInsecureRfcommSocketToServiceRecord(UUID
					.fromString("00001101-0000-1000-8000-00805F9B34FB"));
		} else {
			return device.createRfcommSocketToServiceRecord(UUID
					.fromString("00001101-0000-1000-8000-00805F9B34FB"));
		}
	}

	@Override
	public synchronized void disconnect() {
		if (disconnect_) {
			return;
		}
		Log.v(TAG, "Client initiated disconnect");
		disconnect_ = true;
		if (socket_ != null) {
			try {
				socket_.close();
			} catch (IOException e) {
			}
		}
	}

	@Override
	public InputStream getInputStream() throws ConnectionLostException {
		return inputStream_;
	}

	@Override
	public OutputStream getOutputStream() throws ConnectionLostException {
		return outputStream_;
	}

	@Override
	public boolean canClose() {
		return true;
	}
}
