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

/**
 * An interface for controlling an SPI module, in SPI bus-master mode, enabling
 * communication with multiple SPI-enabled slave modules.
 * <p>
 * SPI is a common hardware communication protocol, enabling full-duplex,
 * synchronous point-to-multi-point data transfer. It requires MOSI, MISO and
 * CLK lines shared by all nodes, as well as a SS line per slave, connected
 * between this slave and a respective pin on the master. The MISO line should
 * operate in pull-up mode, using either the internal pull-up or an external
 * resistor. SpiMaster instances are obtained by calling
 * {@link IOIO#openSpiMaster(DigitalInput.Spec, DigitalOutput.Spec, DigitalOutput.Spec, DigitalOutput.Spec[], SpiMaster.Config)}.
 * <p>
 * The SPI protocol is comprised of simultaneous sending and receiving of data
 * between the bus master and a single slave. By the very nature of this
 * protocol, the amount of bytes sent is equal to the amount of bytes received.
 * However, by padding the sent data with garbage data, and by ignoring the
 * leading bytes of the received data arbitrarily-sized packets can be sent and
 * received.
 * <p>
 * A very common practice for SPI-based slave devices (although not always the
 * case), is to have a fixed request and response length and a fixed lag between
 * them, based on the request type. For example, an SPI-based sensor may define
 * the the protocol for obtaining its measured value is by sending a 2-byte
 * message, whereas the returned 3-byte value has its first byte overlapping the
 * second value of the response, as illustrated below:
 * 
 * <pre>
 * Master: M1   M2   GG   GG
 * Slave:  GG   S1   S2   S3
 * </pre>
 * 
 * M1, M2: the master's request<br>
 * S1, S2, S3: the slave's response<br>
 * GG: garbage bytes used for padding.
 * <p>
 * The IOIO SPI interface supports such fixed length message protocols using a
 * single method, {@link #writeRead(int, byte[], int, int, byte[], int)}, which
 * gets the request data, and the lengths of the request, the response and the
 * total transaction bytes.
 * 
 * <p>
 * The instance is alive since its creation. If the connection with the IOIO
 * drops at any point, the instance transitions to a disconnected state, in
 * which every attempt to use it (except {@link #close()}) will throw a
 * {@link ConnectionLostException}. Whenever {@link #close()} is invoked the
 * instance may no longer be used. Any resources associated with it are freed
 * and can be reused.
 * <p>
 * Typical usage (single slave, as per the example above):
 * 
 * <pre>
 * {@code
 * // MISO, MOSI, CLK, SS on pins 3, 4, 5, 6, respectively.
 * SpiMaster spi = ioio.openSpiMaster(3, 4, 5, 6, SpiMaster.Rate.RATE_125K);
 * final byte[] request = new byte[]{ 0x23, 0x45 };
 * final byte[] response = new byte[3];
 * spi.writeRead(request, 2, 4, response, 3);
 * ...
 * spi.close();  // free SPI module and pins
 * }
 * </pre>
 * 
 * @see IOIO#openSpiMaster(DigitalInput.Spec, DigitalOutput.Spec,
 *      DigitalOutput.Spec, DigitalOutput.Spec[], SpiMaster.Config)
 */
public interface SpiMaster extends Closeable {
	/** Possible data rates for SPI, in Hz. */
	enum Rate {
		RATE_31K, RATE_35K, RATE_41K, RATE_50K, RATE_62K, RATE_83K, RATE_125K, RATE_142K, RATE_166K, RATE_200K, RATE_250K, RATE_333K, RATE_500K, RATE_571K, RATE_666K, RATE_800K, RATE_1M, RATE_1_3M, RATE_2M, RATE_2_2M, RATE_2_6M, RATE_3_2M, RATE_4M, RATE_5_3M, RATE_8M
	}

	/** An object that can be waited on for asynchronous calls. */
	public interface Result {
		/**
		 * Wait until the asynchronous call which returned this instance is
		 * complete.
		 * 
		 * @throws ConnectionLostException
		 *             Connection with the IOIO has been lost.
		 * @throws InterruptedException
		 *             This operation has been interrupted.
		 */
		public void waitReady() throws ConnectionLostException,
				InterruptedException;
	}

	/** SPI configuration structure. */
	static class Config {
		/** Data rate. */
		public Rate rate;
		/** Whether to invert clock polarity. */
		public boolean invertClk;
		/**
		 * Whether to do the input and output sampling on the trailing clock
		 * edge.
		 */
		public boolean sampleOnTrailing;

		/**
		 * Constructor.
		 * 
		 * @param rate
		 *            Data rate.
		 * @param invertClk
		 *            Whether to invert clock polarity.
		 * @param sampleOnTrailing
		 *            Whether to do the input and output sampling on the
		 *            trailing clock edge.
		 */
		public Config(Rate rate, boolean invertClk, boolean sampleOnTrailing) {
			this.rate = rate;
			this.invertClk = invertClk;
			this.sampleOnTrailing = sampleOnTrailing;
		}

		/**
		 * Constructor with common defaults. Equivalent to Config(rate, false,
		 * false)
		 * 
		 * @see SpiMaster.Config#Config(SpiMaster.Config.Rate, boolean, boolean)
		 */
		public Config(Rate rate) {
			this(rate, false, false);
		}
	}

	/**
	 * Perform a single SPI transaction which includes optional transmission and
	 * optional reception of data to a single slave. This is a blocking
	 * operation that can take a few milliseconds to a few tens of milliseconds.
	 * To abort this operation, client can interrupt the blocked thread. If
	 * readSize is 0, the call returns immediately.
	 * 
	 * @param slave
	 *            The slave index. It is determined by the index of its
	 *            slave-select pin, as per the array passed to
	 *            {@link IOIO#openSpiMaster(DigitalInput.Spec, DigitalOutput.Spec, DigitalOutput.Spec, DigitalOutput.Spec[], SpiMaster.Config)}
	 *            .
	 * @param writeData
	 *            A byte array of data to write. May be null if writeSize is 0.
	 * @param writeSize
	 *            Number of bytes to write. Valid values are 0 to totalSize.
	 * @param totalSize
	 *            Total transaction length, in bytes. Valid values are 1 to 64.
	 * @param readData
	 *            An array where the response is to be stored. May be null if
	 *            readSize is 0.
	 * @param readSize
	 *            The number of expected response bytes. Valid values are 0 to
	 *            totalSize.
	 * @throws ConnectionLostException
	 *             Connection to the IOIO has been lost.
	 * @throws InterruptedException
	 *             Calling thread has been interrupted.
	 */
	public void writeRead(int slave, byte[] writeData, int writeSize,
			int totalSize, byte[] readData, int readSize)
			throws ConnectionLostException, InterruptedException;

	/**
	 * Shorthand for {@link #writeRead(int, byte[], int, int, byte[], int)} for
	 * the single-slave case.
	 * 
	 * @see #writeRead(int, byte[], int, int, byte[], int)
	 */
	public void writeRead(byte[] writeData, int writeSize, int totalSize,
			byte[] readData, int readSize) throws ConnectionLostException,
			InterruptedException;

	/**
	 * The same as {@link #writeRead(int, byte[], int, int, byte[], int)}, but
	 * returns immediately and returns a {@link Result} object that can be
	 * waited on. If readSize is 0, the result object is ready immediately.
	 * 
	 * @see #writeRead(int, byte[], int, int, byte[], int)
	 */
	public Result writeReadAsync(int slave, byte[] writeData, int writeSize,
			int totalSize, byte[] readData, int readSize)
			throws ConnectionLostException;
}
