/*
 * Copyright 2014 Ytai Ben-Tsvi. All rights reserved.
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

import ioio.lib.spi.NoRuntimeSupportException;
import android.annotation.TargetApi;
import android.app.PendingIntent;
import android.content.Context;
import android.content.ContextWrapper;
import android.os.Build;
import android.os.ParcelFileDescriptor;

/**
 * This class serves as a common interface for USB accessory for either com.android.future.usb or
 * android.hardware.usb.
 *
 * When this class is instantiated, it will throw a {@link NoRuntimeSupportException} if none of the
 * underlying implementations exist. Otherwise, the client may call
 * {@link #getManager(ContextWrapper)}, which returns an interface that is equivalent to UsbManager
 * on either API. Likewise, the {@link UsbAccessoryInterface} interface is a wrapper around
 * UsbAccessory.
 *
 */
class Adapter {
	static abstract class AbstractUsbManager {
		final String ACTION_USB_ACCESSORY_DETACHED;
		final String EXTRA_PERMISSION_GRANTED;

		abstract UsbAccessoryInterface[] getAccessoryList();

		abstract boolean hasPermission(UsbAccessoryInterface accessory);

		abstract void requestPermission(UsbAccessoryInterface accessory, PendingIntent pendingIntent);

		abstract ParcelFileDescriptor openAccessory(UsbAccessoryInterface accessory);

		protected AbstractUsbManager(String action_usb_accessory_detached,
				String extra_permission_granted) {
			ACTION_USB_ACCESSORY_DETACHED = action_usb_accessory_detached;
			EXTRA_PERMISSION_GRANTED = extra_permission_granted;
		}
	}

	static interface UsbAccessoryInterface {
	}

	Adapter() throws NoRuntimeSupportException {
		try {
			Class.forName("android.hardware.usb.UsbManager");
			support_ = Support.NEW;
			return;
		} catch (ClassNotFoundException e) {
		}
		try {
			Class.forName("com.android.future.usb.UsbManager");
			support_ = Support.LEGACY;
			return;
		} catch (ClassNotFoundException e) {
		}
		throw new NoRuntimeSupportException("No support for USB accesory mode.");
	}

	AbstractUsbManager getManager(ContextWrapper wrapper) {
		switch (support_) {
		case NEW:
			return getManagerNew(wrapper);
		case LEGACY:
			return getManagerLegacy(wrapper);
		default:
			return null;
		}
	}

	private enum Support {
		NEW, LEGACY, NONE
	}

	Support support_ = Support.NONE;

	private static class UsbAccessoryAdapter<T> implements UsbAccessoryInterface {
		final T impl;

		UsbAccessoryAdapter(T t) {
			impl = t;
		}
	}

	@TargetApi(Build.VERSION_CODES.HONEYCOMB_MR1)
	private AbstractUsbManager getManagerNew(ContextWrapper wrapper) {
		final android.hardware.usb.UsbManager manager = (android.hardware.usb.UsbManager) wrapper
				.getSystemService(Context.USB_SERVICE);
		return new NewUsbManager(manager);
	}

	private AbstractUsbManager getManagerLegacy(ContextWrapper wrapper) {
		final com.android.future.usb.UsbManager manager = com.android.future.usb.UsbManager
				.getInstance(wrapper);
		return new LegacyUsbManager(manager);
	}

	private static final class LegacyUsbManager extends AbstractUsbManager {
		private final com.android.future.usb.UsbManager manager_;

		private LegacyUsbManager(com.android.future.usb.UsbManager manager) {
			super(com.android.future.usb.UsbManager.ACTION_USB_ACCESSORY_DETACHED,
					com.android.future.usb.UsbManager.EXTRA_PERMISSION_GRANTED);
			manager_ = manager;
		}

		@SuppressWarnings("unchecked")
		@Override
		void requestPermission(UsbAccessoryInterface accessory, PendingIntent pendingIntent) {
			manager_.requestPermission(
					((UsbAccessoryAdapter<com.android.future.usb.UsbAccessory>) accessory).impl,
					pendingIntent);
		}

		@SuppressWarnings("unchecked")
		@Override
		ParcelFileDescriptor openAccessory(UsbAccessoryInterface accessory) {
			return manager_
					.openAccessory(((UsbAccessoryAdapter<com.android.future.usb.UsbAccessory>) accessory).impl);
		}

		@SuppressWarnings("unchecked")
		@Override
		boolean hasPermission(UsbAccessoryInterface accessory) {
			return manager_
					.hasPermission(((UsbAccessoryAdapter<com.android.future.usb.UsbAccessory>) accessory).impl);
		}

		@Override
		UsbAccessoryInterface[] getAccessoryList() {
			final com.android.future.usb.UsbAccessory[] accs = manager_.getAccessoryList();
			if (accs == null)
				return null;
			UsbAccessoryInterface[] result = new UsbAccessoryInterface[accs.length];
			for (int i = 0; i < accs.length; ++i) {
				result[i] = new UsbAccessoryAdapter<com.android.future.usb.UsbAccessory>(accs[i]);
			}
			return result;
		}
	}

	@TargetApi(Build.VERSION_CODES.HONEYCOMB_MR1)
	private static final class NewUsbManager extends AbstractUsbManager {
		private final android.hardware.usb.UsbManager manager_;

		private NewUsbManager(android.hardware.usb.UsbManager manager) {
			super(android.hardware.usb.UsbManager.ACTION_USB_ACCESSORY_DETACHED,
					android.hardware.usb.UsbManager.EXTRA_PERMISSION_GRANTED);
			manager_ = manager;
		}

		@SuppressWarnings("unchecked")
		@Override
		void requestPermission(UsbAccessoryInterface accessory, PendingIntent pendingIntent) {
			manager_.requestPermission(
					((UsbAccessoryAdapter<android.hardware.usb.UsbAccessory>) accessory).impl,
					pendingIntent);
		}

		@SuppressWarnings("unchecked")
		@Override
		ParcelFileDescriptor openAccessory(UsbAccessoryInterface accessory) {
			return manager_
					.openAccessory(((UsbAccessoryAdapter<android.hardware.usb.UsbAccessory>) accessory).impl);
		}

		@SuppressWarnings("unchecked")
		@Override
		boolean hasPermission(UsbAccessoryInterface accessory) {
			return manager_
					.hasPermission(((UsbAccessoryAdapter<android.hardware.usb.UsbAccessory>) accessory).impl);
		}

		@Override
		UsbAccessoryInterface[] getAccessoryList() {
			final android.hardware.usb.UsbAccessory[] accs = manager_.getAccessoryList();
			if (accs == null)
				return null;
			UsbAccessoryInterface[] result = new UsbAccessoryInterface[accs.length];
			for (int i = 0; i < accs.length; ++i) {
				result[i] = new UsbAccessoryAdapter<android.hardware.usb.UsbAccessory>(accs[i]);
			}
			return result;
		}
	}
}
