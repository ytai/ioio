/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
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
package ioio.lib.util.android;

import android.app.Service;
import android.content.Intent;

import ioio.lib.impl.SocketIOIOConnection;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.IOIOLooperProvider;

/**
 * A convenience class for easy creation of IOIO-based services.
 * <p>
 * It is used by creating a concrete {@link Service} in your application, which
 * extends this class. This class then takes care of proper creation and
 * abortion of the IOIO connection and of a dedicated thread for IOIO
 * communication.
 * <p>
 * In the basic usage the client should extend this class and implement
 * {@link #createIOIOLooper()}, which should return an implementation of the
 * {@link IOIOLooper} interface. In this implementation, the client implements
 * the {@link IOIOLooper#setup(ioio.lib.api.IOIO)} method, which gets called as
 * soon as communication with the IOIO is established, and the
 * {@link IOIOLooper#loop()} method, which gets called repetitively as long as
 * the IOIO is connected.
 * <p>
 * In addition, the {@link IOIOLooper#disconnected()} method may be overridden
 * in order to execute logic as soon as a disconnection occurs for whichever
 * reason. The {@link IOIOLooper#incompatible()} method may be overridden in
 * order to take action in case where a IOIO whose firmware is incompatible with
 * the IOIOLib version that application is built with.
 * <p>
 * In a more advanced use case, more than one IOIO is available. In this case, a
 * thread will be created for each IOIO, whose semantics are as defined above.
 * If the client needs to be able to distinguish between them, it is possible to
 * override {@link #createIOIOLooper(String, Object)} instead of
 * {@link #createIOIOLooper()}. The first argument provided will contain the
 * connection class name, such as ioio.lib.impl.SocketIOIOConnection for a
 * connection established over a TCP socket (which is used over ADB). The second
 * argument will contain information specific to the connection type. For
 * example, in the case of {@link SocketIOIOConnection}, the second argument
 * will contain an {@link Integer} representing the local port number.
 */
public abstract class IOIOService extends Service implements IOIOLooperProvider {

    private final IOIOAndroidApplicationHelper helper_ = new IOIOAndroidApplicationHelper(this, this);
    private boolean started_ = false;

    /**
     * Subclasses should call this method from their own onCreate() if
     * overloaded. It takes care of connecting with the IOIO.
     */
    @Override
    public void onCreate() {
        super.onCreate();
        helper_.create();
    }

    /**
     * Subclasses should call this method from their own onDestroy() if
     * overloaded. It takes care of connecting with the IOIO.
     */
    @Override
    public void onDestroy() {
        stop();
        helper_.destroy();
        super.onDestroy();
    }

    private void start(Intent intent) {
        if (!started_) {
            helper_.start();
            started_ = true;
        } else {
            helper_.restart();
        }
    }

    /**
     * This is the old onStart() method. Override and/or call this method only
     * if you're using Android API level lower than 5. Otherwise you shoud call
     * the onStartCommand() method.
     */
    @Override
    public void onStart(Intent intent, int startId) {
        start(intent);
    }

    /**
     * Subclasses should call this method from their own onStartCommand() if
     * overloaded. It takes care of connecting with the IOIO.
     */
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        start(intent);
        // We want this service to continue running until it is explicitly
        // stopped, so return sticky.
        return START_STICKY;
    }

    /**
     * Subclasses should call this method if they wish to disconnect from the
     * IOIO(s) until the next onStart().
     */
    protected void stop() {
        if (started_) {
            helper_.stop();
            started_ = false;
        }
    }

    /**
     * Subclasses must either implement this method or its other overload by
     * returning an implementation of {@link IOIOLooper}. A dedicated thread
     * will be created for each available IOIO, from which the
     * {@link IOIOLooper}'s methods will be invoked. <code>null</code> may be
     * returned if the client is not interested to create a thread for this
     * IOIO. In multi-IOIO scenarios, where you want to identify which IOIO the
     * thread is for, consider overriding
     * {@link #createIOIOLooper(String, Object)} instead.
     *
     * @return An implementation of {@link IOIOLooper}, or <code>null</code> to
     * skip.
     */
    protected IOIOLooper createIOIOLooper() {
        throw new RuntimeException("Client must override one of the createIOIOLooper overloads!");
    }

    @Override
    public IOIOLooper createIOIOLooper(String connectionType, Object extra) {
        return createIOIOLooper();
    }

}
