package ioio.manager;

import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

public class IOIOManagerService extends Service {
	public static final String ACTION_IOIO_CONNECT = "ioio.manager.IOIO_CONNECT";
	public static final String EXTRA_STATE = "state";
	public static final String EXTRA_HARDWARE_VER = "hardware";
	public static final String EXTRA_BOOTLOADER_VER = "bootloader";
	public static final int STATE_DISCONNECTED = 0;
	public static final int STATE_BOOTLOADER = 1;
	public static final int STATE_DOWNLOADING = 2;
	public static final int STATE_RUNNING = 3;

	private NotificationManager mNM;

	public int mHardwareVer;
	public int mBootloaderVer;

	private class Server extends Thread {
		private class IOIOException extends Exception {
			public IOIOException(String s) {
				super(s);
			}
		}

		private static final String PORT_FILENAME = "port";
		private static final int HEADER = 0x4F494F49;

		private SocketChannel mChannel;
		private ByteBuffer mBuf = ByteBuffer.allocateDirect(1024);

		@Override
		public void run() {
			Log.i("IOIOManagerService", "Thread started");
			mBuf.order(ByteOrder.LITTLE_ENDIAN);
			try {
				ServerSocketChannel serverSocketChannel = ServerSocketChannel
						.open();
				serverSocketChannel.socket().bind(null);
				Log.i("IOIOManagerService", "Server socket bound to port: "
						+ serverSocketChannel.socket().getLocalPort());
				createPortFile(serverSocketChannel.socket().getLocalPort());
				while (true) {
					Log.i("IOIOManagerService", "Accepting");
					mChannel = serverSocketChannel.accept();
					Log.i("IOIOManagerService", "Accpeted");
					try {
						handleConnection();
						Intent intent = new Intent(ACTION_IOIO_CONNECT);
						intent.putExtra(EXTRA_STATE, STATE_BOOTLOADER);
						intent.putExtra(EXTRA_HARDWARE_VER, mHardwareVer);
						intent.putExtra(EXTRA_BOOTLOADER_VER, mBootloaderVer);
						sendStickyBroadcast(intent);
						waitEOF();
						Log.i("IOIOManagerService", "EOF");
					} catch (IOIOException e) {
						Log.i("IOIOManagerService",
								"IOIOException: " + e.getMessage());
					} finally {
						mChannel.close();
						Intent intent = new Intent(ACTION_IOIO_CONNECT);
						intent.putExtra(EXTRA_STATE, STATE_DISCONNECTED);
						sendStickyBroadcast(intent);
					}
				}
			} catch (Exception e) {
				deleteFile(PORT_FILENAME);
				Log.i("IOIOManagerService", "Exception caught. Dying.");
			}
		}

		private void createPortFile(int port) throws IOException {
			FileOutputStream os = openFileOutput(PORT_FILENAME,
					MODE_WORLD_READABLE);
			os.write(port & 0xFF);
			os.write((port >> 8) & 0xFF);
			os.close();
		}

		private void handleConnection() throws IOException, IOIOException {
			int header = readInt();
			if (header != HEADER) {
				throw new IOIOException("Unexpected header");
			}
			mHardwareVer = readInt();
			mBootloaderVer = readInt();
		}

		private int readInt() throws IOException, IOIOException {
			int result;
			while (mBuf.position() < 4) {
				if (mChannel.read(mBuf) == -1) {
					throw new IOIOException("Unexpected EOF");
				}
			}
			mBuf.flip();
			result = mBuf.getInt();
			mBuf.compact();
			return result;
		}

		private byte[] read(int size) throws IOException, IOIOException {
			byte[] result = new byte[size];
			while (mBuf.position() < size) {
				if (mChannel.read(mBuf) == -1) {
					throw new IOIOException("Unexpected EOF");
				}
			}
			mBuf.flip();
			mBuf.get(result);
			mBuf.compact();
			return result;
		}

		private void waitEOF() throws IOException {
			do {
				mBuf.clear();
			} while (mChannel.read(mBuf) != -1);
		}
	}

	Server server = new Server();

	/**
	 * Class for clients to access. Because we know this service always runs in
	 * the same process as its clients, we don't need to deal with IPC.
	 */
	public class LocalBinder extends Binder {
		IOIOManagerService getService() {
			return IOIOManagerService.this;
		}
	}

	BroadcastReceiver mReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			switch (intent.getIntExtra(EXTRA_STATE, STATE_DISCONNECTED)) {
			case STATE_DISCONNECTED:
				removeNotification();
				break;
			case STATE_BOOTLOADER:
				showNotification(intent.getIntExtra("hardware", 0),
						intent.getIntExtra("bootloader", 0));
				break;
			}
		}
	};

	@Override
	public void onCreate() {
		mNM = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
		registerReceiver(mReceiver, new IntentFilter(ACTION_IOIO_CONNECT));
		server.start();
	}

	@Override
	public void onStart(Intent intent, int startId) {
		Log.i("IOIOManagerService", "Received start id " + startId + ": "
				+ intent);
	}

	@Override
	public void onDestroy() {
		unregisterReceiver(mReceiver);
		server.interrupt();
	}

	@Override
	public IBinder onBind(Intent intent) {
		return mBinder;
	}

	// This is the object that receives interactions from clients. See
	// RemoteService for a more complete example.
	private final IBinder mBinder = new LocalBinder();

	private void showNotification(int hardware, int bootloader) {
		CharSequence text = getText(R.string.service_connect);
		Notification notification = new Notification(R.drawable.notif_icon,
				text, System.currentTimeMillis());
		PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
				new Intent(this, IOIOManagerMainActivity.class), 0);
		String longText = String.format(
				getResources().getString(R.string.service_connect_long),
				hardware, bootloader);
		notification.setLatestEventInfo(this, text, longText, contentIntent);
		notification.flags |= Notification.FLAG_NO_CLEAR
				| Notification.FLAG_ONGOING_EVENT;
		mNM.notify(R.string.service_connect, notification);
	}

	private void removeNotification() {
		// Cancel the persistent notification.
		mNM.cancel(R.string.service_connect);
	}
}
