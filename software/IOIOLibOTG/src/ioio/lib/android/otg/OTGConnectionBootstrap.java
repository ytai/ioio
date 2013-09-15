package ioio.lib.android.otg;

import ioio.lib.api.IOIOConnection;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.spi.IOIOConnectionFactory;
import ioio.lib.spi.NoRuntimeSupportException;
import ioio.lib.util.android.ContextWrapperDependent;

import java.io.IOException;
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
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.os.Build;
import android.util.Log;

public class OTGConnectionBootstrap extends BroadcastReceiver implements ContextWrapperDependent, IOIOConnectionBootstrap {
	private static final String TAG = "OTGConnectionBootstrap";
	private static final String ACTION_USB_PERMISSION = "ioio.lib.otg.action.USB_PERMISSION";
	
	private static final int REQUEST_TYPE = 0x21;
    private static final int SET_CONTROL_LINE_STATE = 0x22;
    
    private static final int TRANSFER_TIMEOUT_MILLIS = 0;
	
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

	public OTGConnectionBootstrap() {
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
				pendingIntent_ = PendingIntent.getBroadcast(activity_, 0,
						new Intent(ACTION_USB_PERMISSION), 0);
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
	
	private void openStreams() {
		connection_ = usbManager_.openDevice(device_);
		controlInterface_ = device_.getInterface(0);
		if (!connection_.claimInterface(controlInterface_, true)) {
			Log.e(TAG, "Failed to claim UsbInterface 0");
			setState(State.CLOSED);
			return;
		}
		dataInterface_ = device_.getInterface(1);
		if (!connection_.claimInterface(dataInterface_, true)) {
			Log.e(TAG, "Failed to claim UsbInterface 1");
			setState(State.CLOSED);
			return;
		}
		final UsbEndpoint epIn = dataInterface_.getEndpoint(1);
		final UsbEndpoint epOut = dataInterface_.getEndpoint(0);
		
		// TODO(misha): Check for failures.
		// Set DTR to false
		if (0 != connection_.controlTransfer(
				REQUEST_TYPE, SET_CONTROL_LINE_STATE,
				0x00, 0, null, 0, TRANSFER_TIMEOUT_MILLIS)) {
			Log.e(TAG, "Failed to set DTR to false");
			closeStreams();
			setState(State.CLOSED);
		}
		// Set DTR to true (initiates handshake).
		if (0  != connection_.controlTransfer(
				REQUEST_TYPE, SET_CONTROL_LINE_STATE,
				0x01, 0, null, 0, TRANSFER_TIMEOUT_MILLIS)) {
			Log.e(TAG, "Failed to set DTR to true");
			closeStreams();
			setState(State.CLOSED);
		}

		inputStream_ = new InputStream() {
			@Override
			public int read() throws IOException {
				byte[] buffer = new byte[1];
				return read(buffer) == 1 ? buffer[0] : -1;
			}
			
			@Override
			public int read(byte[] buffer) throws IOException {
				return read(buffer, 0, buffer.length);
			}
			
			@Override
			public int read(byte[] buffer, int offset, int length) throws IOException {
				if (offset == 0) {
					return connection_.bulkTransfer(epIn, buffer, buffer.length, TRANSFER_TIMEOUT_MILLIS);
				}
				byte[] tmp = new byte[length];
				int readAmount = connection_.bulkTransfer(epIn, tmp, buffer.length, TRANSFER_TIMEOUT_MILLIS);
				System.arraycopy(tmp, 0, buffer, offset, Math.min(length, readAmount));
				return readAmount;
			}
		};
		
		outputStream_ = new OutputStream() {
			@Override
			public void write(int oneByte) throws IOException {
				write(new byte[]{(byte) oneByte});
			}
			
			@Override
			public void write(byte[] buffer) throws IOException {
				write(buffer, 0, buffer.length);
			}
			
			@Override
			public void write(byte[] buffer, int offset, int count) throws IOException {
				while (count > 0) {
					if (offset == 0) {
						offset = connection_.bulkTransfer(epOut, buffer, Math.min(buffer.length, count), TRANSFER_TIMEOUT_MILLIS);
						if  (offset < 0) {
							throw new IOException("Couldn't write to USB");
						}
						count -= offset;
						continue;
					}
					int len = Math.min(buffer.length - offset, count);
					// TODO: Allocate only once?
					byte[] tmp = new byte[len];
					System.arraycopy(buffer, offset, tmp, 0, len);
					int written = connection_.bulkTransfer(epOut, tmp, len, TRANSFER_TIMEOUT_MILLIS);
					if (written < 0) {
						throw new IOException("Couldn't write to USB");
					}
					offset += written;
					count -= written;
				}
			}
		};

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
				return OTGConnectionBootstrap.class.getCanonicalName();
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
	
	class Connection implements IOIOConnection{
		private InstanceState instanceState_ = InstanceState.INIT;
		
		@Override
		public boolean canClose() {
			return false;
		}

		@Override
		public void disconnect() {
			synchronized (OTGConnectionBootstrap.this) {
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
			synchronized (OTGConnectionBootstrap.this) {
				if (instanceState_ != InstanceState.INIT) {
					throw new IllegalStateException(
							"waitForConnect() may only be called once");
				}
				while (instanceState_ != InstanceState.DEAD	&& state_ != State.OPEN) {
					try {
						OTGConnectionBootstrap.this.wait();
					} catch (InterruptedException e) { }
				}
				if (instanceState_ == InstanceState.DEAD) {
					throw new ConnectionLostException();
				}
			}
			instanceState_ = InstanceState.CONNECTED;
		}
	}
}
