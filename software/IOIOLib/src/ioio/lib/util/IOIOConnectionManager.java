/*
 * Copyright 2012 Ytai Ben-Tsvi. All rights reserved.
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

import ioio.lib.spi.IOIOConnectionFactory;

import java.util.Collection;
import java.util.LinkedList;

/**
 * Manages IOIO threads per connection.
 * <p>
 * This class will take care of creating a thread for each possible IOIO
 * connection, and cleaning up these threads. Client is responsible for providing the
 * actual threads, by implementing the IOIOConnectionThreadProvider interface.
 * <p>
 * Basic usage is:
 * <ul>
 * <li>Create a IOIOConnectionManager instance, pass a IOIOConnectionThreadProvider.</li>
 * <li>Call {@link #start()}. This will invoke {@link IOIOConnectionThreadProvider#createThreadFromFactory(IOIOConnectionFactory)}
 * for every possible connection (more precisely, for every IOIOConnectionFactory). Return null if you do not wish to create a thread for a certain connection type.</li>
 * <li>When done, call {@link #stop()}. This will call {@link Thread#abort()} for each running thread and then join them.</li>
 * </ul>
 */
public class IOIOConnectionManager {
	private final IOIOConnectionThreadProvider provider_;

	public IOIOConnectionManager(IOIOConnectionThreadProvider provider) {
		provider_ = provider;
	}

	public abstract static class Thread extends java.lang.Thread {
		public abstract void abort();
	}

	public void start() {
		createAllThreads();
		startAllThreads();
	}

	public void stop() {
		abortAllThreads();
		try {
			joinAllThreads();
		} catch (InterruptedException e) {
		}
	}

	public interface IOIOConnectionThreadProvider {
		public Thread createThreadFromFactory(IOIOConnectionFactory factory);
	}

	private Collection<Thread> threads_ = new LinkedList<Thread>();

	private void abortAllThreads() {
		for (Thread thread : threads_) {
			thread.abort();
		}
	}

	private void joinAllThreads() throws InterruptedException {
		for (Thread thread : threads_) {
			thread.join();
		}
	}

	private void createAllThreads() {
		threads_.clear();
		Collection<IOIOConnectionFactory> factories = IOIOConnectionRegistry
				.getConnectionFactories();
		for (IOIOConnectionFactory factory : factories) {
			Thread thread = provider_.createThreadFromFactory(factory);
			if (thread != null) {
				threads_.add(thread);
			}
		}
	}

	private void startAllThreads() {
		for (Thread thread : threads_) {
			thread.start();
		}
	}

}