/*
 * Copyright 2011. All rights reserved.
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
package ioio.api;

import ioio.api.exception.ConnectionLostException;
import ioio.api.exception.InvalidOperationException;
import ioio.api.exception.OperationAbortedException;
import ioio.api.exception.OutOfResourceException;
import ioio.lib.IOIOSpi;
import ioio.lib.IOIOTwi;

import java.net.SocketException;

/**
 * An interface for interacting with the IOIO board.
 *
 * Initially all pins should be tri-stated (floating).
 * Whenever a connection is lost or dropped, the board should immediately 
 * return to the initial state.
 *
 * Typical Usage:
 * <pre>
 * PeripheralController controller = PeripheralController.waitForController();
 * DigitalOutput out = controller.openDigitalOutput(10, false);
 * out.write(true);
 * </pre>
 *
 * @author arshan
 * @author birmiwal
 */
public interface IOIOLib {

    /**
	 * Establishes connection with a board.
	 *
	 * This method is blocking until connection is established.
	 * This method can be aborted by calling abortConnection();
	 *
	 * @throws OperationAbortedException if abortConnection() got called.
	 */
	public void waitForConnect() throws OperationAbortedException, SocketException;

	/**
	 * Closes a connection to the board, and returns it to the initial state.
	 *
	 * All objects obtained from this instance until now get invalidated,
	 * and will throw an exception on every operation.
	 */
	public void disconnect();

	/**
	 * Aborts the currently running waitForConnect() method.
	 */
	public void abortConnection();

	/**
	 * @return true if a connection is established.
	 */
	public boolean isConnected();

	/**
	 * Resets the entire state (returning to initial state), without dropping the connection.
	 *
	 * All objects obtained from this instance until now get invalidated,
	 * and will throw an exception on every operation.
	 *
	 * @throws ConnectionLostException in case connection was lost before running this method.
	 */
	public void softReset() throws ConnectionLostException;

	/**
	 * Equivalent to disconnecting and reconnecting board power.
	 *
	 * The connection will be dropped and not reestablished.
	 * Boot sequence will take place.
	 *
	 * @throws ConnectionLostException in case connection was lost before running this method.
	 */
	public void hardReset() throws ConnectionLostException;

	/**
	 * Assign a pin for digital input.
	 *
	 * See board documentation for a complete list of functions supported by each physical pin.
	 *
	 * @param pin The number of pin to assign as appears on the board.
	 * @return Object of the assigned pin.
	 * @throws ConnectionLostException in case connection was lost before running this method.
	 * @throws InvalidOperationException
	 */
	public DigitalInput openDigitalInput(int pin)
	throws ConnectionLostException, InvalidOperationException;

    public DigitalInput openDigitalInput(int pin, DigitalInputMode mode)
    throws ConnectionLostException, InvalidOperationException;

    /**
     * Assign a pin for digital output, and set its initial state to LOW.
     *
     * See board documentation for a complete list of functions supported by each physical pin.
     *
     * @param pin The number of pin to assign as appears on the board.
     * @param startValue the initial value of that pin.
     * @return Object of the assigned pin.
     * @throws ConnectionLostException in case connection was lost before running this method.
     * @throws InvalidOperationException
     */
    public DigitalOutput openDigitalOutput(int pin, boolean startValue)
    throws ConnectionLostException, InvalidOperationException;

    /**
     * Assign a pin for digital output.
     *
     * See board documentation for a complete list of functions supported by each physical pin.
     *
     * @param pin The number of pin to assign as appears on the board.
     * @param startValue the initial value of that pin.
     * @param mode mode for opening the output; can be used for setting in open-drain mode where an external pullup is required.
     * @return Object of the assigned pin.
     * @throws ConnectionLostException in case connection was lost before running this method.
     * @throws InvalidOperationException
     */
    public DigitalOutput openDigitalOutput(int pin, boolean startValue, DigitalOutputMode mode)
    throws ConnectionLostException, InvalidOperationException;

	/**
	 * Assign a pin for analog input.
	 *
	 * See board documentation for a complete list of functions supported by each physical pin.
	 *
	 * @param pin The number of pin to assign as appears on the board.
	 * @return Object of the assigned pin.
	 * @throws ConnectionLostException in case connection was lost before running this method.
	 * @throws InvalidOperationException
	 */
	public AnalogInput openAnalogInput(int pin)
	throws ConnectionLostException, InvalidOperationException;

	/**
	 * Assign a pin for PWM output.
	 *
	 * See board documentation for a complete list of functions supported by each physical pin.
	 * Note: Number of concurrent PWM outputs is limited, see board documentation for details.
	 *
	 * @param pin The number of pin to assign as appears on the board.
	 * @param enableOpenDrain true for opening pin in open drain mode (digital HIGH will put pin in tri-state).
	 * @param freqHz The PWM frequency in Hz.
	 * @return Object of the assigned pin.
	 * @throws OutOfResourceException in case maximum concurrent PWM outputs are already in use.
	 * @throws ConnectionLostException in case connection was lost before running this method.
	 * @throws InvalidOperationException
	 */
    public PwmOutput openPwmOutput(int pin, int freqHz)
    throws OutOfResourceException, ConnectionLostException, InvalidOperationException;

    public PwmOutput openPwmOutput(DigitalOutput pin, int freqHz)
    throws OutOfResourceException, ConnectionLostException, InvalidOperationException;

	
	/**
	 * Open a UART module, enabling a bulk transfer of byte buffers.
	 *
	 * See board documentation for a complete list of functions supported by each physical pin.
     * Note: Number of concurrent UART modules is limited, see board documentation for details.
	 *
	 * @param rx The number of pin to assign for receiving as appears on the board.
	 * @param tx The number of pin to assign for sending as appears on the board.
	 * @param baud The clock frequency of the UART module in Hz.
	 * @param parity The parity mode.
	 * @param stopbits Number of stop bits.
	 * @return Object of the assigned UART module.
	 * @throws ConnectionLostException in case connection was lost before running this method.
	 * @throws InvalidOperationException
	 */
	public Uart openUart(int rx, int tx, int baud, int parity, int stopbits)
    throws ConnectionLostException, InvalidOperationException;
    
	public Uart openUart(DigitalInput rx, DigitalOutput tx, int baud, int parity, int stopbits)
    throws ConnectionLostException, InvalidOperationException;
    
	
	/**
	 * Opens an spi channel using the indicated slave pin.
	 * 
	 * TODO(arshan): option to pass in an SpiMaster as well, so that one master can drive multiple devices
	 * 
	 * @param miso
	 * @param mosi
	 * @param clk
	 * @param select
	 * @param speed
	 * @return
	 */
	public IOIOSpi openSpi(
            int miso, 
            int mosi, 
            int clk, 
            int select, 
            int speed)
	throws ConnectionLostException, InvalidOperationException;
    
	public IOIOSpi openSpi(
            DigitalInput  miso, 
            DigitalOutput mosi, 
            DigitalOutput clk, 
            DigitalOutput select, 
            int speed)
	throws ConnectionLostException, InvalidOperationException;

	/**
	 * The pins for Twi are static. 
	 * 
	 * @param speed
	 * @return
	 * @throws ConnectionLostException
	 * @throws InvalidOperationException
	 */
	public IOIOTwi openTwi(int speed)
	throws ConnectionLostException, InvalidOperationException;
	
	
}