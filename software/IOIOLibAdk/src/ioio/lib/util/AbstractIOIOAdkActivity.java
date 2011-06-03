/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * This file has been modified from its original version by Ytai Ben-Tsvi for
 * the IOIO open source project.
 */

package ioio.lib.util;

import ioio.lib.api.IOIO;
import ioio.lib.api.IOIOConnection;
import ioio.lib.api.IOIOFactory;
import ioio.lib.api.exception.ConnectionLostException;

import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.util.Log;

import com.android.future.usb.UsbAccessory;
import com.android.future.usb.UsbManager;

/**
 * This class provides the same functionality as {@link AbstractIOIOActivity},
 * but establishes the IOIO connection using the OpenAccessory protocol ("ADK"),
 * rather than using the default TCP connection.
 * <p>
 * Implementations extending this class must delegate all {@link Activity}
 * methods to this class (for example, if the implementation overrides
 * onPause(), it must call super.onPause() as first thing.
 * <p>
 * There is some setup required to make your application play nicely with ADK.
 * First, you need an {@code AndroidManifest.xml} file similar to the following:
 * 
 * <pre>
 * {@code<?xml version="1.0" encoding="utf-8"?>
 * <manifest xmlns:a="http://schemas.android.com/apk/res/android"
 *       package="ioio.examples.adk"
 *       a:versionCode="1"
 *       a:versionName="1.0">
 *     <uses-sdk a:minSdkVersion="10" />
 * 
 *     <application a:icon="@drawable/icon" a:label="@string/app_name">
 *         <uses-library a:name="com.android.future.usb.accessory" />
 *         <activity a:name=".IOIOLibAdkTestMainActivity"
 *                 a:label="@string/app_name" a:launchMode="singleInstance">
 *             <intent-filter>
 *                 <action a:name="android.hardware.usb.action.USB_ACCESSORY_ATTACHED" />
 *                 <action a:name="android.intent.action.MAIN" />
 *                 <category a:name="android.intent.category.LAUNCHER" />
 *             </intent-filter>
 *             <meta-data a:name="android.hardware.usb.action.USB_ACCESSORY_ATTACHED"
 *                     a:resource="@xml/accessory_filter" />
 *         </activity>
 *     </application>
 * </manifest>}
 * </pre>
 * 
 * An in addition, create a file called {@code accessory_filter.xml}, in a
 * directory called {@code xml} under {@code res}. The content of this file
 * should be:
 * 
 * <pre>
 * {@code
 * <?xml version="1.0" encoding="utf-8"?>
 * <resources>
 *     <usb-accessory model="IOIO" />
 * </resources>
 * }
 * </pre>
 * 
 * <h2>Known Limitations</h2>
 * Due to current limitations in the accessory library in the Android O/S
 * (V2.3.4 at the time of writing), it is impossible to gracefully close a
 * connection to an accessory from within the application. For that reason,
 * exiting your application will not properly disconnect from the IOIO, and upon
 * re-entry an attempt to establish a new connection will block indenfinitely.
 * The workaround is to physically disconnect (or power off) the IOIO, which
 * will cause a proper closure of the connection.
 * <p>
 * For more information on how to use this class, see
 * {@link AbstractIOIOActivity}.
 */
public abstract class AbstractIOIOAdkActivity extends AbstractIOIOActivity {
	private static final String TAG = "AbstractIOIOAdkActivity";

	private static final String ACTION_USB_PERMISSION = "ioio.lib.adk.action.USB_PERMISSION";

	private UsbManager mUsbManager;
	private PendingIntent mPermissionIntent;
	private boolean mPermissionRequestPending;

	private UsbAccessory mAccessory;
	private ParcelFileDescriptor mFileDescriptor;
	private FileInputStream mInputStream;
	private FileOutputStream mOutputStream;

	private final BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			Log.d(TAG, "onReceive " + intent.toString());
			String action = intent.getAction();
			if (ACTION_USB_PERMISSION.equals(action)) {
				synchronized (this) {
					UsbAccessory accessory = UsbManager.getAccessory(intent);
					if (intent.getBooleanExtra(
							UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
						openAccessory(accessory);
					} else {
						Log.d(TAG, "permission denied for accessory "
								+ accessory);
					}
					mPermissionRequestPending = false;
				}
			} else if (UsbManager.ACTION_USB_ACCESSORY_DETACHED.equals(action)) {
				UsbAccessory accessory = UsbManager.getAccessory(intent);
				if (accessory != null && accessory.equals(mAccessory)) {
					closeAccessory();
				}
			}
		}
	};

	/** Called when the activity is first created. */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		mUsbManager = UsbManager.getInstance(this);
		mPermissionIntent = PendingIntent.getBroadcast(this, 0, new Intent(
				ACTION_USB_PERMISSION), 0);
		IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
		filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
		registerReceiver(mUsbReceiver, filter);

		if (getLastNonConfigurationInstance() != null) {
			mAccessory = (UsbAccessory) getLastNonConfigurationInstance();
			openAccessory(mAccessory);
		}
	}

	@Override
	public Object onRetainNonConfigurationInstance() {
		if (mAccessory != null) {
			return mAccessory;
		} else {
			return super.onRetainNonConfigurationInstance();
		}
	}

	@Override
	protected void onResume() {
		super.onResume();

		if (mInputStream != null && mOutputStream != null) {
			return;
		}

		UsbAccessory[] accessories = mUsbManager.getAccessoryList();
		UsbAccessory accessory = (accessories == null ? null : accessories[0]);
		if (accessory != null) {
			if (mUsbManager.hasPermission(accessory)) {
				openAccessory(accessory);
			} else {
				synchronized (mUsbReceiver) {
					if (!mPermissionRequestPending) {
						mUsbManager.requestPermission(accessory,
								mPermissionIntent);
						mPermissionRequestPending = true;
					}
				}
			}
		} else {
			Log.d(TAG, "mAccessory is null");
		}
	}

	@Override
	protected void onPause() {
		super.onPause();
		closeAccessory();
	}

	@Override
	protected void onDestroy() {
		unregisterReceiver(mUsbReceiver);
		super.onDestroy();
	}

	private final synchronized void openAccessory(UsbAccessory accessory) {
		Log.d(TAG, "openAccessory(" + accessory.getModel() + ")");
		mFileDescriptor = mUsbManager.openAccessory(accessory);
		if (mFileDescriptor != null) {
			mAccessory = accessory;
			FileDescriptor fd = mFileDescriptor.getFileDescriptor();
			Log.d(TAG, "fd: " + fd.toString() + " valid: " + fd.valid());
			mInputStream = new FileInputStream(fd);
			mOutputStream = new FileOutputStream(fd);
			notifyAll();
			Log.d(TAG, "accessory opened");
		} else {
			Log.d(TAG, "accessory open fail");
		}
	}

	protected synchronized IOIO createIOIO() {
		return IOIOFactory.create(createIOIOConnection());
	}

	protected IOIOConnection createIOIOConnection() {
		return new IOIOConnection() {
			private boolean mDisconnected = false;

			@Override
			public void waitForConnect() throws ConnectionLostException {
				synchronized (AbstractIOIOAdkActivity.this) {
					if (mDisconnected) {
						throw new ConnectionLostException();
					}
					while (mFileDescriptor == null && !mDisconnected) {
						try {
							AbstractIOIOAdkActivity.this.wait();
						} catch (InterruptedException e) {
						}
					}
					if (mDisconnected) {
						throw new ConnectionLostException();
					}
				}
			}

			@Override
			public OutputStream getOutputStream()
					throws ConnectionLostException {
				return mOutputStream;
			}

			@Override
			public InputStream getInputStream() throws ConnectionLostException {
				return mInputStream;
			}

			@Override
			public void disconnect() {
				mDisconnected = true;
				// broken! the input stream will not unblock.
				closeAccessory();
			}
		};
	}

	private final synchronized void closeAccessory() {
		Log.d(TAG, "closeAccessory()");
		try {
			if (mInputStream != null) {
				mInputStream.close();
			}
		} catch (IOException e) {
		} finally {
			mInputStream = null;
		}

		try {
			if (mFileDescriptor != null) {
				mFileDescriptor.close();
			}
		} catch (IOException e) {
		} finally {
			mFileDescriptor = null;
			mAccessory = null;
			notifyAll();
		}
	}
}
