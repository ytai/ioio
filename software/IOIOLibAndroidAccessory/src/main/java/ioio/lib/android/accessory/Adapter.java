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

import android.annotation.TargetApi;
import android.app.PendingIntent;
import android.content.Context;
import android.content.ContextWrapper;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;
import android.os.Build;
import android.os.ParcelFileDescriptor;

import ioio.lib.spi.NoRuntimeSupportException;

/**
 * This class serves as a common interface for USB accessory for either com.android.future.usb or
 * android.hardware.usb.
 * <p>
 * When this class is instantiated, it will throw a {@link NoRuntimeSupportException} if none of the
 * underlying implementations exist. Otherwise, the client may call
 * {@link #getManager(ContextWrapper)}, which returns an interface that is equivalent to UsbManager
 * on either API. Likewise, the {@link UsbAccessoryInterface} interface is a wrapper around
 * UsbAccessory.
 */
class Adapter {
    static abstract class AbstractUsbManager {
        final String EXTRA_PERMISSION_GRANTED;

        abstract UsbAccessoryInterface[] getAccessoryList();

        abstract boolean hasPermission(UsbAccessoryInterface accessory);

        abstract void requestPermission(UsbAccessoryInterface accessory, PendingIntent pendingIntent);

        abstract ParcelFileDescriptor openAccessory(UsbAccessoryInterface accessory);

        protected AbstractUsbManager(String action_usb_accessory_detached,
                                     String extra_permission_granted) {
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
        final UsbManager manager = (UsbManager) wrapper.getSystemService(Context.USB_SERVICE);
        return new LegacyUsbManager(manager);
    }

    private static final class LegacyUsbManager extends AbstractUsbManager {
        private final UsbManager manager_;

        private LegacyUsbManager(UsbManager manager) {
            super(UsbManager.ACTION_USB_ACCESSORY_DETACHED, UsbManager.EXTRA_PERMISSION_GRANTED);
            manager_ = manager;
        }

        @TargetApi(Build.VERSION_CODES.HONEYCOMB_MR1)
        @SuppressWarnings("unchecked")
        @Override
        void requestPermission(UsbAccessoryInterface accessory, PendingIntent pendingIntent) {
            manager_.requestPermission(((UsbAccessoryAdapter<UsbAccessory>) accessory).impl, pendingIntent);
        }

        @TargetApi(Build.VERSION_CODES.HONEYCOMB_MR1)
        @SuppressWarnings("unchecked")
        @Override
        ParcelFileDescriptor openAccessory(UsbAccessoryInterface accessory) {
            return manager_.openAccessory(((UsbAccessoryAdapter<UsbAccessory>) accessory).impl);
        }

        @TargetApi(Build.VERSION_CODES.HONEYCOMB_MR1)
        @SuppressWarnings("unchecked")
        @Override
        boolean hasPermission(UsbAccessoryInterface accessory) {
            return manager_.hasPermission(((UsbAccessoryAdapter<UsbAccessory>) accessory).impl);
        }

        @TargetApi(Build.VERSION_CODES.HONEYCOMB_MR1)
        @Override
        UsbAccessoryInterface[] getAccessoryList() {
            final UsbAccessory[] accs = manager_.getAccessoryList();
            if (accs == null) {
                return null;
            }
            UsbAccessoryInterface[] result = new UsbAccessoryInterface[accs.length];
            for (int i = 0; i < accs.length; ++i) {
                result[i] = new UsbAccessoryAdapter<UsbAccessory>(accs[i]);
            }
            return result;
        }
    }

    @TargetApi(Build.VERSION_CODES.HONEYCOMB_MR1)
    private static final class NewUsbManager extends AbstractUsbManager {
        private final UsbManager manager_;

        private NewUsbManager(UsbManager manager) {
            super(UsbManager.ACTION_USB_ACCESSORY_DETACHED, UsbManager.EXTRA_PERMISSION_GRANTED);
            manager_ = manager;
        }

        @SuppressWarnings("unchecked")
        @Override
        void requestPermission(UsbAccessoryInterface accessory, PendingIntent pendingIntent) {
            manager_.requestPermission(
                    ((UsbAccessoryAdapter<UsbAccessory>) accessory).impl, pendingIntent);
        }

        @SuppressWarnings("unchecked")
        @Override
        ParcelFileDescriptor openAccessory(UsbAccessoryInterface accessory) {
            return manager_.openAccessory(((UsbAccessoryAdapter<UsbAccessory>) accessory).impl);
        }

        @SuppressWarnings("unchecked")
        @Override
        boolean hasPermission(UsbAccessoryInterface accessory) {
            return manager_.hasPermission(((UsbAccessoryAdapter<UsbAccessory>) accessory).impl);
        }

        @Override
        UsbAccessoryInterface[] getAccessoryList() {
            final UsbAccessory[] accs = manager_.getAccessoryList();
            if (accs == null) {
                return null;
            }
            UsbAccessoryInterface[] result = new UsbAccessoryInterface[accs.length];
            for (int i = 0; i < accs.length; ++i) {
                result[i] = new UsbAccessoryAdapter<UsbAccessory>(accs[i]);
            }
            return result;
        }
    }
}
