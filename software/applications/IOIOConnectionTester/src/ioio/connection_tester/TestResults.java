package ioio.connection_tester;

import java.util.SortedSet;
import java.util.TreeSet;

public class TestResults {
	public class Throughput {
		public long bytes;
		public double time;
	}
	public class Latency {
		SortedSet<Double> latencies = new TreeSet<Double>();
	}
	
	public Throughput uplink = new Throughput();
	public Throughput downlink = new Throughput();
	public Throughput bidi = new Throughput();
	public Latency light = new Latency();
	public Latency heavy = new Latency();
}
