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

import ioio.lib.util.IOIOBaseApplicationHelper;
import ioio.lib.util.IOIOConnectionRegistry;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.IOIOLooperProvider;
import android.content.ContextWrapper;

/**
 * A helper class for creating different kinds of IOIO based applications on
 * Android.
 * <p>
 * <i><b>Note</b>: Consider using {@link IOIOActivity} or {@link IOIOService}
 * for easy creation of IOIO activities and services. This class is intended for
 * more advanced use-cases not covered by them.</i>
 * <p>
 * This class implements a common life-cycle for Android applications
 * interacting with IOIO devices. Usage is as follows:
 * <ul>
 * <li>Create an instance of {@link IOIOAndroidApplicationHelper}, passing a
 * {@link IOIOLooperProvider} and a {@link ContextWrapper} to the constructor.</li>
 * <li>Call {@link #create()}, {@link #destroy()}, {@link #start()},
 * {@link #stop()} and {@link #restart()} from the respective Android life-cycle
 * event methods.</li>
 * <li>{@link #start()} will trigger callback of
 * {@link IOIOLooperProvider#createIOIOLooper(String, Object)} for every
 * possible IOIO connection and create a new thread for interacting with this
 * IOIO, through the created {@link IOIOLooper}.</li>
 * <li>{@link #stop()} will make sure proper cleanup and disconnection is done.</li>
 * </ul>
 */
public class IOIOAndroidApplicationHelper extends IOIOBaseApplicationHelper {
	private final AndroidIOIOConnectionManager manager_;

	public IOIOAndroidApplicationHelper(ContextWrapper wrapper,
			IOIOLooperProvider provider) {
		super(provider);
		manager_ = new AndroidIOIOConnectionManager(wrapper, this);
	}

	static {
		IOIOConnectionRegistry
				.addBootstraps(new String[] {
						"ioio.lib.impl.SocketIOIOConnectionBootstrap",
						"ioio.lib.android.accessory.AccessoryConnectionBootstrap",
						"ioio.lib.android.bluetooth.BluetoothIOIOConnectionBootstrap" });
	}

	public void create() {
		manager_.create();
	}

	public void destroy() {
		manager_.destroy();
	}

	public void start() {
		manager_.start();
	}

	public void stop() {
		manager_.stop();
	}

	public void restart() {
		manager_.restart();
	}
}
