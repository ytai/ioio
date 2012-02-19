package ioio.manager;

import java.io.IOException;
import java.util.UUID;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothServerSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

public class IOIOManagerBTService extends Service {
	private NotificationManager mNM;

	/**
	 * Class for clients to access. Because we know this service always runs in
	 * the same process as its clients, we don't need to deal with IPC.
	 */
	public class IOIOManagerBTServiceBinder extends Binder {
		IOIOManagerBTService getService() {
			return IOIOManagerBTService.this;
		}
	}

	private BTThread mBTThread;
	private BroadcastReceiver mReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			if (intent.getAction().equals(BTThread.ACTION_BT_CONNECTION)) {
				boolean connected = intent.getBooleanExtra(
						BTThread.EXTRA_CONNECTED, false);
				if (connected) {
					showNotification();
				} else {
					hideNotification();
				}
			} else if (intent.getAction().equals(BTThread.ACTION_BT_GOT_IMAGE)) {
				Toast.makeText(
						IOIOManagerBTService.this,
						getText(R.string.got_image) + ": "
								+ intent.getStringExtra(BTThread.EXTRA_FILENAME),
						Toast.LENGTH_LONG).show();
			}
		}
	};

	@Override
	public void onCreate() {
		mNM = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
		IntentFilter filter = new IntentFilter(BTThread.ACTION_BT_CONNECTION);
		filter.addAction(BTThread.ACTION_BT_GOT_IMAGE);
		registerReceiver(mReceiver, filter);
		try {
			BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
			BluetoothServerSocket server = adapter
					.listenUsingRfcommWithServiceRecord("IOIO Programmer", UUID
							.fromString("00001101-0000-1000-8000-00805F9B34FB"));
			mBTThread = new BTThread(this, server);
			mBTThread.start();
		} catch (IOException e) {
			Log.e("IOIOManagerBTService", "Failed to initialize BT", e);
		}
	}

	@Override
	public void onStart(Intent intent, int startId) {
		Log.i("IOIOManagerBTService", "Received start id " + startId + ": "
				+ intent);
		if (intent.getBooleanExtra("close", false)) {
			stopSelf();
		}
	}

	@Override
	public void onDestroy() {
		mBTThread.cancel();
		try {
			mBTThread.join();
		} catch (InterruptedException e) {
		}
		unregisterReceiver(mReceiver);
	}

	@Override
	public IBinder onBind(Intent intent) {
		return mBinder;
	}

	// This is the object that receives interactions from clients. See
	// RemoteService for a more complete example.
	private final IBinder mBinder = new IOIOManagerBTServiceBinder();

	/**
	 * Show a notification while this service is running.
	 */
	private void showNotification() {
		CharSequence text = getText(R.string.service_start);
		Notification notification = new Notification(R.drawable.notif_icon,
				text, System.currentTimeMillis());
		notification.flags |= Notification.FLAG_ONGOING_EVENT
				| Notification.FLAG_NO_CLEAR;

		Intent stopIntent = new Intent(this, this.getClass());
		stopIntent.putExtra("close", true);
		PendingIntent contentIntent = PendingIntent.getService(this, 0,
				stopIntent, 0);

		// Set the info for the views that show in the notification panel.
		notification.setLatestEventInfo(this, getText(R.string.service_start),
				getText(R.string.service_start_detail), contentIntent);
		// Send the notification.
		// We use a layout id because it is a unique number. We use it later to
		// cancel.
		mNM.notify(R.string.service_start, notification);
	}

	private void hideNotification() {
		// Cancel the persistent notification.
		mNM.cancel(R.string.service_start);
	}
}
