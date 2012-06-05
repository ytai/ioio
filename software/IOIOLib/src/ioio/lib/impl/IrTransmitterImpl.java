/*
 * Copyright 2012 Markus Lanthaler <mail@markus-lanthaler.com>. All rights reserved.
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

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IrTransmitter;
import ioio.lib.api.exception.ConnectionLostException;

import java.io.IOException;

import android.util.Log;

public class IrTransmitterImpl extends AbstractResource implements IrTransmitter {
	private DigitalOutput outPin;
	private int pinNum_;

	public IrTransmitterImpl(IOIOImpl ioio, DigitalOutput tx, int pinNum) throws ConnectionLostException {
		super(ioio);
		outPin = tx;
		pinNum_ = pinNum;
	}

	@Override
	public void close() {
		outPin.close();
	}

	private void sendIrData(int data) {
		try {
			ioio_.protocol_.irData(data);
		} catch (IOException e) {
			Log.e("IrImpl", e.getMessage());
		}
	}

	private void startTransmission() {
		try {
			ioio_.protocol_.irStartTransmission(pinNum_);
		} catch (IOException e) {
			Log.e("IrImpl", e.getMessage());
		}
	}

	@Override
	public void transmitIrCommand(int[] data) {

		for (int i = 0; i < data.length; i++)
		{
			sendIrData(data[i]);
		}

		startTransmission();
	}
}
