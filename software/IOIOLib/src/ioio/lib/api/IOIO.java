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

import ioio.lib.api.PulseInput.PulseMode;
import ioio.lib.api.TwiMaster.Rate;
import ioio.lib.api.Uart.Parity;
import ioio.lib.api.Uart.StopBits;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
import ioio.lib.api.exception.OutOfResourceException;

import java.io.Closeable;

/**
 * This interface provides control over all the IOIO board functions.
 * <p>
 * An instance of this interface is typically obtained by using the
 * {@link IOIOFactory} class. Initially, a connection should be established, by
 * calling {@link #waitForConnect()}. This method will block until the board is
 * connected an a connection has been established.
 * <p>
 * During the connection process, this library verifies that the IOIO firmware
 * is compatible with the required version. If not, {@link #waitForConnect()}
 * will throw a {@link IncompatibilityException}, putting the {@link IOIO}
 * instance in a "zombie" state: nothing could be done with it except calling
 * {@link #disconnect()}, or waiting for the physical connection to drop via
 * {@link #waitForDisconnect()}.
 * <p>
 * As soon as a connection is established, the IOIO can be used, typically, by
 * calling the openXXX() functions to obtain additional interfaces for
 * controlling specific function of the board.
 * <p>
 * Whenever a connection is lost as a result of physically disconnecting the
 * board or as a result of calling {@link #disconnect()}, this instance and all
 * the interfaces obtained from it become invalid, and will throw a
 * {@link ConnectionLostException} on every operation. Once the connection is
 * lost, those instances cannot be recycled, but rather it is required to create
 * new ones and wait for a connection again.
 * <p>
 * Initially all pins are tri-stated (floating), and all functions are disabled.
 * Whenever a connection is lost or dropped, the board will immediately return
 * to the this initial state.
 * <p>
 * Typical usage:
 * 
 * <pre>
 * IOIO ioio = IOIOFactory.create();
 * try {
 *   ioio.waitForConnect();
 *   DigitalOutput out = ioio.openDigitalOutput(10);
 *   out.write(true);
 *   ...
 * } catch (ConnectionLostException e) {
 * } catch (Exception e) {
 *   ioio.disconnect();
 * } finally {
 *   ioio.waitForDisconnect();
 * }
 * </pre>
 * 
 * @see IOIOFactory#create()
 */
public interface IOIO {
	/** An invalid pin number. */
	public static final int INVALID_PIN = -1;
	/** The pin number used to designate the on-board 'stat' LED. */
	public static final int LED_PIN = 0;

	/**
	 * A versioned component in the system.
	 * 
	 * @see IOIO#getImplVersion(VersionType)
	 */
	public enum VersionType {
		/** Hardware version. */
		HARDWARE_VER,
		/** Bootloader version. */
		BOOTLOADER_VER,
		/** Application layer firmware version. */
		APP_FIRMWARE_VER,
		/** IOIOLib version. */
		IOIOLIB_VER
	}

	/**
	 * A state of a IOIO instance.
	 */
	public enum State {
		/** Connection not yet established. */
		INIT,
		/** Connected. */
		CONNECTED,
		/** Connection established, incompatible firmware detected. */
		INCOMPATIBLE,
		/** Disconnected. Instance is useless. */
		DEAD
	}

	/**
	 * Establishes connection with the IOIO board.
	 * <p>
	 * This method is blocking until connection is established. This method can
	 * be aborted by calling {@link #disconnect()}. In this case, it will throw
	 * a {@link ConnectionLostException}.
	 * 
	 * @throws ConnectionLostException
	 *             An error occurred during connection or disconnect() has been
	 *             called during connection. The instance state is disconnected.
	 * @throws IncompatibilityException
	 *             An incompatible board firmware of hardware has been detected.
	 *             The instance state is disconnected.
	 * @see #disconnect()
	 * @see #waitForDisconnect()
	 */
	public void waitForConnect() throws ConnectionLostException,
			IncompatibilityException;

	/**
	 * Closes the connection to the board, or aborts a connection process
	 * started with waitForConnect().
	 * <p>
	 * Once this method is called, this IOIO instance and all the instances
	 * obtain from it become invalid and will throw an exception on every
	 * operation.
	 * <p>
	 * This method is asynchronous, i.e. it returns immediately, but it is not
	 * guaranteed that all connection-related resources has already been freed
	 * and can be reused upon return. In cases when this is important, client
	 * can call {@link #waitForDisconnect()}, which will block until all
	 * resources have been freed.
	 */
	public void disconnect();

	/**
	 * Blocks until IOIO has been disconnected and all connection-related
	 * resources have been freed, so that a new connection can be attempted.
	 * 
	 * @throws InterruptedException
	 *             When interrupt() has been called on this thread. This might
	 *             mean that an immediate attempt to create and connect a new
	 *             IOIO object might fail for resource contention.
	 * @see #disconnect()
	 * @see #waitForConnect()
	 */
	public void waitForDisconnect() throws InterruptedException;

	/**
	 * Gets the connections state.
	 * 
	 * @return The connection state.
	 */
	public State getState();

	/**
	 * Resets the entire state (returning to initial state), without dropping
	 * the connection.
	 * <p>
	 * It is equivalent to calling {@link Closeable#close()} on every interface
	 * obtained from this instance. A connection must have been established
	 * prior to calling this method, by invoking {@link #waitForConnect()}.
	 * 
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 * @see #hardReset()
	 */
	public void softReset() throws ConnectionLostException;

	/**
	 * Equivalent to disconnecting and reconnecting the board power supply.
	 * <p>
	 * The connection will be dropped and not reestablished. Full boot sequence
	 * will take place, so firmware upgrades can be performed. A connection must
	 * have been established prior to calling this method, by invoking
	 * {@link #waitForConnect()}.
	 * 
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 * @see #softReset()
	 */
	public void hardReset() throws ConnectionLostException;

	/**
	 * Query the implementation version of the system's components. The
	 * implementation version uniquely identifies a hardware revision or a
	 * software build. Returned version IDs are always 8-character long,
	 * according to the IOIO versioning system: first 4 characters are the
	 * version authority and last 4 characters are the revision.
	 * 
	 * @param v
	 *            The component whose version we query.
	 * @return An 8-character implementation version ID.
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 */
	public String getImplVersion(VersionType v) throws ConnectionLostException;

	/**
	 * Open a pin for digital input.
	 * <p>
	 * A digital input pin can be used to read logic-level signals. The pin will
	 * operate in this mode until close() is invoked on the returned interface.
	 * It is illegal to open a pin that has already been opened and has not been
	 * closed. A connection must have been established prior to calling this
	 * method, by invoking {@link #waitForConnect()}.
	 * 
	 * @param spec
	 *            Pin specification, consisting of the pin number, as labeled on
	 *            the board, and the mode, which determines whether the pin will
	 *            be floating, pull-up or pull-down. See
	 *            {@link DigitalInput.Spec.Mode} for more information.
	 * @return Interface of the assigned pin.
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 * @see DigitalInput
	 */
	public DigitalInput openDigitalInput(DigitalInput.Spec spec)
			throws ConnectionLostException;

	/**
	 * Shorthand for openDigitalInput(new DigitalInput.Spec(pin)).
	 * 
	 * @see #openDigitalInput(ioio.lib.api.DigitalInput.Spec)
	 */
	public DigitalInput openDigitalInput(int pin)
			throws ConnectionLostException;

	/**
	 * Shorthand for openDigitalInput(new DigitalInput.Spec(pin, mode)).
	 * 
	 * @see #openDigitalInput(ioio.lib.api.DigitalInput.Spec)
	 */
	public DigitalInput openDigitalInput(int pin, DigitalInput.Spec.Mode mode)
			throws ConnectionLostException;

	/**
	 * Open a pin for digital output.
	 * <p>
	 * A digital output pin can be used to generate logic-level signals. The pin
	 * will operate in this mode until close() is invoked on the returned
	 * interface. It is illegal to open a pin that has already been opened and
	 * has not been closed. A connection must have been established prior to
	 * calling this method, by invoking {@link #waitForConnect()}.
	 * 
	 * @param spec
	 *            Pin specification, consisting of the pin number, as labeled on
	 *            the board, and the mode, which determines whether the pin will
	 *            be normal or open-drain. See {@link DigitalOutput.Spec.Mode}
	 *            for more information.
	 * @param startValue
	 *            The initial logic level this pin will generate as soon at it
	 *            is open.
	 * @return Interface of the assigned pin.
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 * @see DigitalOutput
	 */
	public DigitalOutput openDigitalOutput(DigitalOutput.Spec spec,
			boolean startValue) throws ConnectionLostException;

	/**
	 * Shorthand for openDigitalOutput(new DigitalOutput.Spec(pin, mode),
	 * startValue).
	 * 
	 * @see #openDigitalOutput(ioio.lib.api.DigitalOutput.Spec, boolean)
	 */
	public DigitalOutput openDigitalOutput(int pin,
			DigitalOutput.Spec.Mode mode, boolean startValue)
			throws ConnectionLostException;

	/**
	 * Shorthand for openDigitalOutput(new DigitalOutput.Spec(pin), startValue).
	 * Pin mode will be "normal" (as opposed to "open-drain".
	 * 
	 * @see #openDigitalOutput(ioio.lib.api.DigitalOutput.Spec, boolean)
	 */
	public DigitalOutput openDigitalOutput(int pin, boolean startValue)
			throws ConnectionLostException;

	/**
	 * Shorthand for openDigitalOutput(new DigitalOutput.Spec(pin), false). Pin
	 * mode will be "normal" (as opposed to "open-drain".
	 * 
	 * @see #openDigitalOutput(ioio.lib.api.DigitalOutput.Spec, boolean)
	 */
	public DigitalOutput openDigitalOutput(int pin)
			throws ConnectionLostException;

	/**
	 * Open a pin for analog input.
	 * <p>
	 * An analog input pin can be used to measure voltage. Note that not every
	 * pin can be used as an analog input. See board documentation for the legal
	 * pins and permitted voltage range.
	 * <p>
	 * The pin will operate in this mode until close() is invoked on the
	 * returned interface. It is illegal to open a pin that has already been
	 * opened and has not been closed. A connection must have been established
	 * prior to calling this method, by invoking {@link #waitForConnect()}.
	 * 
	 * @param pin
	 *            Pin number, as labeled on the board.
	 * @return Interface of the assigned pin.
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 * @see AnalogInput
	 */
	public AnalogInput openAnalogInput(int pin) throws ConnectionLostException;

	/**
	 * Open a pin for PWM (Pulse-Width Modulation) output.
	 * <p>
	 * A PWM pin produces a logic-level PWM signal. These signals are typically
	 * used for simulating analog outputs for controlling the intensity of LEDs,
	 * the rotation speed of motors, etc. They are also frequently used for
	 * controlling hobby servo motors.
	 * <p>
	 * Note that not every pin can be used as PWM output. In addition, the total
	 * number of concurrent PWM modules in use is limited. See board
	 * documentation for the legal pins and limit on concurrent usage.
	 * <p>
	 * The pin will operate in this mode until close() is invoked on the
	 * returned interface. It is illegal to open a pin that has already been
	 * opened and has not been closed. A connection must have been established
	 * prior to calling this method, by invoking {@link #waitForConnect()}.
	 * 
	 * @param spec
	 *            Pin specification, consisting of the pin number, as labeled on
	 *            the board, and the mode, which determines whether the pin will
	 *            be normal or open-drain. See {@link DigitalOutput.Spec.Mode}
	 *            for more information.
	 * @param freqHz
	 *            PWM frequency, in Hertz.
	 * @return Interface of the assigned pin.
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 * @throws OutOfResourceException
	 *             This is a runtime exception, so it is not necessary to catch
	 *             it if the client guarantees that the total number of
	 *             concurrent PWM resources is not exceeded.
	 * @see PwmOutput
	 */
	public PwmOutput openPwmOutput(DigitalOutput.Spec spec, int freqHz)
			throws ConnectionLostException;

	/**
	 * Shorthand for openPwmOutput(new DigitalOutput.Spec(pin), freqHz).
	 * 
	 * @see #openPwmOutput(ioio.lib.api.DigitalOutput.Spec, int)
	 */
	public PwmOutput openPwmOutput(int pin, int freqHz)
			throws ConnectionLostException;

	/**
	 * Open a pin for pulse input.
	 * <p>
	 * The pulse input module is quite flexible. It enables several kinds of
	 * timing measurements on a digital signal: pulse width measurement
	 * (positive or negative pulse), and frequency of a periodic signal.
	 * <p>
	 * Note that not every pin can be used as pulse input. In addition, the
	 * total number of concurrent pulse input modules in use is limited. See
	 * board documentation for the legal pins and limit on concurrent usage.
	 * <p>
	 * The pin will operate in this mode until close() is invoked on the
	 * returned interface. It is illegal to open a pin that has already been
	 * opened and has not been closed. A connection must have been established
	 * prior to calling this method, by invoking {@link #waitForConnect()}.
	 * 
	 * @param spec
	 *            Pin specification, consisting of the pin number, as labeled on
	 *            the board, and the mode, which determines whether the pin will
	 *            be floating, pull-up or pull-down. See
	 *            {@link DigitalInput.Spec.Mode} for more information.
	 * @param rate
	 *            The clock rate to use for timing the signal. A faster clock
	 *            rate will result in better precision but will only be able to
	 *            measure narrow pulses / high frequencies.
	 * @param mode
	 *            The mode in which to operate. Determines whether the module
	 *            will measure pulse durations or frequency.
	 * @param doublePrecision
	 *            Whether to open a double-precision pulse input module. Double-
	 *            precision modules enable reading of much longer pulses and
	 *            lower frequencies with high accuracy than single precision
	 *            modules. However, their number is limited, so when possible,
	 *            and if the resources are all needed, use single-precision. For
	 *            more details on the exact spec of single- vs. double-
	 *            precision, see {@link PulseInput}.
	 * @return An instance of the {@link PulseInput}, which can be used to
	 *         obtain the data.
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 * @throws OutOfResourceException
	 *             This is a runtime exception, so it is not necessary to catch
	 *             it if the client guarantees that the total number of
	 *             concurrent PWM resources is not exceeded.
	 * @see PulseInput
	 */
	public PulseInput openPulseInput(DigitalInput.Spec spec,
			PulseInput.ClockRate rate, PulseInput.PulseMode mode,
			boolean doublePrecision) throws ConnectionLostException;

	/**
	 * Shorthand for openPulseInput(new DigitalInput.Spec(pin), rate, mode,
	 * true), i.e. opens a double-precision, 16MHz pulse input on the given pin
	 * with the given mode.
	 * 
	 * @see #openPulseInput(ioio.lib.api.DigitalInput.Spec,
	 *      ioio.lib.api.PulseInput.ClockRate,
	 *      ioio.lib.api.PulseInput.PulseMode, boolean)
	 */
	public PulseInput openPulseInput(int pin, PulseMode mode)
			throws ConnectionLostException;

	/**
	 * Open a UART module, enabling a bulk transfer of byte buffers.
	 * <p>
	 * UART is a very common hardware communication protocol, enabling full-
	 * duplex, asynchronous point-to-point data transfer. It typically serves
	 * for opening consoles or as a basis for higher-level protocols, such as
	 * MIDI RS-232, and RS-485.
	 * <p>
	 * Note that not every pin can be used for UART RX or TX. In addition, the
	 * total number of concurrent UART modules in use is limited. See board
	 * documentation for the legal pins and limit on concurrent usage.
	 * <p>
	 * The UART module will operate, and the pins will work in their respective
	 * modes until close() is invoked on the returned interface. It is illegal
	 * to use pins that have already been opened and has not been closed. A
	 * connection must have been established prior to calling this method, by
	 * invoking {@link #waitForConnect()}.
	 * 
	 * @param rx
	 *            Pin specification for the RX pin, consisting of the pin
	 *            number, as labeled on the board, and the mode, which
	 *            determines whether the pin will be floating, pull-up or
	 *            pull-down. See {@link DigitalInput.Spec.Mode} for more
	 *            information. null can be passed to designate that we do not
	 *            want RX input to this module.
	 * @param tx
	 *            Pin specification for the TX pin, consisting of the pin
	 *            number, as labeled on the board, and the mode, which
	 *            determines whether the pin will be normal or open-drain. See
	 *            {@link DigitalOutput.Spec.Mode} for more information. null can
	 *            be passed to designate that we do not want TX output to this
	 *            module.
	 * @param baud
	 *            The clock frequency of the UART module in Hz.
	 * @param parity
	 *            The parity mode, as in {@link Parity}.
	 * @param stopbits
	 *            Number of stop bits, as in {@link StopBits}.
	 * @return Interface of the assigned module.
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 * @throws OutOfResourceException
	 *             This is a runtime exception, so it is not necessary to catch
	 *             it if the client guarantees that the total number of
	 *             concurrent UART resources is not exceeded.
	 * @see Uart
	 */
	public Uart openUart(DigitalInput.Spec rx, DigitalOutput.Spec tx, int baud,
			Parity parity, StopBits stopbits) throws ConnectionLostException;

	/**
	 * Shorthand for
	 * {@link #openUart(DigitalInput.Spec, DigitalOutput.Spec, int, Uart.Parity, Uart.StopBits)}
	 * , where the input pins use their default specs. {@link #INVALID_PIN} can
	 * be used on either pin if a TX- or RX-only UART is needed.
	 * 
	 * @see #openUart(DigitalInput.Spec, DigitalOutput.Spec, int, Uart.Parity,
	 *      Uart.StopBits)
	 */
	public Uart openUart(int rx, int tx, int baud, Parity parity,
			StopBits stopbits) throws ConnectionLostException;

	/**
	 * Open a SPI master module, enabling communication with multiple
	 * SPI-enabled slave modules.
	 * <p>
	 * SPI is a common hardware communication protocol, enabling full-duplex,
	 * synchronous point-to-multi-point data transfer. It requires MOSI, MISO
	 * and CLK lines shared by all nodes, as well as a SS line per slave,
	 * connected between this slave and a respective pin on the master. The MISO
	 * line should operate in pull-up mode, using either the internal pull-up or
	 * an external resistor.
	 * <p>
	 * Note that not every pin can be used for SPI MISO, MOSI or CLK. In
	 * addition, the total number of concurrent SPI modules in use is limited.
	 * See board documentation for the legal pins and limit on concurrent usage.
	 * <p>
	 * The SPI module will operate, and the pins will work in their respective
	 * modes until close() is invoked on the returned interface. It is illegal
	 * to use pins that have already been opened and has not been closed. A
	 * connection must have been established prior to calling this method, by
	 * invoking {@link #waitForConnect()}.
	 * 
	 * @param miso
	 *            Pin specification for the MISO (Master In Slave Out) pin,
	 *            consisting of the pin number, as labeled on the board, and the
	 *            mode, which determines whether the pin will be floating,
	 *            pull-up or pull-down. See {@link DigitalInput.Spec.Mode} for
	 *            more information.
	 * @param mosi
	 *            Pin specification for the MOSI (Master Out Slave In) pin,
	 *            consisting of the pin number, as labeled on the board, and the
	 *            mode, which determines whether the pin will be normal or
	 *            open-drain. See {@link DigitalOutput.Spec.Mode} for more
	 *            information.
	 * @param clk
	 *            Pin specification for the CLK pin, consisting of the pin
	 *            number, as labeled on the board, and the mode, which
	 *            determines whether the pin will be normal or open-drain. See
	 *            {@link DigitalOutput.Spec.Mode} for more information.
	 * @param slaveSelect
	 *            An array of pin specifications for each of the slaves' SS
	 *            (Slave Select) pin. The index of this array designates the
	 *            slave index, used later to refer to this slave. The spec is
	 *            consisting of the pin number, as labeled on the board, and the
	 *            mode, which determines whether the pin will be normal or
	 *            open-drain. See {@link DigitalOutput.Spec.Mode} for more
	 *            information.
	 * @param config
	 *            The configuration of the SPI module. See
	 *            {@link SpiMaster.Config} for details.
	 * @return Interface of the assigned module.
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 * @throws OutOfResourceException
	 *             This is a runtime exception, so it is not necessary to catch
	 *             it if the client guarantees that the total number of
	 *             concurrent SPI resources is not exceeded.
	 * @see SpiMaster
	 */
	public SpiMaster openSpiMaster(DigitalInput.Spec miso,
			DigitalOutput.Spec mosi, DigitalOutput.Spec clk,
			DigitalOutput.Spec[] slaveSelect, SpiMaster.Config config)
			throws ConnectionLostException;

	/**
	 * Shorthand for
	 * {@link #openSpiMaster(ioio.lib.api.DigitalInput.Spec, ioio.lib.api.DigitalOutput.Spec, ioio.lib.api.DigitalOutput.Spec, ioio.lib.api.DigitalOutput.Spec[], ioio.lib.api.SpiMaster.Config)}
	 * , where the pins are all open with the default modes and default
	 * configuration values are used.
	 * 
	 * @see #openSpiMaster(ioio.lib.api.DigitalInput.Spec,
	 *      ioio.lib.api.DigitalOutput.Spec, ioio.lib.api.DigitalOutput.Spec,
	 *      ioio.lib.api.DigitalOutput.Spec[], ioio.lib.api.SpiMaster.Config)
	 */
	public SpiMaster openSpiMaster(int miso, int mosi, int clk,
			int[] slaveSelect, SpiMaster.Rate rate)
			throws ConnectionLostException;

	/**
	 * Shorthand for
	 * {@link #openSpiMaster(ioio.lib.api.DigitalInput.Spec, ioio.lib.api.DigitalOutput.Spec, ioio.lib.api.DigitalOutput.Spec, ioio.lib.api.DigitalOutput.Spec[], ioio.lib.api.SpiMaster.Config)}
	 * , where the MISO pins is opened with pull up, and the other pins are open
	 * with the default modes and default configuration values are used. In this
	 * version, a single slave is used.
	 * 
	 * @see #openSpiMaster(ioio.lib.api.DigitalInput.Spec,
	 *      ioio.lib.api.DigitalOutput.Spec, ioio.lib.api.DigitalOutput.Spec,
	 *      ioio.lib.api.DigitalOutput.Spec[], ioio.lib.api.SpiMaster.Config)
	 */
	public SpiMaster openSpiMaster(int miso, int mosi, int clk,
			int slaveSelect, SpiMaster.Rate rate)
			throws ConnectionLostException;

	/**
	 * Open a TWI (Two-Wire Interface, such as I2C/SMBus) master module,
	 * enabling communication with multiple TWI-enabled slave modules.
	 * <p>
	 * TWI is a common hardware communication protocol, enabling half-duplex,
	 * synchronous point-to-multi-point data transfer. It requires a physical
	 * connection of two lines (SDA, SCL) shared by all the bus nodes, where the
	 * SDA is open-drain and externally pulled-up.
	 * <p>
	 * Note that there is a fixed number of TWI modules, and the pins they use
	 * are static. Client has to make sure these pins are not already opened
	 * before calling this method. See board documentation for the number of
	 * modules and the respective pins they use.
	 * <p>
	 * The TWI module will operate, and the pins will work in their respective
	 * modes until close() is invoked on the returned interface. It is illegal
	 * to use pins that have already been opened and has not been closed. A
	 * connection must have been established prior to calling this method, by
	 * invoking {@link #waitForConnect()}.
	 * 
	 * @param twiNum
	 *            The TWI module index to use. Will also determine the pins
	 *            used.
	 * @param rate
	 *            The clock rate. Can be 100KHz / 400KHz / 1MHz.
	 * @param smbus
	 *            When true, will use SMBus voltage levels. When false, I2C
	 *            voltage levels.
	 * @return Interface of the assigned module.
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 * @see TwiMaster
	 */
	public TwiMaster openTwiMaster(int twiNum, Rate rate, boolean smbus)
			throws ConnectionLostException;

	/**
	 * Open an ICSP channel, enabling Flash programming of an external PIC MCU,
	 * and in particular, another IOIO board.
	 * <p>
	 * ICSP (In-Circuit Serial Programming) is a protocol intended for
	 * programming of PIC MCUs. It is a serial protocol over three wires: PGC
	 * (clock), PGD (data) and MCLR (reset), where PGC and MCLR are controlled
	 * by the master and PGD is shared by the master and slave, depending on the
	 * transaction state.
	 * <p>
	 * Note that there is only one ICSP modules, and the pins it uses are
	 * static. Client has to make sure that the ICSP module is not already in
	 * use, as well as those dedicated pins. See board documentation for the
	 * actual pins used for ICSP.
	 * 
	 * @return Interface of the ICSP module.
	 * @see IcspMaster
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 */
	public IcspMaster openIcspMaster() throws ConnectionLostException;
	
	/**
	 * Shorthand for openCapSense(pin, CapSense.DEFAULT_COEF).
	 * 
	 * @see #openCapSense(int, float)
	 */
	public CapSense openCapSense(int pin) throws ConnectionLostException;

	/**
	 * Open a pin for cap-sense.
	 * <p>
	 * A cap-sense input pin can be used to measure capacitance, typically in
	 * touch sensing applications. Note that not every pin can be used as cap-
	 * sense. See board documentation for the legal pins.
	 * <p>
	 * The pin will operate in this mode until close() is invoked on the
	 * returned interface. It is illegal to open a pin that has already been
	 * opened and has not been closed. A connection must have been established
	 * prior to calling this method, by invoking {@link #waitForConnect()}.
	 * 
	 * @param pin
	 *            Pin number, as labeled on the board.
	 * @return Interface of the assigned pin.
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 * @see CapSense
	 */
	public CapSense openCapSense(int pin, float filterCoef)
			throws ConnectionLostException;

	/**
	 * Start a batch of operations. This is strictly an optimization and will
	 * not change functionality: if the client knows that a sequence of several
	 * IOIO operations are going to be performed immediately following each
	 * other, a call to {@link #beginBatch()} before the sequence and
	 * {@link #endBatch()} after the sequence will cause the operations to be
	 * grouped into one transfer to the IOIO, thus reducing latency. A matching
	 * {@link #endBatch()} operation must always follow, or otherwise no
	 * operation will ever be actually executed. {@link #beginBatch()} /
	 * {@link #endBatch()} blocks may be nested - the transfer will occur when
	 * the outermost {@link #endBatch()} is invoked. Note that it is not
	 * guaranteed that no transfers will happen while inside a batch - it should
	 * be treated as a hint. Code running inside the block must be quick as it
	 * blocks <b>all</b> transfers to the IOIO, including those performed from
	 * other threads.
	 * 
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 */
	public void beginBatch() throws ConnectionLostException;

	/**
	 * End a batch of operations. For explanation, see {@link #beginBatch()}.
	 * 
	 * @throws ConnectionLostException
	 *             Connection was lost before or during the execution of this
	 *             method.
	 */
	public void endBatch() throws ConnectionLostException;
}
