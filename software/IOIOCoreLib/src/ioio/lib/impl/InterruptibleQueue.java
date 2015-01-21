package ioio.lib.impl;

import java.util.Queue;
import java.util.concurrent.ArrayBlockingQueue;

public class InterruptibleQueue<T> {
	private Queue<T> queue_;

	public static class Nudged extends Throwable {
		private static final long serialVersionUID = -7943717843515344247L;
	}

	public InterruptibleQueue(int capacity) {
		assert capacity > 0;
		queue_ = new ArrayBlockingQueue<T>(capacity);
	}

	public synchronized void pushDiscardingOld(T element) {
		if (!queue_.offer(element)) {
			queue_.remove();
			queue_.offer(element);
		}
		notifyAll();
	}

	public synchronized T pull() throws Nudged, InterruptedException {
		if (queue_.isEmpty()) {
			wait();
		}
		if (queue_.isEmpty()) {
			throw new Nudged();
		}
		return queue_.remove();
	}

	public synchronized void nudge() {
		notifyAll();
	}
}
