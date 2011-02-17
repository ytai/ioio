package ioio.lib.pic;

import ioio.lib.IOIOException.ConnectionLostException;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

/**
 * Uart implementation for the IOIO.
 *
 * @author arshan
 */
public class Uart implements IOIOPacketListener {

	// Define some known good constants?
	public static final int NO_PARITY = 0;
	public static final int ODD_PARITY = 2; // I'd prefer Odd be 1 for artistic value :)
	public static final int EVEN_PARITY = 1;

	// TODO(arshan): what is the highest supported baud?
	public static final int BAUD_2400 = 2400;
	public static final int BAUD_4800 = 4800;
	public static final int BAUD_9600 = 9600;
	public static final int BAUD_19200 = 19200;
	public static final int BAUD_28800 = 28800;
	public static final int BAUD_33600 = 33600;

	public static final float ONE_STOP_BIT = 1;
	public static final float TWO_STOP_BITS = 2;
	public static final float ONE_AND_HALF_STOP_BITS = 1.5f;

	private int baud = 0; // Support all standard bauds (or send in divisor and support arbitrary?)
	private int parity = 0; // even/odd/no
	private float stop_bits = 0; // 1/1.5/2
	private int data_bits = 8;	// 7/8/9
	private DigitalInput rx;
	private DigitalOutput tx;

	private IOIOImpl ioio;

	// Uart module on the ioio.
	private int uartNum;

	private int rate;
	private boolean fourX = false;

	// Cached IOIOPackets
	private IOIOPacket configure;
	private IOIOPacket setRx;
	private IOIOPacket setTx;

	// Java buffers, how much buffer do we have on the device?
	BlockingQueue<Byte> incoming = new LinkedBlockingQueue<Byte>();
	BlockingQueue<Byte> outgoing = new LinkedBlockingQueue<Byte>();

	Uart(IOIOImpl ioio, int module, int rx, int tx, int baud, int parity, float stop) throws ConnectionLostException {
		this.stop_bits = stop;
		this.parity = parity;
		this.baud = baud;
		this.rx = ioio.openDigitalInput(rx);
		this.tx = ioio.openDigitalOutput(tx, false);
		this.ioio = ioio;
		uartNum = module;
		init();
	}

	private void init() {
		calculateRates();

		configure = new IOIOPacket(
				Constants.UART_CONFIGURE,
				new byte[]{
						(byte) (uartNum << 6
								| (fourX ? 0x80 : 0x00)
								| (stop_bits == TWO_STOP_BITS ? 0x40 : 0)
								| (parity & 0x3) ),
						(byte) (rate & 0xFF),
						(byte) ((rate >> 8) & 0xFF)
				}
		);

		setRx = new IOIOPacket(Constants.UART_SET_RX,
				new byte[]{(byte)rx.pin, (byte)(0x80|uartNum)});

		setTx = new IOIOPacket(Constants.UART_SET_TX,
				new byte[]{(byte)tx.pin, (byte)(0x80|uartNum)});

		// Since the rx and tx pins are initialized as DigitalI/O pins we already have
		// them setup to be used as rx/tx here.
		ioio.queuePacket(configure);
		ioio.queuePacket(setRx);
		ioio.queuePacket(setTx);
	}

	private void calculateRates() {
		// TODO(arshan) : read the datasheet, there must be a real way of sorting this out.
		fourX = baud >= 38400 ? true : false;
		rate = calculateUartRate(baud, fourX);
	}

	private int calculateUartRate(int baud, boolean fourX) {
		long numerator = fourX ? 4000000 : 1000000;
		return (int) (Math.floor(numerator/baud) - 1);
	}

	public DataOutputStream openDataOutputStream() {
		return new DataOutputStream(openOutputStream());
	}

	public DataInputStream openDataInputStream() {
		return new DataInputStream(openInputStream());
	}

	public UARTOutputStream openOutputStream() {
		return new UARTOutputStream(this);
	}

	public UARTInputStream openInputStream() {
		return new UARTInputStream(this);
	}

	int readByte() {
		try {
			return incoming.take();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		return -1; // nonsense.
	}

	void writeByte(int val) {
		outgoing.offer((byte)val);
	}

	public class UARTOutputStream extends OutputStream {

		Uart parent;

		protected UARTOutputStream(Uart uart) {
			parent = uart;
		}

		@Override
		public void write(int val) throws IOException {
			parent.writeByte(val);
		}
	}

	public class UARTInputStream extends InputStream {

		Uart parent;

		protected UARTInputStream(Uart uart) {
			parent = uart;
		}

		@Override
		public int read() throws IOException {
			return parent.readByte();
		}
	}

	@Override
    public void handlePacket(IOIOPacket packet) {
		switch (packet.message) {
		case Constants.UART_RX:
			// received bytes from ioio.
			if ((packet.payload[0] >> 6) == uartNum) {
				// TODO(arshan): consider our own buffer implementation for perf. ie. circular byte[]
				for (int x = 1; x < packet.payload.length; x++) {
					incoming.offer(packet.payload[x]);
				}
			}
			break;

		case Constants.UART_SET_RX:
		case Constants.UART_SET_TX:
		case Constants.UART_CONFIGURE:
			// consider catching these to get the confirmations.
			break;

		case Constants.UART_TX_STATUS:
			break;
		}

	}
}