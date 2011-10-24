package ioio.lib.util;

import java.util.Collection;

public interface IOIOConnectionDiscovery {
	public static class IOIOConnectionSpec {
		public final String className;
		public final Object[] args;
		
		public IOIOConnectionSpec(String c, Object[] a) {
			className = c;
			args = a;
		}
	}
	
	public void getSpecs(Collection<IOIOConnectionSpec> result);
}
