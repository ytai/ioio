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
package ioio.lib.api;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import ioio.lib.api.exception.ConnectionLostException;

/**
 * A connection abstraction for IOIO. This interface enables a {@link IOIO}
 * instance to communicate with a physical IOIO board.
 * 
 * This interface is intended for providing a custom communication channel
 * between the IOIOLib and a IOIO board. The client should implement the
 * interface according to the requirements detailed below.
 * 
 * It is very important that all implementations adhere to the following
 * requirements:
 * <ul>
 * <li>{@link #waitForConnect()} and {@link #disconnect()} should be
 * synchronized on the instance.</li>
 * <li>{@link #waitForConnect()} normally blocks until a connection is
 * established.</li>
 * <li>{@link #disconnect()} may be called at any point in time:
 * <ol>
 * <li>If called before {@link #waitForConnect()} has been called, when
 * #waitForConnect() gets called it will exit immediately with e
 * {@link ConnectionLostException}.</li>
 * <li>If called during {@link #waitForConnect()}, the thread blocking on
 * {@link #waitForConnect()} will exit with a {@link ConnectionLostException}.</li>
 * <li>If called after {@link #waitForConnect()} has completed - the connection
 * should be dropped.</li>
 * <li>If called after {@link #disconnect()} has already been called, it should
 * do nothing</li>
 * </ol>
 * </li>
 * <li>Requirements 2-3 above also apply for the case of a physical drop of the
 * underlying connection</li>
 * <li>After {@link #disconnect()} has been called, the instance becomes a
 * "zombie". In order to re-establish a connection, a new instance will be
 * created.</li>
 * <li>The same behavior should be
 * <li>It can be assumed that {@link #waitForConnect()} will not be called more
 * than once on a single instance.</li>
 * </ul>
 * 
 * Once connected, this instance provides an {@link InputStream} and and
 * {@link OutputStream} via the respective {@link #getInputStream()} and
 * {@link #getOutputStream()} methods. These streams do not need to be
 * thread-safe.
 * 
 * As soon a disconnection occurs, either as result of a drop of the underlying
 * connection or as result of an explicit call to {@link #disconnect()}, the
 * {@link InputStream} must unblock a thread blocking on read(), either
 * returning -1 (EOF) or throwing an {@link IOException}. Any further calls to
 * read() should exit immediately in one of the above ways.
 * 
 */
public interface IOIOConnection {
	void waitForConnect() throws ConnectionLostException;

	void disconnect();

	InputStream getInputStream() throws ConnectionLostException;

	OutputStream getOutputStream() throws ConnectionLostException;
}
