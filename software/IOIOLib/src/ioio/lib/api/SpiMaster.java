/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *  
 * 
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */
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
