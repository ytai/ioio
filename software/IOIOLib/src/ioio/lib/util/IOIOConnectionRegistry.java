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
package ioio.lib.util;

import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.spi.IOIOConnectionFactory;

import java.util.Collection;
import java.util.LinkedList;

import android.util.Log;

public class IOIOConnectionRegistry {
	/**
	 * Get all available connection specifications. This is a list of all
	 * currently available communication channels in which a IOIO may be
	 * available. The client typically passes elements of this collection to
	 * {@link #create(ConnectionSpec)}, possibly after filtering based on the
	 * specification's properties.
	 * 
	 * @return A collection of specifications.
	 */
	public static Collection<IOIOConnectionFactory> getConnectionFactories() {
		Collection<IOIOConnectionFactory> result = new LinkedList<IOIOConnectionFactory>();
		for (IOIOConnectionBootstrap bootstrap : bootstraps_) {
			bootstrap.getFactories(result);
		}
		return result;
	}

	/**
	 * For advanced usage only! Used for special runtime handling of bootstrap
	 * classes.
	 * 
	 * @return The bootstraps.
	 */
	public static Collection<IOIOConnectionBootstrap> getBootstraps() {
		return bootstraps_;
	}

	/**
	 * For advanced usage only! Add platform-specific connection bootstrap
	 * classes. Call before calling {@link #getConnectionFactories()}.
	 */
	public static void addBootstraps(String[] classNames) {
		for (String className : classNames) {
			addBootstrap(className);
		}
	}

	private static final String TAG = "IOIOConnectionRegistry";

	private static Collection<IOIOConnectionBootstrap> bootstraps_;

	static {
		bootstraps_ = new LinkedList<IOIOConnectionBootstrap>();
		String[] classNames = new String[] { "ioio.lib.impl.SocketIOIOConnectionBootstrap" };
		addBootstraps(classNames);
	}

	private static void addBootstrap(String className) {
		try {
			Class<? extends IOIOConnectionBootstrap> bootstrapClass = Class
					.forName(className).asSubclass(
							IOIOConnectionBootstrap.class);
			bootstraps_.add(bootstrapClass.newInstance());
			Log.d(TAG, "Successfully added bootstrap class: " + className);
		} catch (ClassNotFoundException e) {
			Log.d(TAG, "Bootstrap class not found: " + className
					+ ". Not adding.");
		} catch (RuntimeException e) {
			Log.e(TAG,
					"Runtime exception caught while attempting to initialize accessory connection factory",
					e);
			throw e;
		} catch (Exception e) {
			Log.w(TAG,
					"Exception caught while attempting to initialize accessory connection factory",
					e);
		}
	}

}
