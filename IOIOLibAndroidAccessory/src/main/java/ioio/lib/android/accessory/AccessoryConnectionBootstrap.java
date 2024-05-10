/*
 * Copyright 2015 Ytai Ben-Tsvi. All rights reserved.
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

import ioio.lib.android.accessory.Adapter.UsbAccessoryInterface;
import ioio.lib.api.IOIOConnection;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.FixedReadBufferedInputStream;
import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.spi.IOIOConnectionFactory;
import ioio.lib.spi.NoRuntimeSupportException;
import ioio.lib.util.android.ContextWrapperDependent;

import java.io.BufferedOutputStream;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Collection;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.os.ParcelFileDescriptor;
import android.util.Log;

public class AccessoryConnectionBootstrap extends BroadcastReceiver implements
        ContextWrapperDependent, IOIOConnectionBootstrap, IOIOConnectionFactory {
    private static final String TAG = AccessoryConnectionBootstrap.class.getSimpleName();
    private static final String ACTION_USB_PERMISSION = "ioio.lib.accessory.action.USB_PERMISSION";

    private ContextWrapper activity_;
    private Adapter adapter_;
    private Adapter.AbstractUsbManager usbManager_;
    private boolean shouldTryOpen_ = false;
    private PendingIntent pendingIntent_;
    private ParcelFileDescriptor fileDescriptor_;
    private InputStream inputStream_;
    private OutputStream outputStream_;

    public AccessoryConnectionBootstrap() throws NoRuntimeSupportException {
        adapter_ = new Adapter();
    }

    @Override
    public void onCreate(ContextWrapper wrapper) {
        activity_ = wrapper;
        usbManager_ = adapter_.getManager(wrapper);
        registerReceiver();
    }

    @Override
    public void onDestroy() {
        unregisterReceiver();
    }

    @Override
    public synchronized void onReceive(Context context, Intent intent) {
        final String action = intent.getAction();
        if (ACTION_USB_PERMISSION.equals(action)) {
            pendingIntent_ = null;
            if (intent.getBooleanExtra(usbManager_.EXTRA_PERMISSION_GRANTED, false)) {
                notifyAll();
            } else {
                Log.e(TAG, "Permission denied");
            }
        }
    }

    @Override
    public synchronized void open() {
        notifyAll();
    }

    @Override
    public synchronized void reopen() {
        notifyAll();
    }

    @Override
    public synchronized void close() {
    }

    private synchronized void disconnect() {
        // This should abort any current open attempt.
        shouldTryOpen_ = false;
        notifyAll();

        // And this should kill any ongoing connections.
        if (fileDescriptor_ != null) {
            try {
                fileDescriptor_.close();
            } catch (IOException e) {
                Log.e(TAG, "Failed to close file descriptor.", e);
            }
            fileDescriptor_ = null;
        }

        if (pendingIntent_ != null) {
            pendingIntent_.cancel();
            pendingIntent_ = null;
        }
    }

    @Override
    public IOIOConnection createConnection() {
        return new Connection();
    }

    @Override
    public void getFactories(Collection<IOIOConnectionFactory> result) {
        result.add(this);
    }

    @Override
    public String getType() {
        return Connection.class.getCanonicalName();
    }

    @Override
    public Object getExtra() {
        return null;
    }

    private synchronized void waitForConnect() throws ConnectionLostException {
        // In order to simplify the connection process in face of many different sequences of events
        // that might occur, we collapsed the entire sequence into one non-blocking method,
        // tryOpen(), which tries the entire process from the beginning, undoes everything if
        // something along the way fails and always returns immediately.
        // This method, simply calls tryOpen() in a loop until it succeeds or until we're no longer
        // interested. Between attempts, it waits until "something interesting" has happened, which
        // may be permission granted, the client telling us to try again (via reopen()) or stop
        // trying, etc.
        shouldTryOpen_ = true;
        while (shouldTryOpen_) {
            if (tryOpen()) {
                // Success!
                return;
            }
            forceWait();
        }
        throw new ConnectionLostException();
    }

    private void forceWait() {
        try {
            wait();
        } catch (InterruptedException e) {
            Log.e(TAG, "Do not interrupt me!");
        }
    }

    private boolean tryOpen() {
        // Find the accessory.
        UsbAccessoryInterface[] accessories = usbManager_.getAccessoryList();
        UsbAccessoryInterface accessory = (accessories == null ? null : accessories[0]);

        if (accessory == null) {
            Log.v(TAG, "No accessory found.");
            return false;
        }

        // Check for permission to access the accessory.
        if (!usbManager_.hasPermission(accessory)) {
            if (pendingIntent_ == null) {
                Log.v(TAG, "Requesting permission.");

                int pendingIntentFlags;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                    pendingIntentFlags = PendingIntent.FLAG_IMMUTABLE | PendingIntent.FLAG_UPDATE_CURRENT;
                } else {
                    pendingIntentFlags = PendingIntent.FLAG_UPDATE_CURRENT;
                }

                pendingIntent_ = PendingIntent.getBroadcast(activity_, 0, new Intent(ACTION_USB_PERMISSION), pendingIntentFlags);
                usbManager_.requestPermission(accessory, pendingIntent_);
            }
            return false;
        }

        boolean success = false;

        // From this point on, if anything goes wrong, we're responsible for canceling the intent.
        try {
            // Obtain a file descriptor.
            fileDescriptor_ = usbManager_.openAccessory(accessory);
            if (fileDescriptor_ == null) {
                Log.v(TAG, "Failed to open file descriptor.");
                return false;
            }

            // From this point on, if anything goes wrong, we're responsible for closing the file
            // descriptor.
            try {
                FileDescriptor fd = fileDescriptor_.getFileDescriptor();
                // Apparently, some Android devices (e.g. Nexus 5) only support read operations of
                // multiples of the endpoint buffer size. So there you have it!
                inputStream_ = new FixedReadBufferedInputStream(new FileInputStream(fd), 1024);
                outputStream_ = new BufferedOutputStream(new FileOutputStream(fd), 1024);

                // Soft-open the connection
                outputStream_.write(0x00);
                outputStream_.flush();

                // We're going to block now. We're counting on the IOIO to
                // write back a byte, or otherwise we're locked until
                // physical disconnection. This is a known OpenAccessory
                // bug:
                // http://code.google.com/p/android/issues/detail?id=20545
                while (inputStream_.read() != 1) {
                    trySleep();
                }

                success = true;
                return true;
            } catch (IOException e) {
                Log.v(TAG, "Failed to open streams", e);
                return false;
            } finally {
                if (!success) {
                    try {
                        fileDescriptor_.close();
                    } catch (IOException e) {
                        Log.e(TAG, "Failed to close file descriptor.", e);
                    }
                    fileDescriptor_ = null;
                }
            }
        } finally {
            if (!success && pendingIntent_ != null) {
                pendingIntent_.cancel();
                pendingIntent_ = null;
            }
        }
    }

    private void registerReceiver() {
        IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
        activity_.registerReceiver(this, filter);
    }

    private void unregisterReceiver() {
        activity_.unregisterReceiver(this);
    }

    private void trySleep() {
        synchronized (AccessoryConnectionBootstrap.this) {
            try {
                AccessoryConnectionBootstrap.this.wait(1000);
            } catch (InterruptedException ignored) {
            }
        }
    }

    private enum InstanceState {
        INIT, CONNECTED, DEAD
    }

    private class Connection implements IOIOConnection {
        private InstanceState instanceState_ = InstanceState.INIT;

        @Override
        public InputStream getInputStream() {
            return inputStream_;
        }

        @Override
        public OutputStream getOutputStream() {
            return outputStream_;
        }

        @Override
        public boolean canClose() {
            return false;
        }

        @Override
        public void waitForConnect() throws ConnectionLostException {
            synchronized (AccessoryConnectionBootstrap.this) {
                if (instanceState_ != InstanceState.INIT) {
                    throw new IllegalStateException("waitForConnect() may only be called once");
                }

                try {
                    AccessoryConnectionBootstrap.this.waitForConnect();
                    instanceState_ = InstanceState.CONNECTED;
                } catch (ConnectionLostException e) {
                    instanceState_ = InstanceState.DEAD;
                    throw e;
                }
            }
        }

        @Override
        public void disconnect() {
            synchronized (AccessoryConnectionBootstrap.this) {
                if (instanceState_ != InstanceState.DEAD) {
                    AccessoryConnectionBootstrap.this.disconnect();
                    instanceState_ = InstanceState.DEAD;
                }
            }
        }

        @Override
        protected void finalize() {
            disconnect();
        }
    }
}