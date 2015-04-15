/*
 * Copyright 2013 Ytai Ben-Tsvi. All rights reserved.
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
import ioio.lib.util.IOIOConnectionManager;
import ioio.lib.util.IOIOConnectionRegistry;

import java.util.Collection;

import android.content.ContextWrapper;

/**
 * An extension of {@link IOIOConnectionManager} for the Android environment.
 * <p>
 * <i><b>Note</b>: This utility is not typically intended for use by end-users.
 * See {@link IOIOActivity} and {@link IOIOSerive} for high-level utilities,
 * which facilitate the creation of Android-based applications.</i>
 * <p>
 * The reason for this extension is that some Android-related connections (e.g.
 * OpenAccessory) are tightly coupled with the Android Activity life-cycle and
 * require to be notified when certain life-cycle events occur.
 * <p>
 * Use this class similarly to {@link IOIOConnectionManager}, but in addition
 * call {@link #create()}, {@link #destroy()}, {@link #start()}, {@link #stop()}
 * and {@link #restart()} from your activity's {@code onCreate()},
 * {@code onDestroy()}, {@code onStart()}, {@code onStop()} and
 * {@code onRestart()}, respectively.
 */
public class AndroidIOIOConnectionManager extends IOIOConnectionManager {
	private final ContextWrapper contextWrapper_;
	private Collection<IOIOConnectionBootstrap> bootstraps_ = IOIOConnectionRegistry
			.getBootstraps();

	public AndroidIOIOConnectionManager(ContextWrapper wrapper,
			IOIOConnectionThreadProvider provider) {
		super(provider);
		contextWrapper_ = wrapper;
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
