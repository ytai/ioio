package ioio.lib.new_impl;

import ioio.lib.api.IOIO;
import ioio.lib.api.Uart;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.new_impl.FlowControlledOutputStream.Sender;
import ioio.lib.new_impl.IncomingState.DataModuleListener;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.util.Log;

public class UartImpl extends AbstractResource implements DataModuleListener, Sender, Uart {
	private static final int MAX_PACKET = 64;
	
	final int uartNum_;
	final int rxPinNum_;
	final int txPinNum_;
	FlowControlledOutputStream outgoing_ = new FlowControlledOutputStream(this, MAX_PACKET);
	QueueInputStream incoming_ = new QueueInputStream();
	
	public UartImpl(IOIOImpl ioio, int txPin, int rxPin, int uartNum) throws ConnectionLostException {
		super(ioio);
		uartNum_ = uartNum;
		rxPinNum_ = rxPin;
		txPinNum_ = txPin;
	}

	@Override
	public void dataReceived(byte[] data, int size) {
		incoming_.write(data, size);
	}

	@Override
	public void send(byte[] data, int size) {
		try {
			ioio_.protocol_.uartData(uartNum_, size, data);
		} catch (IOException e) {
			Log.e("UartImpl", e.getMessage());
		}
	}

	@Override
	synchronized public void close() {
		super.close();
		incoming_.close();
		outgoing_.close();
		ioio_.closeUart(uartNum_);
		if (rxPinNum_ != IOIO.INVALID_PIN_NUMBER) {
			ioio_.closePin(rxPinNum_);
		}
		if (txPinNum_ != IOIO.INVALID_PIN_NUMBER) {
			ioio_.closePin(txPinNum_);
		}
	}
	
	@Override
	synchronized public void disconnected() {
		super.disconnected();
		outgoing_.kill();
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
	public void reportBufferRemaining(int bytesRemaining) {
		outgoing_.readyToSend(bytesRemaining);
	}
}
