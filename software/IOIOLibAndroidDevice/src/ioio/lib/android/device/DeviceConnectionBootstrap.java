package ioio.lib.android.device;

import ioio.lib.api.IOIOConnection;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.spi.IOIOConnectionFactory;
import ioio.lib.spi.NoRuntimeSupportException;
import ioio.lib.util.android.ContextWrapperDependent;

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

public class DeviceConnectionBootstrap extends BroadcastReceiver implements
		ContextWrapperDependent, IOIOConnectionBootstrap {
	private static final String TAG = "DeviceConnectionBootstrap";
	private static final String ACTION_USB_PERMISSION = "ioio.lib.otg.action.USB_PERMISSION";

	private static final int REQUEST_TYPE = 0x21;
	private static final int SET_CONTROL_LINE_STATE = 0x22;

	private enum State {
		CLOSED, WAIT_PERMISSION, OPEN
	}

	private ContextWrapper activity_;
	private UsbManager usbManager_;
	private UsbDevice device_;
	private State state_ = State.CLOSED;
	private PendingIntent pendingIntent_;
	private UsbDeviceConnection connection_;
	private UsbInterface controlInterface_;
	private UsbInterface dataInterface_;
	private InputStream inputStream_;
	private OutputStream outputStream_;

	public DeviceConnectionBootstrap() {
		if (Build.VERSION.SDK_INT < 12) {
			throw new NoRuntimeSupportException("OTG is not supported on this device.");
		}
	}

	@Override
	public void onCreate(ContextWrapper wrapper) {
		activity_ = wrapper;
		usbManager_ = (UsbManager) wrapper.getSystemService(Context.USB_SERVICE);
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
		HashMap<String, UsbDevice> devicesMap = usbManager_.getDeviceList();
		for (UsbDevice device : devicesMap.values()) {
			if (device.getProductId() != 0x0008 || device.getVendorId() != 0x1B4F) {
				// This is not IOIO :(.
				continue;
			}
			device_ = device;
			if (usbManager_.hasPermission(device_)) {
				openStreams();
			} else {
				pendingIntent_ = PendingIntent.getBroadcast(activity_, 0, new Intent(
						ACTION_USB_PERMISSION), 0);
				usbManager_.requestPermission(device_, pendingIntent_);
				setState(State.WAIT_PERMISSION);
			}
			return;
		}
		Log.d(TAG, "No device found.");
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
	public void reopen() {
		open();
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

	private void setState(State state) {
		state_ = state;
		notifyAll();
	}

	private boolean claimInterfaces() {
		if (device_.getInterfaceCount() != 2) {
			Log.e(TAG,
					"UsbDevice doesn't have exactly 2 interface. It has "
							+ device_.getInterfaceCount());
			return false;
		}

		controlInterface_ = device_.getInterface(0);
		if (!connection_.claimInterface(controlInterface_, true)) {
			Log.e(TAG, "Failed to claim UsbInterface 0");
			return false;
		}
		dataInterface_ = device_.getInterface(1);
		if (!connection_.claimInterface(dataInterface_, true)) {
			Log.e(TAG, "Failed to claim UsbInterface 1");
			return false;
		}
		return true;
	}

	private void openStreams() {
		connection_ = usbManager_.openDevice(device_);
		if (connection_ == null) {
			Log.e(TAG, "Couldn't open UsbDevice");
			setState(State.CLOSED);
			return;
		}
		if (!claimInterfaces()) {
			closeStreams();
			setState(State.CLOSED);
			return;
		}

		if (controlInterface_.getEndpointCount() != 1) {
			Log.e(TAG, "Control interface (0) of UsbDevice doesn't have exactly 1 end point");
			closeStreams();
			setState(State.CLOSED);
			return;

		}

		if (dataInterface_.getEndpointCount() != 2) {
			Log.e(TAG, "Data interface (1) of UsbDevice doesn't have exactly 2 end point");
			closeStreams();
			setState(State.CLOSED);
			return;
		}

		final UsbEndpoint epIn = dataInterface_.getEndpoint(1);
		final UsbEndpoint epOut = dataInterface_.getEndpoint(0);

		if (epIn.getDirection() != UsbConstants.USB_DIR_IN
				|| epOut.getDirection() != UsbConstants.USB_DIR_OUT) {
			Log.e(TAG, "Data interface's endpoints are not IN and OUT as expected.");
			closeStreams();
			setState(State.CLOSED);
			return;

		}

		// Set DTR to false
		if (0 != connection_.controlTransfer(REQUEST_TYPE, SET_CONTROL_LINE_STATE, 0x00, 0, null,
				0, Streams.TRANSFER_TIMEOUT_MILLIS)) {
			Log.e(TAG, "Failed to set DTR to false");
			closeStreams();
			setState(State.CLOSED);
		}
		// Set DTR to true (initiates handshake).
		if (0 != connection_.controlTransfer(REQUEST_TYPE, SET_CONTROL_LINE_STATE, 0x01, 0, null,
				0, Streams.TRANSFER_TIMEOUT_MILLIS)) {
			Log.e(TAG, "Failed to set DTR to true");
			closeStreams();
			setState(State.CLOSED);
		}

		inputStream_ = new Streams.DeviceInputStream(connection_, epIn);
		outputStream_ = new Streams.DeviceOutputStream(connection_, epOut);

		setState(State.OPEN);
	}

	private void closeStreams() {
		connection_.releaseInterface(controlInterface_);
		connection_.releaseInterface(dataInterface_);
		connection_.close();
	}

	@Override
	public synchronized void onReceive(Context context, Intent intent) {
		String action = intent.getAction();
		if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)) {
			UsbDevice device = (UsbDevice) intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
			if (device.equals(device_)) {
				close();
			}
		} else if (UsbManager.ACTION_USB_DEVICE_ATTACHED.equals(action)) {
			if (state_ == State.CLOSED) {
				open();
			}
		} else if (ACTION_USB_PERMISSION.equals(action)) {
			if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
				openStreams();
			} else {
				setState(State.CLOSED);
			}
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

	private enum InstanceState {
		INIT, CONNECTED, DEAD
	};

	class Connection implements IOIOConnection {
		private InstanceState instanceState_ = InstanceState.INIT;

		@Override
		public boolean canClose() {
			return true;
		}

		@Override
		public void disconnect() {
			synchronized (DeviceConnectionBootstrap.this) {
				instanceState_ = InstanceState.DEAD;
				close();
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
		public void waitForConnect() throws ConnectionLostException {
			synchronized (DeviceConnectionBootstrap.this) {
				if (instanceState_ != InstanceState.INIT) {
					throw new IllegalStateException("waitForConnect() may only be called once");
				}
				while (instanceState_ != InstanceState.DEAD && state_ != State.OPEN) {
					try {
						DeviceConnectionBootstrap.this.wait();
					} catch (InterruptedException e) {
					}
				}
				if (instanceState_ == InstanceState.DEAD) {
					throw new ConnectionLostException();
				}
			}
			instanceState_ = InstanceState.CONNECTED;
		}
	}
}
