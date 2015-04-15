/*
 * Copyright 2013 Ytai Ben-Tsvi. All rights reserved.
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
package ioio.lib.android.device;

import ioio.lib.api.IOIOConnection;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.FixedReadBufferedInputStream;
import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.spi.IOIOConnectionFactory;
import ioio.lib.spi.NoRuntimeSupportException;
import ioio.lib.util.android.ContextWrapperDependent;

import java.io.BufferedOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Collection;
import java.util.HashMap;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.os.Build;
import android.util.Log;

/**
 * A bootstrap class which allows connecting to a IOIO as a USB device from Android. Requires API-12
 * or higher to build and run. Will fail gracefully if runtime API is smaller.
 *
 * @author Misha Seltzer
 * @author Nadir Izrael
 * @author Ytai Ben-Tsvi
 */
public class DeviceConnectionBootstrap extends BroadcastReceiver implements
		ContextWrapperDependent, IOIOConnectionBootstrap {
	private static final String TAG = "DeviceConnectionBootstrap";
	private static final String ACTION_USB_PERMISSION = "ioio.lib.otg.action.USB_PERMISSION";

	private static final int REQUEST_TYPE = 0x21;
	private static final int SET_CONTROL_LINE_STATE = 0x22;

	private enum State {
		CLOSED, WAIT_DEVICE_ATTACHED, DEVICE_ATTACHED, WAIT_DEVICE_PERMITTED, DEVICE_ZOMBIE, DEVICE_OPEN
	}

	private enum Permission {
		UNKNOWN, PENDING, GRANTED, DENIED
	}

	// State-related signals.
	private State state_ = State.CLOSED;
	private boolean shouldOpen_ = false;
	private boolean shouldOpenDevice_ = false;
	private Permission permission_ = Permission.UNKNOWN;

	// Android-ism.
	private ContextWrapper activity_;
	private PendingIntent pendingIntent_;

	// USB device stuff.
	private UsbManager usbManager_;
	private UsbDevice device_;
	private UsbDeviceConnection connection_;
	private UsbInterface controlInterface_;
	private UsbInterface dataInterface_;
	private UsbEndpoint epIn_;
	private UsbEndpoint epOut_;
	private InputStream inputStream_;
	private OutputStream outputStream_;

	public DeviceConnectionBootstrap() {
		if (Build.VERSION.SDK_INT < 12) {
			throw new NoRuntimeSupportException("OTG is not supported on this device.");
		}
	}

	@Override
	public void getFactories(Collection<IOIOConnectionFactory> result) {
		result.add(new IOIOConnectionFactory() {
			@Override
			public String getType() {
				return DeviceConnectionBootstrap.class.getCanonicalName();
			}

			@Override
			public Object getExtra() {
				return null;
			}

			@Override
			public IOIOConnection createConnection() {
				return new Connection();
			}
		});
	}

	@Override
	public void onCreate(ContextWrapper wrapper) {
		Log.v(TAG, "onCreate()");
		activity_ = wrapper;
		usbManager_ = (UsbManager) wrapper.getSystemService(Context.USB_SERVICE);
		registerReceiver();
	}

	@Override
	public void onDestroy() {
		Log.v(TAG, "onDestroy()");
		unregisterReceiver();
	}

	@Override
	public synchronized void onReceive(Context context, Intent intent) {
		Log.v(TAG, "onReceive(" + intent + ")");
		String action = intent.getAction();
		if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)) {
			UsbDevice device = (UsbDevice) intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
			if (device.equals(device_)) {
				device_ = null;
				updateState();
			}
		} else if (UsbManager.ACTION_USB_DEVICE_ATTACHED.equals(action)) {
			updateState();
		} else if (ACTION_USB_PERMISSION.equals(action)) {
			permission_ = intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false) ? Permission.GRANTED
					: Permission.DENIED;
			updateState();
		}
	}

	@Override
	public synchronized void open() {
		Log.v(TAG, "open()");
		shouldOpen_ = true;
		updateState();
	}

	@Override
	public synchronized void close() {
		Log.v(TAG, "close()");
		shouldOpen_ = false;
		updateState();
	}

	@Override
	public void reopen() {
		open();
	}

	private void setState(State state) {
		state_ = state;
		Log.v(TAG, "state <= " + state);
	}

	/**
	 * Re-evaluate state.
	 *
	 * This is the main state machine governing the connection process with IOIO device. This is the
	 * only place where the {@link #state_} field may be changed. External events may update the
	 * control signals, then call {@link #updateState()} to act upon them. Those signals include:
	 * <dl>
	 * <dt>{@link #shouldOpen_}</dt>
	 * <dd>Updated by the {@link #open()}/{@link #close()} methods to designate whether our overall
	 * system should be active.</dd>
	 * <dt>{@link #shouldOpenDevice_}</dt>
	 * <dd>Updated by the connection methods to designate whether the client is interested in a
	 * connection to the device to be established.</dd>
	 * <dt>{@link #device_}</dt>
	 * <dd>Set whenever the device is attached and cleared whenever it is detached.</dd>
	 * <dt>{@link #permission_}</dt>
	 * <dd>Designates whether we have permission to talk to the IOIO, or we have a pending request,
	 * or we haven't checked yet.</dd>
	 * </dl>
	 * The state machine is organized such that the escalation in establishing a connection does not
	 * skip steps, but rather goes through every step, but will keep checking whether we should stay
	 * there within the same call. When there is nothing more to do, we set {@code done} to
	 * {@code true} and thus exit the loop.
	 *
	 * The same is true for closing the connection - we will go backwards in several steps until
	 * reaching the desired target state.
	 */
	private synchronized void updateState() {
		boolean done = false;
		while (!done) {
			switch (state_) {
			case CLOSED:
				if (shouldOpen_) {
					setState(State.WAIT_DEVICE_ATTACHED);
				} else {
					done = true;
				}
				break;

			case WAIT_DEVICE_ATTACHED:
				if (!shouldOpen_) {
					setState(State.CLOSED);
				} else if (checkAttached()) {
					permission_ = Permission.UNKNOWN;
					setState(State.DEVICE_ATTACHED);
				} else {
					done = true;
				}
				break;

			case DEVICE_ATTACHED:
				if (!shouldOpen_) {
					setState(State.CLOSED);
				} else if (shouldOpenDevice_) {
					setState(State.WAIT_DEVICE_PERMITTED);
				} else {
					done = true;
				}
				break;

			case WAIT_DEVICE_PERMITTED:
				if (!shouldOpen_ || !shouldOpenDevice_) {
					if (permission_ == Permission.PENDING) {
						pendingIntent_.cancel();
					}
					setState(State.DEVICE_ATTACHED);
				} else if (device_ == null) {
					// Detached
					if (permission_ == Permission.PENDING) {
						pendingIntent_.cancel();
					}
					setState(State.WAIT_DEVICE_ATTACHED);
				} else {
					checkPermission();
					switch (permission_) {
					case PENDING:
						done = true;
						break;

					case GRANTED:
						if (openDevice()) {
							setState(State.DEVICE_OPEN);
						} else {
							setState(State.DEVICE_ZOMBIE);
						}
						break;

					case DENIED:
						setState(State.DEVICE_ZOMBIE);
						break;
					default:
						assert false;
					}
				}
				break;

			case DEVICE_ZOMBIE:
				if (device_ == null) {
					// Detached
					setState(State.WAIT_DEVICE_ATTACHED);
				} else if (!shouldOpen_ || !shouldOpenDevice_) {
					setState(State.DEVICE_ATTACHED);
				} else {
					done = true;
				}
				break;

			case DEVICE_OPEN:
				if (device_ == null) {
					// Detached
					setState(State.WAIT_DEVICE_ATTACHED);
				} else if (!shouldOpen_ || !shouldOpenDevice_) {
					closeDevice();
					setState(State.DEVICE_ATTACHED);
				} else {
					done = true;
				}
				break;
			}
		}
		notifyAll();
	}

	/**
	 * Open a connection to the IOIO device.
	 *
	 * If the call succeeds it returns {@code true} and the caller is responsible to call
	 * {@link #closeDevice()}. If the call fails, it returns (@code false} and no clean-up is
	 * required from the caller.
	 *
	 * @precondition The device is attached and permission has been granted to connect to it.
	 */
	private boolean openDevice() {
		assert device_ != null;

		if (!processDescriptor())
			return false;

		connection_ = usbManager_.openDevice(device_);
		if (connection_ != null) {
			if (openStreams()) {
				return true;
			}
			// If we got here, we failed.
			connection_.close();
			connection_ = null;
		}
		return false;
	}

	/**
	 * Close the connection, previously established by {@link #openDevice()}.
	 */
	private void closeDevice() {
		assert device_ != null;
		closeStreams();
		connection_.close();
		connection_ = null;
	}

	/**
	 * Validate the device descriptor and extract the interface and endpoint descriptors.
	 *
	 * @return {@code true} if successful.
	 */
	private boolean processDescriptor() {
		assert device_ != null;

		if (device_.getInterfaceCount() != 2) {
			Log.e(TAG, "UsbDevice doesn't have exactly 2 interfaces.");
			return false;
		}

		controlInterface_ = device_.getInterface(0);
		dataInterface_ = device_.getInterface(1);

		if (controlInterface_.getEndpointCount() != 1) {
			Log.e(TAG, "Control interface (0) of UsbDevice doesn't have exactly 1 endpoints.");
			return false;
		}

		if (dataInterface_.getEndpointCount() != 2) {
			Log.e(TAG, "Data interface (1) of UsbDevice doesn't have exactly 2 endpoints.");
			return false;
		}

		final UsbEndpoint ep0 = dataInterface_.getEndpoint(0);
		final UsbEndpoint ep1 = dataInterface_.getEndpoint(1);

		if (ep0.getDirection() == UsbConstants.USB_DIR_IN
				&& ep1.getDirection() == UsbConstants.USB_DIR_OUT) {
			epIn_ = ep0;
			epOut_ = ep1;
		} else if (ep0.getDirection() == UsbConstants.USB_DIR_OUT
				&& ep1.getDirection() == UsbConstants.USB_DIR_IN) {
			epIn_ = ep1;
			epOut_ = ep0;
		} else {
			Log.e(TAG, "Endpoints directions are not compatible.");
			return false;
		}
		return true;
	}

	/**
	 * Claim interfaces and create the I/O streams.
	 *
	 * If the call succeeds it returns {@code true} and the caller is responsible to call
	 * {@link #closeStreams()}. If the call fails, it returns (@code false} and no clean-up is
	 * required from the caller.
	 *
	 * @precondition The device is attached, permission has been granted to connect to it, and the
	 *               descriptor has been processed.
	 */
	private boolean openStreams() {
		// Claim interfaces.
		if (connection_.claimInterface(controlInterface_, true)) {
			if (connection_.claimInterface(dataInterface_, true)) {
				// Raise DTR.
				if (setDTR(true)) {
					// Create streams. Buffer them with a reasonable buffer sizes.
					inputStream_ = new FixedReadBufferedInputStream(new Streams.DeviceInputStream(
							connection_, epIn_), 1024);
					outputStream_ = new BufferedOutputStream(new Streams.DeviceOutputStream(
							connection_, epOut_), 1024);
					return true;
				} else {
					Log.e(TAG, "Failed to set DTR to true");
				}
				// If we got here, we failed.
				connection_.releaseInterface(dataInterface_);
				Log.e(TAG, "Failed to claim UsbInterface 1");
			}
			// If we got here, we failed.
			connection_.releaseInterface(controlInterface_);
		} else {
			Log.e(TAG, "Failed to claim UsbInterface 0");
		}
		// If we got here, we failed.
		return false;
	}

	/**
	 * Close the streams and release the interfaces, to clean-up a successful call to
	 * {@link #openStreams()}.
	 */
	void closeStreams() {
		setDTR(false);
		connection_.releaseInterface(controlInterface_);
		connection_.releaseInterface(dataInterface_);
	}

	/**
	 * Sends a DTR signal to the IOIO.
	 *
	 * The IOIO uses this signal to indicate whether a client is currently connected to it.
	 *
	 * @param {@code true} means start a session, {@code false} means end a session.
	 * @return {@code true} if the transfer succeeded.
	 */
	private boolean setDTR(boolean dtr) {
		return 0 == connection_.controlTransfer(REQUEST_TYPE, SET_CONTROL_LINE_STATE, dtr ? 0x01
				: 0x00, 0, null, 0, Streams.TRANSFER_TIMEOUT_MILLIS);
	}

	/**
	 * Check whether the IOIO is attached.
	 *
	 * Returns fast if has previously returned positively.
	 *
	 * @return {@code true} If attached. In this case the {@link #device_} field will contain the
	 *         {@link UsbDevice}.
	 */
	private boolean checkAttached() {
		if (device_ != null)
			return true;
		HashMap<String, UsbDevice> devicesMap = usbManager_.getDeviceList();
		for (UsbDevice device : devicesMap.values()) {
			if (device.getProductId() != 0x0008 || device.getVendorId() != 0x1B4F) {
				// This is not IOIO :(.
				continue;
			}
			device_ = device;
			return true;
		}
		return false;
	}

	/**
	 * Check the status of the permission to connect to the IOIO.
	 *
	 * If it is yet unknown whether we have permission or not, will issue an permission request.
	 * Updates the {@link #permission_} field.
	 */
	private void checkPermission() {
		if (permission_ == Permission.UNKNOWN) {
			if (usbManager_.hasPermission(device_)) {
				permission_ = Permission.GRANTED;
			} else {
				pendingIntent_ = PendingIntent.getBroadcast(activity_, 0, new Intent(
						ACTION_USB_PERMISSION), 0);
				usbManager_.requestPermission(device_, pendingIntent_);
				permission_ = Permission.PENDING;
			}
		}
	}

	private void registerReceiver() {
		IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
		filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
		filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
		activity_.registerReceiver(this, filter);
	}

	private void unregisterReceiver() {
		activity_.unregisterReceiver(this);
	}

	private enum InstanceState {
		INIT, CONNECTED, DEAD
	};

	/**
	 * The actual {@link IOIOConnection} implementation.
	 *
	 * Doesn't do much, except signal the external state machine whether or not we're interested in
	 * an open connection, and waits for the external state machine to reach the desired states.
	 */
	class Connection implements IOIOConnection {
		private InstanceState instanceState_ = InstanceState.INIT;

		@Override
		public void waitForConnect() throws ConnectionLostException {
			Log.v(TAG, "waitForConnect()");
			synchronized (DeviceConnectionBootstrap.this) {
				if (instanceState_ != InstanceState.INIT) {
					throw new IllegalStateException("waitForConnect() may only be called once");
				}
				shouldOpenDevice_ = true;
				updateState();
				while (instanceState_ != InstanceState.DEAD && state_ != State.DEVICE_OPEN) {
					try {
						DeviceConnectionBootstrap.this.wait();
					} catch (InterruptedException e) {
						Log.e(TAG, "waitForConnect() was interrupted");
					}
				}
				if (instanceState_ == InstanceState.DEAD) {
					throw new ConnectionLostException();
				}
				instanceState_ = InstanceState.CONNECTED;
			}
		}

		@Override
		public void disconnect() {
			Log.v(TAG, "disconnect()");
			synchronized (DeviceConnectionBootstrap.this) {
				shouldOpenDevice_ = false;
				instanceState_ = InstanceState.DEAD;
				updateState();
			}
		}

		@Override
		public boolean canClose() {
			return true;
		}

		@Override
		public InputStream getInputStream() throws ConnectionLostException {
			return inputStream_;
		}

		@Override
		public OutputStream getOutputStream() throws ConnectionLostException {
			return outputStream_;
		}
	}
}
