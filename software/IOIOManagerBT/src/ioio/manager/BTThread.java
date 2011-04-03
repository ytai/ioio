package ioio.manager;

import java.io.File;
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

	private static final String FILENAME = "image";
	private static final String IMAGE_SUFFIX = "ioio";
	private static final String FINGERPRINT_SUFFIX = "fp";

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
		while (true) {
			try {
				Log.i("BTThread", "Accepting");
				BluetoothSocket socket = mServer.accept();
				Log.i("BTThread", "Accepted");
				copyToFile(socket);
				createFingerprint();
				renameFilesByVariant();
				Log.i("BTThread", "Done");
				mContext.sendBroadcast(new Intent(ACTION_BT_GOT_IMAGE));
			} catch (Exception e) {
				if (e.getMessage().equals("Operation Canceled")) {
					break;
				} else {
					Log.e("BTThread", "Exception caught", e);
				}
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
		OutputStream output = mContext.openFileOutput(FILENAME + "." + IMAGE_SUFFIX,
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

	private void createFingerprint() throws IOException,
			NoSuchAlgorithmException {
		InputStream in = mContext.openFileInput(FILENAME + "." + IMAGE_SUFFIX);
		OutputStream out = mContext.openFileOutput(FILENAME + "." + FINGERPRINT_SUFFIX,
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

	private void renameFilesByVariant() throws IOException {
		InputStream in = mContext.openFileInput(FILENAME + "." + IMAGE_SUFFIX);;
		byte[] buf = new byte[12];
		String variantName;
		try {
			if (in.read(buf) != 12) {
				throw new IOException("IOIO file is corrupt");
			}
			int variant = buf[11];
			variant = variant << 8 | buf[10];
			variant = variant << 8 | buf[9];
			variant = variant << 8 | buf[8];
			variantName = Integer.toString(variant);
		} finally {
			in.close();
		}
		final File newImageFile = mContext.getFileStreamPath(FILENAME + "." + variantName + "." + IMAGE_SUFFIX);
		final File newFingerprintFile = mContext.getFileStreamPath(FILENAME + "." + variantName + "." + FINGERPRINT_SUFFIX);
		Log.i("BTThread", "Renaming files - start");
		new File(mContext.getFilesDir(), FILENAME + "." + IMAGE_SUFFIX).renameTo(newImageFile);
		new File(mContext.getFilesDir(), FILENAME + "." + FINGERPRINT_SUFFIX).renameTo(newFingerprintFile);
		Log.i("BTThread", "Renaming files - done");
	}
}
