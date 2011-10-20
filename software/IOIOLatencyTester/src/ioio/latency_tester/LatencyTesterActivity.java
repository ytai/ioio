package ioio.latency_tester;

import ioio.lib.api.IOIOConnection;
import ioio.lib.api.IOIOFactory;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.IOIOConnectionMultiplexer;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Random;
import java.util.Vector;

import android.app.Activity;
import android.os.Bundle;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class LatencyTesterActivity extends Activity {
	private static final String TAG = "IOIO_LATENCY_TEST";

	private static final int MAX_PACKET_SIZE = 256;
	private static final int PACKETS_PER_TEST = 128;
	private static final int MAX_OUTSTANDING_PACKETS = 16;

	private TestThread testThread_;
	private DisplayThread displayThread_;
	private final static Random random = new Random();

	private class MillisecAggregate {
		private long count_ = 0;
		private long sum_ = 0;
		private long min_ = Long.MAX_VALUE;
		private long max_ = 0;

		private synchronized void clear() {
			count_ = 0;
			sum_ = 0;
			min_ = Long.MAX_VALUE;
			max_ = 0;
		}

		public synchronized void add(long l) {
			++count_;
			sum_ += l;
			if (l < min_) {
				min_ = l;
			}
			if (l > max_) {
				max_ = l;
			}

		}

		public String toString() {
			float avg = -1;
			float min = -1;
			float max = -1;
			synchronized (this) {
				if (count_ > 0) {
					avg = ((float) sum_) / count_ / 1000000;
					min = ((float) min_) / 1000000;
					max = ((float) max_) / 1000000;
				}
			}
			return String.format("avg: %.2f min: %.2f max:%.2f", avg, min, max);
		}
	}

	private class RateAggregate {
		private long count_ = 0;
		private double sum_ = 0;
		private double min_ = Double.MAX_VALUE;
		private double max_ = 0;

		private synchronized void clear() {
			count_ = 0;
			sum_ = 0;
			min_ = Float.MAX_VALUE;
			max_ = 0;
		}

		public synchronized void add(double d) {
			++count_;
			sum_ += d;
			if (d < min_) {
				min_ = d;
			}
			if (d > max_) {
				max_ = d;
			}

		}

		public String toString() {
			double avg = -1;
			double min = -1;
			double max = -1;
			synchronized (this) {
				if (count_ > 0) {
					avg = sum_ / count_;
					min = min_;
					max = max_;
				}
			}
			return String.format("avg: %.2f min: %.2f max:%.2f", avg, min, max);
		}
	}

	MillisecAggregate latency_ = new MillisecAggregate();
	RateAggregate throughput_ = new RateAggregate();

	private TextView latencyTextView_;
	private TextView throughputTextView_;

	protected class TestThread extends Thread {
		protected IOIOConnection connection_;
		private boolean abort_ = false;
		private boolean connected_ = true;

		@Override
		public final void run() {
			super.run();
			Looper.prepare();
			while (true) {
				try {
					synchronized (this) {
						if (abort_) {
							break;
						}
						connection_ = createMultiplexedConnection();
					}
					connection_.waitForConnect();
					connected_ = true;
					while (!abort_) {
						runTests();
					}
					connection_.disconnect();
				} catch (ConnectionLostException e) {
					if (abort_) {
						break;
					}
				} catch (InterruptedException e) {
					connection_.disconnect();
					break;
				} catch (Exception e) {
					Log.e(TAG, "Unexpected exception caught", e);
					connection_.disconnect();
					break;
				}
			}
		}

		protected void runTests() throws ConnectionLostException,
				InterruptedException {
			testLatency();
			testThroughput();
		}

		protected void testLatency() throws ConnectionLostException,
				InterruptedException {
			InputStream in = connection_.getInputStream();
			OutputStream out = connection_.getOutputStream();

			try {
				for (int i = 0; i < PACKETS_PER_TEST; ++i) {
					byte b = (byte) random.nextInt();
					long startTime = System.nanoTime();
					out.write(b);
					int br = in.read();
					long endTime = System.nanoTime();
					if (br == -1) {
						throw new ConnectionLostException();
					}
					// TODO: check that br == -b
					latency_.add(endTime - startTime);
				}
			} catch (IOException e) {
				throw new ConnectionLostException(e);
			}
		}

		protected void testThroughput() throws ConnectionLostException,
				InterruptedException {
			InputStream in = connection_.getInputStream();
			OutputStream out = connection_.getOutputStream();
			byte[] packet = new byte[MAX_PACKET_SIZE];
			random.nextBytes(packet);

			try {
				ReaderThread thread = new ReaderThread(in, MAX_PACKET_SIZE
						* PACKETS_PER_TEST);
				thread.start();
				long startTime = System.nanoTime();
				for (int i = 0; i < PACKETS_PER_TEST; ++i) {
					synchronized (thread) {
						while (i - thread.received() / MAX_PACKET_SIZE >= MAX_OUTSTANDING_PACKETS) {
							thread.wait();
						}
					}
					out.write(packet);
				}
				thread.join();
				long endTime = System.nanoTime();
				throughput_.add((double) (MAX_PACKET_SIZE * PACKETS_PER_TEST)
						/ (endTime - startTime) * 1e9 / 1024);

			} catch (IOException e) {
				throw new ConnectionLostException(e);
			}
		}

		public final void abort() {
			Log.v(TAG, "abort called");
			synchronized (this) {
				abort_ = true;
				if (connection_ != null) {
					connection_.disconnect();
				}
				if (connected_) {
					interrupt();
				}
			}
			try {
				Log.v(TAG, "abort joining");
				join();
				Log.v(TAG, "abort joined");
			} catch (InterruptedException e) {
			}
		}

		private class ReaderThread extends Thread {
			private final int count_;
			private InputStream in_;
			private byte[] buffer_ = new byte[1024];
			private int received_ = 0;

			public ReaderThread(InputStream in, int count) {
				in_ = in;
				count_ = count;
			}
			
			public int received() {
				return received_;
			}

			@Override
			public void run() {
				super.run();
				try {
					while (received_ < count_) {
						int r = in_.read(buffer_);
						if (r == -1) {
							break;
						}
						synchronized (this) {
							received_ += r;
							notifyAll();
						}
					}
				} catch (IOException e) {
				}
			}

		}

		private IOIOConnection createMultiplexedConnection() {
			Vector<IOIOConnection> connections = new Vector<IOIOConnection>();
			try {
				connections
						.add(IOIOFactory
								.createConnectionDynamically("ioio.lib.bluetooth.BluetoothIOIOConnection"));
			} catch (ClassNotFoundException e) {
				Log.w(TAG, "Bluetooth not supported");
			}
			try {
				connections.add(IOIOFactory.createConnectionDynamically(
						"ioio.lib.impl.SocketIOIOConnection", 4545));
			} catch (ClassNotFoundException e) {
			}

			IOIOConnectionMultiplexer multiplexer = new IOIOConnectionMultiplexer(
					connections.toArray(new IOIOConnection[connections.size()]));
			return multiplexer;
		}
	}

	protected class DisplayThread extends Thread {
		private boolean abort_ = false;

		@Override
		public void run() {
			super.run();
			while (!abort_) {
				updateDisplay();
				try {
					sleep(10);
				} catch (InterruptedException e) {
				}
			}
		}

		public void abort() {
			abort_ = true;
			try {
				join();
			} catch (InterruptedException e) {
			}
		}

	}

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		latencyTextView_ = (TextView) findViewById(R.id.latency);
		throughputTextView_ = (TextView) findViewById(R.id.throughput);
		((Button) findViewById(R.id.clear_button))
				.setOnClickListener(new OnClickListener() {
					@Override
					public void onClick(View v) {
						clearStats();
					}
				});
	}

	protected void clearStats() {
		latency_.clear();
		throughput_.clear();
	}

	public void updateDisplay() {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				latencyTextView_.setText(latency_.toString());
				throughputTextView_.setText(throughput_.toString());
			}
		});
	}

	@Override
	public void onResume() {
		super.onResume();
		testThread_ = new TestThread();
		testThread_.start();
		displayThread_ = new DisplayThread();
		displayThread_.start();
	}

	@Override
	public void onPause() {
		testThread_.abort();
		displayThread_.abort();
		super.onPause();
	}
}
