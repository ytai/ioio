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

import java.io.InputStream;
import java.io.OutputStream;

/**
 * An interface for controlling a UART module.
 * <p>
 * UART is a very common hardware communication protocol, enabling full- duplex,
 * asynchronous point-to-point data transfer. It typically serves for opening
 * consoles or as a basis for higher-level protocols, such as MIDI, RS-232 and
 * RS-485. Uart instances are obtained by calling
 * {@link IOIO#openUart(DigitalInput.Spec, DigitalOutput.Spec, int, Uart.Parity, Uart.StopBits)}.
 * <p>
 * The UART protocol is completely symmetric - there is no "master" and "slave"
 * at this layer. Each end may send any number of bytes at arbitrary times,
 * making it very useful for terminals and terminal-controllable devices.
 * <p>
 * Working with UART is very intuitive - it just provides a standard InputStream
 * and/or OutputStream. Optionally, one could create a read-only or write-only
 * UART instances, by passing null (or INVALID_PIN) for either TX or RX pins.
 * <p>
 * The instance is alive since its creation. If the connection with the IOIO
 * drops at any point, the instance transitions to a disconnected state, which
 * every attempt to use it (except {@link #close()}) will throw a
 * {@link ConnectionLostException}. Whenever {@link #close()} is invoked the
 * instance may no longer be used. Any resources associated with it are freed
 * and can be reused.
 * <p>
 * Typical usage:
 * 
 * <pre>
 * Uart uart = ioio.openUart(3, 4, 19200, Parity.NONE, StopBits.ONE);
 * InputStream in = uart.getInputStream();
 * OutputStream out = uart.getOutputStream();
 * out.write(new String("Hello").getBytes());
 * int i = in.read();  // blocking
 * ...
 * uart.close();  // free UART module and pins
 * </pre>
 * 
 * @see IOIO#openUart(DigitalInput.Spec, DigitalOutput.Spec, int, Uart.Parity,
 *      Uart.StopBits)
 */
public interface Uart extends Closeable {
	/** Parity-bit mode. */
	enum Parity {
		/** No parity. */
		NONE,
		/** Even parity. */
		EVEN,
		/** Odd parity. */
		ODD
	}

	/** Number of stop-bits. */
	enum StopBits {
		/** One stop bit. */
		ONE,
		/** Two stop bits. */
		TWO
	}

	/**
	 * Gets the input stream.
	 * 
	 * @return An input stream.
	 */
	public InputStream getInputStream();

	/**
	 * Gets the output stream.
	 * 
	 * @return An output stream.
	 */
	public OutputStream getOutputStream();
}
