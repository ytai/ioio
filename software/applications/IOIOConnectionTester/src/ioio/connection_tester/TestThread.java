package ioio.connection_tester;

import ioio.connection_tester.TestResults.Latency;
import ioio.lib.api.IOIOConnection;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.IOIOConnectionFactory;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOConnectionManager.Thread;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

public class TestThread extends Thread {
	private static final String TAG = "TestThread";
	private static final int PACKET_SIZE = 2048;
	private static final int NUM_PACKETS = 100;

	protected IOIOConnection connection_;
	private boolean abort_ = false;
	private boolean connected_ = false;
	private final IOIOConnectionFactory connectionFactory_;
	private final TestResults results_;
	private final AsyncRunner runner_ = new AsyncRunner();
	private final byte[] packet = new byte[PACKET_SIZE];

	TestThread(IOIOConnectionFactory factory, TestResults results) {
		connectionFactory_ = factory;
		results_ = results;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see ioio.lib.util.IOIOConnectionThread#run()
	 */
	@Override
	public final void run() {
		try {
			runner_.start();
			while (!abort_) {
				try {
					synchronized (this) {
						if (abort_) {
							break;
						}
						connection_ = connectionFactory_.createConnection();
					}
				} catch (Exception e) {
					Log.e(TAG,
							"Failed to create IOIOConnection, aborting TestThread!");
					return;
				}
				try {
					connection_.waitForConnect();
					connected_ = true;
					while (true) {
						test();
					}
					// connection_.disconnect();
				} catch (ConnectionLostException e) {
				} catch (InterruptedException e) {
					connection_.disconnect();
				} catch (Exception e) {
					Log.e(TAG, "Unexpected exception caught", e);
					connection_.disconnect();
					break;
				} finally {
					synchronized (this) {
						connection_ = null;
					}
				}
			}
		} finally {
			runner_.interrupt();
			synchronized (results_) {
				results_.dead = true;
			}
			Log.d(TAG, "TestThread is exiting");
		}
	}

	private void test() throws InterruptedException, ConnectionLostException {
		try {
			InputStream in = connection_.getInputStream();
			OutputStream out = connection_.getOutputStream();

			testUpThroughput(in, out);
			testDownThroughput(in, out);
			testBidiThroughput(in, out);
			testLatency(in, out, true);
			testLatency(in, out, false);
		} catch (IOException e) {
			throw new ConnectionLostException();
		}
	}

	protected void testUpThroughput(InputStream in, OutputStream out)
			throws IOException {
		ByteBuffer buf = ByteBuffer.allocate(5);
		buf.order(ByteOrder.LITTLE_ENDIAN);
		buf.put((byte) 0x01);
		buf.putInt(PACKET_SIZE * NUM_PACKETS);
		out.write(buf.array());
		final long start = System.nanoTime();
		for (int i = 0; i < NUM_PACKETS; ++i) {
			out.write(packet);
		}
		in.read();
		final long time = System.nanoTime() - start;
		runner_.add(new Runnable() {
			@Override
			public void run() {
				synchronized (results_) {
					results_.uplink.bytes += PACKET_SIZE * NUM_PACKETS;
					results_.uplink.time += (double) time * 1e-9;
					results_.notifyAll();
				}
			}
		});
	}

	protected void testDownThroughput(InputStream in, OutputStream out)
			throws IOException {
		int size = PACKET_SIZE * NUM_PACKETS;
		ByteBuffer buf = ByteBuffer.allocate(5);
		buf.order(ByteOrder.LITTLE_ENDIAN);
		buf.put((byte) 0x02);
		buf.putInt(size);
		out.write(buf.array());
		final long start = System.nanoTime();
		while (size > 0) {
			size -= in.read(packet, 0, Math.min(size, PACKET_SIZE));
		}
		final long time = System.nanoTime() - start;
		runner_.add(new Runnable() {
			@Override
			public void run() {
				synchronized (results_) {
					results_.downlink.bytes += PACKET_SIZE * NUM_PACKETS;
					results_.downlink.time += (double) time * 1e-9;
					results_.notifyAll();
				}
			}
		});
	}

	protected void testBidiThroughput(final InputStream in, OutputStream out)
			throws IOException, InterruptedException {
		int size = PACKET_SIZE * NUM_PACKETS;
		ByteBuffer buf = ByteBuffer.allocate(5);
		buf.order(ByteOrder.LITTLE_ENDIAN);
		buf.put((byte) 0x03);
		buf.putInt(size);
		out.write(buf.array());
		
		final int[] writeBudget = new int[1];
		writeBudget[0] = 2048;
		java.lang.Thread reader = new java.lang.Thread() {
			@Override
			public void run() {
				int size = PACKET_SIZE * NUM_PACKETS;
				try {
					while (size > 0) {
						int num_read = in.read(packet, 0, Math.min(size, PACKET_SIZE));
						size -= num_read;
						synchronized (writeBudget) {
							writeBudget[0] += num_read;
							writeBudget.notifyAll();
						}
					}
				} catch (IOException e) {
				}
			}
		};
		reader.start();
		final long start = System.nanoTime();
		for (int i = 0; i < NUM_PACKETS; ++i) {
			synchronized (writeBudget) {
				while (writeBudget[0] < packet.length) {
					writeBudget.wait();
				}
				writeBudget[0] -= packet.length;
			}
			out.write(packet);
		}
		reader.join();
		final long time = System.nanoTime() - start;
		runner_.add(new Runnable() {
			@Override
			public void run() {
				synchronized (results_) {
					results_.bidi.bytes += PACKET_SIZE * NUM_PACKETS * 2;
					results_.bidi.time += (double) time * 1e-9;
					results_.notifyAll();
				}
			}
		});
	}

	protected void testLatency(final InputStream in, OutputStream out,
			boolean heavy) throws IOException, InterruptedException {
		final Latency result = heavy ? results_.heavy : results_.light;
		ByteBuffer buf = ByteBuffer.allocate(5);
		buf.order(ByteOrder.LITTLE_ENDIAN);
		buf.put((byte) 0x03);
		buf.putInt(NUM_PACKETS);
		out.write(buf.array());

		final Queue<Long> q = new ConcurrentLinkedQueue<Long>();

		java.lang.Thread reader = new java.lang.Thread() {
			@Override
			public void run() {
				try {
					for (int i = 0; i < NUM_PACKETS; ++i) {
						in.read();
						final long latency = System.nanoTime() - q.remove();
						runner_.add(new Runnable() {
							@Override
							public void run() {
								synchronized (results_) {
									result.latencies.add((double) latency * 1e-9);
								}
							}
						});
					}
				} catch (IOException e) {
				}
			}
		};

		reader.start();
		
		for (int i = 0; i < NUM_PACKETS; ++i) {
			q.add(System.nanoTime());
			out.write(i);
			if (!heavy) {
				sleep(20);
			}
		}
		
		reader.join();
		synchronized(results_) {
			results_.notifyAll();
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see ioio.lib.util.IOIOConnectionThread#abort()
	 */
	@Override
	public synchronized final void abort() {
		abort_ = true;
		if (connection_ != null) {
			connection_.disconnect();
		}
		if (connected_) {
			interrupt();
		}
	}
}
