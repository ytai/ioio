package ioio.emulator_bridge;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

import purejavacomm.CommPort;
import purejavacomm.CommPortIdentifier;
import purejavacomm.NoSuchPortException;
import purejavacomm.SerialPort;

public class Main {

	/**
	 * @param args
	 * @throws IOException
	 * @throws InterruptedException
	 */
	public static void main(String[] args) throws InterruptedException,
			IOException {
		if (args.length != 1) {
			System.err.println("Usage: java -jar bridge.jar <serial_port>");
			System.err.println("Don't forget to first run:");
			System.err.println("adb forward tcp:4545 tcp:4545");
			System.exit(1);
		}
		final String portName = args[0];
		while (true) {
			runBridge(portName);
			Thread.sleep(1000);
		}
	}

	private static class BridgeThread extends Thread {
		private final InputStream in_;
		private final OutputStream out_;
		private final Object toClose_;

		public BridgeThread(InputStream in, OutputStream out, Object toClose) {
			in_ = in;
			out_ = out;
			toClose_ = toClose;
		}

		@Override
		public void run() {
			try {
				int r;
				while ((r = in_.read()) != -1) {
					out_.write(r);
				}
			} catch (IOException e) {
			} finally {
				try {
					System.out.println("Closing!");
					if (toClose_ instanceof Socket) {
						((Socket) toClose_).close();
					} else if (toClose_ instanceof SerialPort) {
						((SerialPort) toClose_).close();
					}
				} catch (IOException e) {
				}
			}
		}

	}

	private static void runBridge(String portName) throws InterruptedException,
			IOException {
		SerialPort port = null;
		Socket socket = null;
		try {
			port = openPort(portName);
			System.err.println("Port is open.");
			socket = openSocket();
			System.err.println("Socket is open.");
			Thread t1 = new BridgeThread(socket.getInputStream(),
					port.getOutputStream(), port);
			Thread t2 = new BridgeThread(port.getInputStream(),
					socket.getOutputStream(), socket);
			t1.start();
			t2.start();
			System.err.println("Bridge running.");
			t1.join();
			t2.join();
			System.err.println("Bridge closed.");
		} finally {
			if (port != null) {
				port.close();
			}
			if (socket != null) {
				try {
					socket.close();
				} catch (IOException e) {
				}
			}
		}
	}

	private static Socket openSocket() {
		while (true) {
			try {
				return new Socket("localhost", 4545);
			} catch (Exception e) {
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e1) {
				}
			}
		}
	}

	private static SerialPort openPort(String portName) {
		SerialPort port = null;
		while (true) {
			try {
				CommPortIdentifier identifier = CommPortIdentifier
						.getPortIdentifier(portName);
				CommPort commPort = identifier.open(Main.class.getName(), 1000);
				port = (SerialPort) commPort;
				port.enableReceiveThreshold(1);
				port.setDTR(true);
				Thread.sleep(100);
				return port;
			} catch (NoSuchPortException e) {
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e1) {
				}
			} catch (Exception e) {
				if (port != null) {
					port.close();
				}
			}
		}
	}
}
