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

import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.util.IOIOApplicationHelper;
import ioio.lib.util.IOIOConnectionRegistry;
import ioio.lib.util.IOIOLooperProvider;

import android.content.ContextWrapper;

public class IOIOAndroidApplicationHelper extends IOIOApplicationHelper {
	private final ContextWrapper contextWrapper_;

	public IOIOAndroidApplicationHelper(ContextWrapper wrapper,
			IOIOLooperProvider provider) {
		super(provider);
		contextWrapper_ = wrapper;
	}

	static {
		IOIOConnectionRegistry
				.addBootstraps(new String[] {
						"ioio.lib.impl.SocketIOIOConnectionBootstrap",
						"ioio.lib.android.accessory.AccessoryConnectionBootstrap",
						"ioio.lib.android.bluetooth.BluetoothIOIOConnectionBootstrap" });
	}

	public void create() {
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			if (bootstrap instanceof ContextWrapperDependent) {
				((ContextWrapperDependent) bootstrap).onCreate(contextWrapper_);
			}
		}
	}

	public void destroy() {
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			if (bootstrap instanceof ContextWrapperDependent) {
				((ContextWrapperDependent) bootstrap).onDestroy();
			}
		}
	}

	@Override
	public void start() {
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			if (bootstrap instanceof ContextWrapperDependent) {
				((ContextWrapperDependent) bootstrap).open();
			}
		}
		super.start();
	}

	@Override
	public void stop() {
		super.stop();
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			if (bootstrap instanceof ContextWrapperDependent) {
				((ContextWrapperDependent) bootstrap).close();
			}
		}
	}

	public void restart() {
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			if (bootstrap instanceof ContextWrapperDependent) {
				((ContextWrapperDependent) bootstrap).reopen();
			}
		}
	}
}
