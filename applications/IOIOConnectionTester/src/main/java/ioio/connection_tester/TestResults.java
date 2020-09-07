package ioio.connection_tester;

import com.google.common.collect.SortedMultiset;
import com.google.common.collect.TreeMultiset;

public class TestResults {
	public class Throughput {
		public long bytes;
		public double time;
	}
	public class Latency {
		SortedMultiset<Double> latencies = TreeMultiset.create();
	}
	
	public Throughput uplink = new Throughput();
	public Throughput downlink = new Throughput();
	public Throughput bidi = new Throughput();
	public Latency light = new Latency();
	public Latency heavy = new Latency();
	public boolean dead = false;
}
