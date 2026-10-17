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

import java.io.InputStream;
import java.io.OutputStream;

import ioio.lib.api.exception.ConnectionLostException;

/**
 * A low-level transport connection to an IOIO board.
 */
public interface IOIOConnection {
    /**
     * Waits until the underlying transport is connected and ready for I/O.
     *
     * @throws ConnectionLostException The connection was lost before it became ready.
     */
    void waitForConnect() throws ConnectionLostException;

    /**
     * Disconnects the underlying transport.
     */
    void disconnect();

    /**
     * Gets the input stream used for reading data from the connection.
     *
     * @return The connection input stream.
     * @throws ConnectionLostException The connection was lost before the stream could be obtained.
     */
    InputStream getInputStream() throws ConnectionLostException;

    /**
     * Gets the output stream used for writing data to the connection.
     *
     * @return The connection output stream.
     * @throws ConnectionLostException The connection was lost before the stream could be obtained.
     */
    OutputStream getOutputStream() throws ConnectionLostException;

    /**
     * Can this connection be closed. Normally the answer would be "true", but
     * some weird connections cannot be closed and need the higher layer to do
     * a "soft close" instead.
     *
     * @return true This connection can be closed.
     */
    boolean canClose();
}
