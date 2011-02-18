package ioio.lib;

import ioio.lib.IOIOException.ConnectionLostException;
import ioio.lib.IOIOException.InvalidOperationException;
import ioio.lib.IOIOException.OperationAbortedException;
import ioio.lib.IOIOException.OutOfResourceException;
import ioio.lib.pic.Uart;

/**
 * An interface for controlling the IOIO board.
 *
 * Initially all pins of the IOIO board are tri-stated (floating).
 * Whenever a connection is lost or dropped, IOIO immediately returns to the initial state.
 *
 * Typical usage: (Simplified, no exception handling)
 * IOIO ioio = IOIOImpl.getInstance();
 * ioio.waitForConnect();  // Blocking
 * // Work with ioio...
 * ioio.disconnect();
 *
 * @author arshan
 * @author birmiwal
 */
public interface IOIO {

	/**
	 * Establishes connection with a IOIO board.
	 *
	 * This method is blocking until connection is established.
	 * This method can be aborted by calling abortConnection();
	 *
	 * @throws OperationAbortedException if abortConnection() got called.
	 */
	public void waitForConnect() throws OperationAbortedException;

	/**
	 * Closes a connection to the IOIO board, and returns it to the initial state.
	 *
	 * All objects obtained from this IOIO instance until now get invalidated,
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
	 * Resets the entire IOIO state (returning to initial state), without dropping the connection.
	 *
	 * All objects obtained from this IOIO instance until now get invalidated,
	 * and will throw an exception on every operation.
	 *
	 * @throws ConnectionLostException in case connection was lost before running this method.
	 */
	public void softReset() throws ConnectionLostException;

	/**
	 * Equivalent to disconnecting and reconnecting IOIO's power.
	 *
	 * The connection will be dropped and not reestablished.
	 * IOIO's boot sequence will take place.
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
	public Input<Boolean> openDigitalInput(int pin)
	throws ConnectionLostException, InvalidOperationException;

    /**
     * Assign a pin for digital output, and set its initial state to LOW.
     *
     * See board documentation for a complete list of functions supported by each physical pin.
     *
     * @param pin The number of pin to assign as appears on the board.
     * @param enableOpenDrain true for opening pin in open drain mode (digital HIGH will put pin in tri-state).
     * @return Object of the assigned pin.
     * @throws ConnectionLostException in case connection was lost before running this method.
     * @throws InvalidOperationException
     */
    public Output<Boolean> openDigitalOutput(int pin, boolean enableOpenDrain)
    throws ConnectionLostException, InvalidOperationException;

    /**
     * Assign a pin for digital output.
     *
     * See board documentation for a complete list of functions supported by each physical pin.
     *
     * @param pin The number of pin to assign as appears on the board.
     * @param enableOpenDrain true for opening pin in open drain mode (digital HIGH will put pin in tri-state).
     * @param startValue the initial value of that pin.
     * @return Object of the assigned pin.
     * @throws ConnectionLostException in case connection was lost before running this method.
     * @throws InvalidOperationException
     */
    public Output<Boolean> openDigitalOutput(int pin, boolean enableOpenDrain, boolean startValue)
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
	public Input<Float> openAnalogInput(int pin)
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
	public PwmOutput openPwmOutput(int pin, boolean enableOpenDrain, int freqHz)
	throws OutOfResourceException, ConnectionLostException, InvalidOperationException;

	// TODO: test support for this
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
	public Uart openUart(int rx, int tx, int baud, int parity, float stopbits)
	throws ConnectionLostException, InvalidOperationException;
}
