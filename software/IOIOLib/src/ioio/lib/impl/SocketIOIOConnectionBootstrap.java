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
package ioio.lib.impl;

import ioio.lib.api.IOIOConnection;
import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.spi.IOIOConnectionFactory;

import java.util.Collection;

public class SocketIOIOConnectionBootstrap implements IOIOConnectionBootstrap {
	/** The TCP port used for communicating with the IOIO board. */
	public static final int IOIO_PORT = 4545;

	@Override
	public void getFactories(Collection<IOIOConnectionFactory> result) {
		result.add(new IOIOConnectionFactory() {
			private Integer port_ = Integer.valueOf(IOIO_PORT);
			
			@Override
			public String getType() {
				return SocketIOIOConnection.class.getCanonicalName();
			}
			
			@Override
			public Object getExtra() {
				return port_;
			}
			
			@Override
			public IOIOConnection createConnection() {
				return new SocketIOIOConnection(IOIO_PORT);
			}
		});
	}
}
