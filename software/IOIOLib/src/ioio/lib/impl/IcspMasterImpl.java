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

import ioio.lib.api.IcspMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.DataModuleListener;

import java.io.IOException;
import java.util.LinkedList;
import java.util.Queue;

class IcspMasterImpl extends AbstractResource implements IcspMaster,
		DataModuleListener {
	private Queue<Integer> resultQueue_ = new LinkedList<Integer>();
	private int rxRemaining_ = 0;

	public IcspMasterImpl(IOIOImpl ioio) throws ConnectionLostException {
		super(ioio);
	}

	@Override
	synchronized public void dataReceived(byte[] data, int size) {
		assert (size == 2);
		int result = (byteToInt(data[1]) << 8) | byteToInt(data[0]);
		resultQueue_.add(result);
		notifyAll();
	}

	@Override
	synchronized public void reportAdditionalBuffer(int bytesToAdd) {
		rxRemaining_ += bytesToAdd;
		notifyAll();
	}

	@Override
	synchronized public void enterProgramming() throws ConnectionLostException {
		checkState();
		try {
			ioio_.protocol_.icspEnter();
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	synchronized public void exitProgramming() throws ConnectionLostException {
		checkState();
		try {
			ioio_.protocol_.icspExit();
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	synchronized public void executeInstruction(int instruction)
			throws ConnectionLostException {
		checkState();
		try {
			ioio_.protocol_.icspSix(instruction);
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	synchronized public void readVisi() throws ConnectionLostException,
			InterruptedException {
		checkState();
		while (rxRemaining_ < 2 && state_ == State.OPEN) {
			wait();
		}
		checkState();
		rxRemaining_ -= 2;
		try {
			ioio_.protocol_.icspRegout();
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	synchronized public void close() {
		super.close();
		ioio_.closeIcsp();
	}

	@Override
	public synchronized void disconnected() {
		super.disconnected();
		notifyAll();
	}
	
	private static int byteToInt(byte b) {
		return ((int) b) & 0xFF;
	}

	@Override
	public synchronized int waitVisiResult() throws ConnectionLostException,
			InterruptedException {
		checkState();
		while (resultQueue_.isEmpty() && state_ == State.OPEN) {
			wait();
		}
		checkState();
		return resultQueue_.remove();
	}
}
