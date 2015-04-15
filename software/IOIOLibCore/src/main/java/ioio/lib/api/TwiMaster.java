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
 * An interface for controlling a TWI module, in TWI bus-master mode, enabling
 * communication with multiple TWI-enabled slave modules.
 * <p>
 * TWI (Two-Wire Interface) is a common hardware communication protocol,
 * enabling half-duplex, synchronous point-to-multi-point data transfer. It
 * requires a physical connection of two lines (SDA, SCL) shared by all the bus
 * nodes, where the SDA is open-drain and externally pulled-up. TwiMaster
 * instances are obtained by calling
 * {@link IOIO#openTwiMaster(int, ioio.lib.api.TwiMaster.Rate, boolean)}.
 * <p>
 * TWI is the generic name for the specific I2C and SMBus protocols, differing
 * mostly by the voltage levels they require. This module supports both.
 * <p>
 * A TWI transaction is comprised of optional sending of a data buffer from the
 * master to a single slave, followed by an optional reception of a data buffer
 * from that slave. Slaves are designated by addresses, which may be 7-bit
 * (common) or 10-bit (less common). TWI transactions may fail, as a result of
 * the slave not responding or as result of the slave NACK'ing the request. Such
 * a transaction is executed using the
 * {@link #writeRead(int, boolean, byte[], int, byte[], int)} method.
 * <p>
 * The instance is alive since its creation. If the connection with the IOIO
 * drops at any point, the instance transitions to a disconnected state, in
 * <p>
 * The instance is alive since its creation. If the connection with the IOIO
 * drops at any point, the instance transitions to a disconnected state, in
 * which every attempt to use it (except {@link #close()}) will throw a
 * {@link ConnectionLostException}. Whenever {@link #close()} is invoked the
 * instance may no longer be used. Any resources associated with it are freed
 * and can be reused.
 * <p>
 * Typical usage:
 * 
 * <pre>
 * {@code
 * // Uses the SDA1/SCL1 pins, I2C volatege levels at 100KHz. 
 * TwiMaster twi = ioio.openTwiMaster(1, TwiMaster.RATE_100KHz, false);
 * final byte[] request = new byte[]{ 0x23, 0x45 };
 * final byte[] response = new byte[3];
 * if (twi.writeRead(0x19, false, request, 2, response, 3)) {
 *   // response is valid
 *   ...
 * } else {
 *   // handle error
 * }
 * twi.close();  // free TWI module and pins
 * }</pre>
 * 
 * @see IOIO#openTwiMaster(int, ioio.lib.api.TwiMaster.Rate, boolean)
 */
public interface TwiMaster extends Closeable {
	enum Rate {
		RATE_100KHz, RATE_400KHz, RATE_1MHz
	}

	/** An object that can be waited on for asynchronous calls. */
	public interface Result {
		/**
		 * Wait until the asynchronous call which returned this instance is
		 * complete.
		 * 
		 * @return true if TWI transaction succeeded.
		 * 
		 * @throws ConnectionLostException
		 *             Connection with the IOIO has been lost.
		 * @throws InterruptedException
		 *             This operation has been interrupted.
		 */
		public boolean waitReady() throws ConnectionLostException,
				InterruptedException;
	}

	/**
	 * Perform a single TWI transaction which includes optional transmission and
	 * optional reception of data to a single slave. This is a blocking
	 * operation that can take a few milliseconds to a few tens of milliseconds.
	 * To abort this operation, client can interrupt the blocked thread.
	 * 
	 * @param address
	 *            The slave address, either 7-bit or 10-bit. Note that in some
	 *            TWI device documentation the documented addresses are actually
	 *            2x the address values used here, as they regard the trailing
	 *            0-bit as part of the address.
	 * @param tenBitAddr
	 *            Whether this is a 10-bit addressing mode.
	 * @param writeData
	 *            The request data.
	 * @param writeSize
	 *            The number of bytes to write. Valid value are 0-255.
	 * @param readData
	 *            The array where the response should be stored.
	 * @param readSize
	 *            The expected number of response bytes. Valid value are 0-255.
	 * @return Whether operation succeeded.
	 * @throws ConnectionLostException
	 *             Connection to the IOIO has been lost.
	 * @throws InterruptedException
	 *             Calling thread has been interrupted.
	 */
	public boolean writeRead(int address, boolean tenBitAddr, byte[] writeData,
			int writeSize, byte[] readData, int readSize)
			throws ConnectionLostException, InterruptedException;

	/**
	 * Asynchronous version of
	 * {@link #writeRead(int, boolean, byte[], int, byte[], int)}. Returns
	 * immediately and provides a {@link Result} object on which the client can
	 * wait for the result.
	 * 
	 * @see #writeRead(int, boolean, byte[], int, byte[], int)
	 */
	public Result writeReadAsync(int address, boolean tenBitAddr,
			byte[] writeData, int writeSize, byte[] readData, int readSize)
			throws ConnectionLostException;
}
