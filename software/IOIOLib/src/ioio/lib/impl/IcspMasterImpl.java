package ioio.lib.impl;

import ioio.lib.api.IcspMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.impl.IncomingState.DataModuleListener;

import java.io.IOException;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

public class IcspMasterImpl extends AbstractResource implements IcspMaster,
		DataModuleListener {
	private BlockingQueue<Integer> resultQueue_ = new ArrayBlockingQueue<Integer>(
			Constants.BUFFER_SIZE);
	private int rxRemaining_ = 0;

	public IcspMasterImpl(IOIOImpl ioio) throws ConnectionLostException {
		super(ioio);
	}

	@Override
	synchronized public void dataReceived(byte[] data, int size) {
		assert (size == 2);
		int result = (((int) data[1]) << 8) | data[0];
		resultQueue_.add(result);
	}

	@Override
	synchronized public void reportAdditionalBuffer(int bytesToAdd) {
		rxRemaining_ += bytesToAdd;
		notifyAll();
	}

	@Override
	synchronized public void enterProgramming() throws ConnectionLostException {
		try {
			ioio_.protocol_.icspEnter();
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	synchronized public void exitProgramming() throws ConnectionLostException {
		try {
			ioio_.protocol_.icspExit();
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	synchronized public void executeInstruction(int instruction)
			throws ConnectionLostException {
		try {
			ioio_.protocol_.icspSix(instruction);
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	synchronized public void readVisi() throws ConnectionLostException,
			InterruptedException {
		while (rxRemaining_ < 2) {
			wait();
		}
		rxRemaining_ -= 2;
		try {
			ioio_.protocol_.icspRegout();
		} catch (IOException e) {
			throw new ConnectionLostException(e);
		}
	}

	@Override
	public BlockingQueue<Integer> getResultQueue() {
		return resultQueue_;
	}

	@Override
	synchronized public void close() {
		super.close();
		ioio_.closeIcsp();
	}

}
