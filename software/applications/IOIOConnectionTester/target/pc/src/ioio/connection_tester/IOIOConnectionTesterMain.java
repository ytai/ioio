package ioio.connection_tester;

import ioio.lib.spi.IOIOConnectionFactory;
import ioio.lib.util.IOIOConnectionManager;
import ioio.lib.util.IOIOConnectionManager.IOIOConnectionThreadProvider;
import ioio.lib.util.IOIOConnectionManager.Thread;
import ioio.lib.util.IOIOConnectionRegistry;

import java.util.Iterator;

import com.google.common.collect.SortedMultiset;

public class IOIOConnectionTesterMain implements IOIOConnectionThreadProvider {
	static {
		IOIOConnectionRegistry
				.addBootstraps(new String[] { "ioio.lib.pc.SerialPortIOIOConnectionBootstrap" });
	}

	public static void main(String[] args) {
		new IOIOConnectionTesterMain().go();
	}

	private void go() {
		IOIOConnectionManager manager = new IOIOConnectionManager(this);
		manager.start();
	}

	@Override
	public Thread createThreadFromFactory(IOIOConnectionFactory factory) {
		final TestResults results = new TestResults();
		addConnection(factory.getType(), factory.getExtra(), results);
		return new TestThread(factory, results);
	}

	protected static double getMedian(SortedMultiset<Double> set) {
		if (set.size() == 0)
			return Double.NaN;
		int pos = set.size() / 2;
		double result;
		Iterator<Double> iter = set.iterator();
		do {
			result = iter.next();
		} while (pos-- > 0);
		return result;
	}

	private void addConnection(String type, Object extra, final TestResults results) {
		new java.lang.Thread() {
			@Override
			public void run() {

				try {
					synchronized (results) {
						while (!results.dead) {
							results.wait();
							// process results
							final double upThroughput = results.uplink.bytes
									/ results.uplink.time / 1024.;
							final double downThroughput = results.downlink.bytes
									/ results.downlink.time / 1024.;
							final double bidiThroughput = results.bidi.bytes
									/ results.bidi.time / 1024.;
							final double lightMedian = getMedian(results.light.latencies);
							final double heavyMedian = getMedian(results.heavy.latencies);
							
							System.out.printf("%.2f[KB/s] %.2f[KB/s] %.2f[KB/s] %.2f[ms] %.2f[ms]\n", upThroughput,
									downThroughput, bidiThroughput, lightMedian * 1000, heavyMedian * 1000);
						}
					}
				} catch (InterruptedException e) {
				}
			}
		}.start();
	}
}
