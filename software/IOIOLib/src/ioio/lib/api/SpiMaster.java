package ioio.lib.api;

import ioio.lib.api.exception.ConnectionLostException;

public interface SpiMaster {
	enum Rate {
	    RATE_31K,
	    RATE_35K,
	    RATE_41K,
	    RATE_50K,
	    RATE_62K,
	    RATE_83K,
	    RATE_125K,
	    RATE_142K,
	    RATE_166K,
	    RATE_200K,
	    RATE_250K,
	    RATE_333K,
	    RATE_500K,
	    RATE_571K,
	    RATE_666K,
	    RATE_800K,
	    RATE_1M,
	    RATE_1_3M,
	    RATE_2M,
	    RATE_2_2M,
	    RATE_2_6M,
	    RATE_3_2M,
	    RATE_4M,
	    RATE_5_3M,
	    RATE_8M
	}
	
	static class Config {
		public Rate rate;
		public boolean invertClk;
		public boolean sampleOnTrailing;
		
		public Config(Rate rate, boolean invertClk, boolean sampleOnTrailing) {
			this.rate = rate;
			this.invertClk = invertClk;
			this.sampleOnTrailing = sampleOnTrailing;
		}

		public Config(Rate rate) {
			this(rate, false, false);
		}
	}
	
	
	public void writeRead(int slave, byte[] writeData, int writeSize, int totalSize,
			byte[] readData, int readSize) throws ConnectionLostException, InterruptedException;
}
