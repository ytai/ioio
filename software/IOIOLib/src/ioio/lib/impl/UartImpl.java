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

import ioio.lib.api.Uart;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.FlowControlledOutputStream.Sender;
import ioio.lib.impl.IncomingState.DataModuleListener;
import ioio.lib.impl.ResourceManager.Resource;
import ioio.lib.spi.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

class UartImpl extends AbstractResource implements DataModuleListener, Sender, Uart {
	private static final int MAX_PACKET = 64;

	private final Resource uart_;
	private final Resource rxPin_;
	private final Resource txPin_;
	private final FlowControlledOutputStream outgoing_ = new FlowControlledOutputStream(this, MAX_PACKET);
	private final QueueInputStream incoming_ = new QueueInputStream();

	public UartImpl(IOIOImpl ioio, Resource txPin, Resource rxPin, Resource uartNum) throws ConnectionLostException {
		super(ioio);
		uart_ = uartNum;
		rxPin_ = rxPin;
		txPin_ = txPin;
	}

	@Override
	public void dataReceived(byte[] data, int size) {
		incoming_.write(data, size);
	}

	@Override
	public void send(byte[] data, int size) {
		try {
			ioio_.protocol_.uartData(uart_.id, size, data);
		} catch (IOException e) {
			Log.e("UartImpl", e.getMessage());
		}
	}

	@Override
	synchronized public void close() {
		checkClose();
		incoming_.close();
		outgoing_.close();
		try {
			ioio_.protocol_.uartClose(uart_.id);
			ioio_.resourceManager_.free(uart_);
		} catch (IOException e) {
		}
		if (rxPin_ != null) {
			ioio_.closePin(rxPin_);
		}
		if (txPin_ != null) {
			ioio_.closePin(txPin_);
		}
		super.close();
	}

	@Override
	synchronized public void disconnected() {
		incoming_.kill();
		outgoing_.close();
		super.disconnected();
	}

	@Override
	public InputStream getInputStream() {
		return incoming_;
	}

	@Override
	public OutputStream getOutputStream() {
		return outgoing_;
	}

	@Override
	public void reportAdditionalBuffer(int bytesRemaining) {
		outgoing_.readyToSend(bytesRemaining);
	}
}
