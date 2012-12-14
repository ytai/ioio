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

package ioio.lib.jsr82;

import ioio.lib.api.IOIOConnection;
import ioio.lib.api.exception.ConnectionLostException;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import javax.bluetooth.ServiceRecord;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;

import android.util.Log;

public class JSR82BluetoothIOIOConnection implements IOIOConnection {
	private static final String TAG = "JSR82BluetoothIOIOConnection";
	private StreamConnection socket_ = null;
	private InputStream socketIn = null;
	private OutputStream socketOut = null;
	private boolean disconnect_ = false;
	private final ServiceRecord serviceRecord_;

	public JSR82BluetoothIOIOConnection(ServiceRecord serviceRecord) {
	  serviceRecord_ = serviceRecord;
	}

	@Override
	public synchronized void waitForConnect() throws ConnectionLostException {
		if (disconnect_) {
			throw new ConnectionLostException();
		}
		try {
			String url = serviceRecord_.getConnectionURL(0, false);
			this.socket_ = (StreamConnection)Connector.open(url);
			this.socketIn = socket_.openInputStream();
			this.socketOut = socket_.openOutputStream();
		} catch (IOException e) {
		  throw new ConnectionLostException(e);
		}
	}

	@Override
	public synchronized void disconnect() {
		if (disconnect_) {
			return;
		}
		Log.v(TAG, "Client initiated disconnect");
		disconnect_ = true;
		if (socket_ != null) {
			try {
				socket_.close();
			} catch (IOException e) {
			}
		}
	}

	@Override
	public InputStream getInputStream() throws ConnectionLostException {
		return socketIn;
	}

	@Override
	public OutputStream getOutputStream() throws ConnectionLostException {
		return socketOut;
	}
}
