package ioio.connection_tester;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class AsyncRunner extends Thread {
	private BlockingQueue<Runnable> queue_ = new LinkedBlockingQueue<Runnable>();
	
	public void add(Runnable r) {
		queue_.add(r);
	}

	@Override
	public void run() {
		try {
			while (true) {
				Runnable runnable = queue_.take();
				runnable.run();
			}
		} catch (InterruptedException e) {
		}
	}
}
