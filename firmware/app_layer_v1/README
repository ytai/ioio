IOIO Application Layer Firmware
===============================

Introduction
------------
The IOIO application layer firmware provides the implementation of the IOIO
application layer protocol, which enables control of the IOIO's on-board
peripherals over a serial connection, which is established by the lower layers
of the IOIO firmware stack.

Code Organization
-----------------
The file main.c contains the main function and the main state machine, which
tries to establish an ADB channel with TCP port 4545 of the ADB device.
Whenever a connection is established the protocol module will be initialized and
incoming messages will be passed to it for handling. Whenever the connection
drops - the entire protocol and peripheral state will be reset immediately.

The protocol module (protocol.{h,c}) handles parsing of incoming protocol
messages, dispatching them to the appropriate function modules. It also handles
sending of outgoing protocol messages, providing convenience buffering of
outgoing data, so that all other modules do not need to worry about whether the
outgoing channel is currently busy, etc.

Then there are function-specific modules:
features.{h,c} has some generic functions (resets, pin modes)
adc.{h,c} implements the analog input functions.
pwm.{h,c} implements PWM output.
digital.{h,c} implements digital input and output.

The module logging.{h,c} facilitates convenience logging functions through UART1
at 38400 baud on pin 4. It is switched on using the ENABLE_LOGGING macro, which
should be off for production.

The module pins.{h,c} contains all the information on mapping pin numbers as
appear on the board to/from respective pin-related registers in the MCU.

The file board.h contains information on specific boards.

Interrupts and Priorities
-------------------------
Priority levels serve two purposes:
- Manage interrupt latency (higher priorities will have better worst-case
  latency).
- Serialize access to shared resources. Certain resources are shared between
  multiple interrupts as well as non-interrupt ("main") code. Each such resource
  is given a priority level from which it should be accessed. It should never be
  accessed from within a different priority level. The interrupt level can be
  explicitly raised when accessing the resource from a lower-priority interrupt
  or non-interrupt code. It can never be accessed from a higher priority
  interrupt.

Priority assignment:
0 - Main code
1 - writing to outgoing channel. Includes:
    + Main code raising itself to level 1 for synchronizing access to the
      outgoing stream.
    + ADC secondary interrupt.
    + Change notify interrupt (digital input).
    + Input capture (second interrupt).

2 - UART TX
2 - I2C

3 - SPI

4 - UART RX
4 - Setting a digital output pin level (can be avoided be making SetPinLat atomic).

5 - USB host

6 - Input capture pulse for changing polarity between leading and trailing edge.
6 - ADC for reading converted value from buffer before another one comes in.

7 - Sequencer timer.
