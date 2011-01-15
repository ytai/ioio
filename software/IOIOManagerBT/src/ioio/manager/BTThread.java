package ioio.manager;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.util.Log;

public class BTThread extends Thread {
	public static final String ACTION_BT_CONNECTION = "ioio.manager.ACTION_BT_CONNECTION";
	public static final String EXTRA_CONNECTED = "connected";
	public static final String ACTION_BT_GOT_IMAGE = "ioio.manager.ACTION_BT_GOT_IMAGE";

	private static final String IMAGE_FILENAME = "image.ioio";
	private static final String FINGERPRINT_FILENAME = "image.fp";

	public Handler mHandler;
	private Context mContext;
	private BluetoothServerSocket mServer;

	BTThread(Context context, BluetoothServerSocket server) {
		mContext = context;
		mServer = server;
	}

	@Override
	public void run() {
		Intent intent = new Intent(ACTION_BT_CONNECTION);
		intent.putExtra(EXTRA_CONNECTED, true);
		mContext.sendStickyBroadcast(intent);
		try {
			while (true) {
				Log.i("BTThread", "Accepting");
				BluetoothSocket socket = mServer.accept();
				Log.i("BTThread", "Accepted");
				copyToFile(socket);
				createFingerprint();
				Log.i("BTThread", "Done");
				mContext.sendBroadcast(new Intent(ACTION_BT_GOT_IMAGE));
			}
		} catch (Exception e) {
			if (!e.getMessage().equals("Operation Canceled")) {
				Log.e("BTThread", "Exception caught", e);
				mContext.deleteFile(IMAGE_FILENAME);
				mContext.deleteFile(FINGERPRINT_FILENAME);
			}
		}
		intent = new Intent(ACTION_BT_CONNECTION);
		intent.putExtra(EXTRA_CONNECTED, false);
		mContext.sendStickyBroadcast(intent);
	}

	public void cancel() {
		try {
			mServer.close();
		} catch (IOException e) {
		}
	}

	private void copyToFile(BluetoothSocket socket) throws IOException {
		InputStream input = socket.getInputStream();
		OutputStream output = mContext.openFileOutput(IMAGE_FILENAME,
				Context.MODE_WORLD_READABLE);
		byte[] b = new byte[1024];
		try {
			int i;
			while ((i = input.read(b)) != -1) {
				output.write(b, 0, i);
			}
		} catch (IOException e) {
			if (!e.getMessage().equals("Software caused connection abort")) {
				throw e;
			}
		} finally {
			output.close();
		}
	}

	private void createFingerprint() throws IOException, NoSuchAlgorithmException {
		InputStream in = mContext.openFileInput(IMAGE_FILENAME);
		OutputStream out = mContext.openFileOutput(FINGERPRINT_FILENAME,
				Context.MODE_WORLD_READABLE);
		try {
			MessageDigest digester = MessageDigest.getInstance("MD5");
			byte[] bytes = new byte[1024];
			int byteCount;
			while ((byteCount = in.read(bytes)) > 0) {
				digester.update(bytes, 0, byteCount);
			}
			byte[] digest = digester.digest();
			out.write(digest);
		} finally {
			in.close();
			out.close();
		}
	}
}
