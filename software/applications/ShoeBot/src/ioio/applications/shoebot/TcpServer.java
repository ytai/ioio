package ioio.applications.shoebot;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;

public class TcpServer implements Runnable {
	interface LineListener {
		public void onLine(String line);
	}

	private LineListener listener_;
	private int port_;
	private ServerSocket socketServer_;
	private Socket socket_;

	public TcpServer(int port, LineListener listener) {
		listener_ = listener;
		port_ = port;
		new Thread(this).start();
	}
	
	public void write(String str) throws IOException {
		socket_.getOutputStream().write(str.getBytes());
	}

	void abort() {
		if (socketServer_ != null) {
			try {
				socketServer_.close();
			} catch (IOException e) {
			}
		}
		if (socket_ != null) {
			try {
				socket_.close();
			} catch (IOException e) {
			}
		}
	}

	@Override
	public void run() {
		try {
			Thread.currentThread().setPriority(Thread.MAX_PRIORITY);
			socketServer_ = new ServerSocket(port_);
			while (true) {
				socket_ = socketServer_.accept();
				BufferedReader reader = new BufferedReader(
						new InputStreamReader(socket_.getInputStream()));
				String line;
				while ((line = reader.readLine()) != null) {
					listener_.onLine(line);
				}
			}
		} catch (IOException e) {
		}
	}
}
