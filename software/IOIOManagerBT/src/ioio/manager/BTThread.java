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
	private static final String LOG_TAG = "BTThread";
	public static final String ACTION_BT_CONNECTION = "ioio.manager.ACTION_BT_CONNECTION";
	public static final String EXTRA_CONNECTED = "connected";
	public static final String ACTION_BT_GOT_IMAGE = "ioio.manager.ACTION_BT_GOT_IMAGE";
	public static final String EXTRA_FILENAME = "filename";

	private static final String IMAGE_SUFFIX = ".ioio";
	private static final String FINGERPRINT_SUFFIX = ".fp";

	public Handler mHandler;
	private Context mContext;
	private BluetoothServerSocket mServer;
	private String mBaseFilename;

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
				Log.i(LOG_TAG, "Accepting");
				BluetoothSocket socket = mServer.accept();
				Log.i(LOG_TAG, "Accepted");
				copyToFile(socket);
				createFingerprint();
				Log.i(LOG_TAG, "Done");
				Intent gotImageIntent = new Intent(ACTION_BT_GOT_IMAGE);
				gotImageIntent.putExtra(EXTRA_FILENAME, mBaseFilename);
				mContext.sendBroadcast(gotImageIntent);
			}
		} catch (Exception e) {
			if (!e.getMessage().equals("Operation Canceled")) {
				Log.e(LOG_TAG, "Exception caught", e);
				mContext.deleteFile(IMAGE_SUFFIX);
				mContext.deleteFile(FINGERPRINT_SUFFIX);
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
		byte buf[] = new byte[8];
		fillBufferFromInputStream(buf, input);
		mBaseFilename = new String(buf);
		String filename = mBaseFilename + IMAGE_SUFFIX;
		OutputStream output = mContext.openFileOutput(filename,
				Context.MODE_WORLD_READABLE);
		byte[] b = new byte[1024];
		try {
			int i;
			while ((i = input.read(b)) != -1) {
				output.write(b, 0, i);
			}
		} catch (IOException e) {
			if (e.getMessage().equals("Software caused connection abort")) {
				Log.i(LOG_TAG, "Wrote image file: " + filename);
			} else {
				throw e;
			}
		} finally {
			output.close();
		}
	}

	private void createFingerprint() throws IOException,
			NoSuchAlgorithmException {
		InputStream in = mContext.openFileInput(mBaseFilename + IMAGE_SUFFIX);
		String filename = mBaseFilename + FINGERPRINT_SUFFIX;
		OutputStream out = mContext.openFileOutput(filename,
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
			Log.i(LOG_TAG, "Wrote fingerprint file: " + filename);
		} finally {
			in.close();
			out.close();
		}
	}

	private void fillBufferFromInputStream(byte[] buffer, InputStream is)
			throws IOException {
		int pos = 0;
		while (pos < buffer.length) {
			int read = is.read(buffer, pos, buffer.length - pos);
			if (read == -1) {
				throw new IOException("Expected " + buffer.length
						+ " bytes, but got only " + pos);
			}
			pos += read;
		}
	}
}
