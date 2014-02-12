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

package ioio.lib.android.accessory;

import ioio.lib.android.accessory.Adapter.UsbAccessoryInterface;
import ioio.lib.api.IOIOConnection;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.FixedReadBufferedInputStream;
import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.spi.IOIOConnectionFactory;
import ioio.lib.spi.NoRuntimeSupportException;
import ioio.lib.util.android.ContextWrapperDependent;

import java.io.BufferedOutputStream;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Collection;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.ParcelFileDescriptor;
import android.util.Log;

public class AccessoryConnectionBootstrap extends BroadcastReceiver implements
		ContextWrapperDependent, IOIOConnectionBootstrap, IOIOConnectionFactory {
	private static final String ACTION_USB_PERMISSION = "ioio.lib.accessory.action.USB_PERMISSION";
	private static final String TAG = "AccessoryIOIOConnection";

	private enum State {
		CLOSED, WAIT_PERMISSION, OPEN
	}

	private enum InstanceState {
		INIT, CONNECTED, DEAD
	};

	private ContextWrapper activity_;
	private Adapter adapter_;
	private Adapter.AbstractUsbManager usbManager_;
	private Adapter.UsbAccessoryInterface accessory_;
	private State state_ = State.CLOSED;
	private PendingIntent pendingIntent_;
	private ParcelFileDescriptor fileDescriptor_;
	private InputStream inputStream_;
	private OutputStream outputStream_;

	public AccessoryConnectionBootstrap() throws NoRuntimeSupportException {
		adapter_ = new Adapter();
	}

	@Override
	public void onCreate(ContextWrapper wrapper) {
		activity_ = wrapper;
		usbManager_ = adapter_.getManager(wrapper);
		registerReceiver();
	}

	@Override
	public void onDestroy() {
		unregisterReceiver();
	}

	@Override
	public synchronized void open() {
		if (state_ != State.CLOSED) {
			return;
		}
		UsbAccessoryInterface[] accessories = usbManager_.getAccessoryList();
		accessory_ = (accessories == null ? null : accessories[0]);
		if (accessory_ != null) {
			if (usbManager_.hasPermission(accessory_)) {
				openStreams();
			} else {
				pendingIntent_ = PendingIntent.getBroadcast(activity_, 0, new Intent(
						ACTION_USB_PERMISSION), 0);
				usbManager_.requestPermission(accessory_, pendingIntent_);
				setState(State.WAIT_PERMISSION);
			}
		} else {
			Log.d(TAG, "No accessory found.");
		}
	}

	@Override
	public void reopen() {
		open();
	}

	@Override
	public synchronized void close() {
		if (state_ == State.OPEN) {
			closeStreams();
		} else if (state_ == State.WAIT_PERMISSION) {
			pendingIntent_.cancel();
		}
		setState(State.CLOSED);
	}

	@Override
	public IOIOConnection createConnection() {
		return new Connection();
	}

	private void registerReceiver() {
		IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
		filter.addAction(usbManager_.ACTION_USB_ACCESSORY_DETACHED);
		// filter.addAction(UsbManager.ACTION_USB_ACCESSORY_ATTACHED);
		activity_.registerReceiver(this, filter);
	}

	private void unregisterReceiver() {
		activity_.unregisterReceiver(this);
	}

	private void openStreams() {
		try {
			fileDescriptor_ = usbManager_.openAccessory(accessory_);
			if (fileDescriptor_ != null) {
				FileDescriptor fd = fileDescriptor_.getFileDescriptor();
				inputStream_ = new FileInputStream(fd);
				outputStream_ = new FileOutputStream(fd);
				setState(State.OPEN);
			} else {
				throw new IOException("Failed to open file descriptor");
			}
		} catch (IOException e) {
			Log.e(TAG, "Failed to open streams", e);
			setState(State.CLOSED);
		}
	}

	private void closeStreams() {
		try {
			fileDescriptor_.close();
		} catch (IOException e) {
			Log.e(TAG, "Failed to proprly close accessory", e);
		}
	}

	@Override
	public synchronized void onReceive(Context context, Intent intent) {
		String action = intent.getAction();
		if (usbManager_.ACTION_USB_ACCESSORY_DETACHED.equals(action)) {
			close();
			// } else if
			// (UsbManager.ACTION_USB_ACCESSORY_ATTACHED.equals(action)) {
			// open();
		} else if (ACTION_USB_PERMISSION.equals(action)) {
			if (intent.getBooleanExtra(usbManager_.EXTRA_PERMISSION_GRANTED, false)) {
				openStreams();
			} else {
				Log.e(TAG, "Permission denied");
				setState(State.CLOSED);
			}
		}
	}

	private void setState(State state) {
		state_ = state;
		notifyAll();
	}

	private class Connection implements IOIOConnection {
		private InstanceState instanceState_ = InstanceState.INIT;
		private InputStream localInputStream_;
		private OutputStream localOutputStream_;

		@Override
		public InputStream getInputStream() throws ConnectionLostException {
			return localInputStream_;
		}

		@Override
		public OutputStream getOutputStream() throws ConnectionLostException {
			return localOutputStream_;
		}

		@Override
		public boolean canClose() {
			return false;
		}

		@Override
		public void waitForConnect() throws ConnectionLostException {
			synchronized (AccessoryConnectionBootstrap.this) {
				if (instanceState_ != InstanceState.INIT) {
					throw new IllegalStateException("waitForConnect() may only be called once");
				}
				while (instanceState_ != InstanceState.DEAD && state_ != State.OPEN) {
					try {
						AccessoryConnectionBootstrap.this.wait();
					} catch (InterruptedException e) {
					}
				}
				if (instanceState_ == InstanceState.DEAD) {
					throw new ConnectionLostException();
				}
				// Apparently, some Android devices (e.g. Nexus 5) only support read operations of
				// multiples of the endpoint buffer size. So there you have it!
				localInputStream_ = new FixedReadBufferedInputStream(inputStream_, 1024);
				localOutputStream_ = new BufferedOutputStream(outputStream_, 1024);
			}
			try {
				while (instanceState_ != InstanceState.CONNECTED) {
					if (instanceState_ == InstanceState.DEAD || state_ != State.OPEN) {
						throw new ConnectionLostException();
					}
					// Soft-open the connection
					localOutputStream_.write(0x00);
					localOutputStream_.flush();
					// We're going to block now. We're counting on the IOIO to
					// write back a byte, or otherwise we're locked until
					// physical disconnection. This is a known OpenAccessory
					// bug:
					// http://code.google.com/p/android/issues/detail?id=20545
					if (localInputStream_.read() == 1) {
						instanceState_ = InstanceState.CONNECTED;
					} else {
						trySleep(1000);
					}
				}
			} catch (IOException e) {
				instanceState_ = InstanceState.DEAD;
				// It takes some time between the physical disconnection of
				// an accessory to when it is actually removed from the
				// list and reported to be detached. To avoid thrashing
				// during this period, we sleep after an IOException.
				trySleep(1000);
				throw new ConnectionLostException();
			}
		}

		@Override
		public void disconnect() {
			synchronized (AccessoryConnectionBootstrap.this) {
				instanceState_ = InstanceState.DEAD;
				AccessoryConnectionBootstrap.this.notifyAll();
			}
		}

		private void trySleep(long time) {
			synchronized (AccessoryConnectionBootstrap.this) {
				try {
					AccessoryConnectionBootstrap.this.wait(time);
				} catch (InterruptedException e) {
				}
			}
		}
	}

	@Override
	public void getFactories(Collection<IOIOConnectionFactory> result) {
		result.add(this);
	}

	@Override
	public String getType() {
		return Connection.class.getCanonicalName();
	}

	@Override
	public Object getExtra() {
		return null;
	}
}