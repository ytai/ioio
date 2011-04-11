package ioio.lib.new_impl;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.util.Log;

import ioio.lib.api.Uart;
import ioio.lib.new_impl.FlowControlledOutputStream.Sender;
import ioio.lib.new_impl.IncomingState;
import ioio.lib.new_impl.IncomingState.PinMode;
import ioio.lib.new_impl.IncomingState.UartListener;

public class UartImpl implements UartListener, Sender, Uart {
	// TODO(ytai): make simpler
	class PinListener implements IncomingState.PinListener {
		@Override
		public void opened(PinMode mode) {
		}

		@Override
		public void closed() {
		}

		@Override
		public void lost() {
		}

		@Override
		public void setValue(int value) {
		}
	}
	
	public IncomingState.PinListener rxListener = new PinListener();
	public IncomingState.PinListener txListener = rxListener;
	
	private static final int MAX_PACKET = 64;
	
	IOIOImpl ioio_;
	final int uartNum_;
	FlowControlledOutputStream outgoing_ = new FlowControlledOutputStream(this, MAX_PACKET);
	QueueInputStream incoming_ = new QueueInputStream();
	
	public UartImpl(IOIOImpl ioio, int txPin, int rxPin, int uartNum) {
		ioio_ = ioio;
		uartNum_ = uartNum;
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
		incoming_.close();
		outgoing_.close();
		ioio_.closeUart(uartNum_);
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
