If most cases, you will not need to modify PIXEL's firmware and thus do not need this reporsitory. Instead just refer to the source code of PIXEL's apps. 
If however you want to modify the fundamental behavior of PIXEL, then
see http://ledpixelart.com/developers.html for instructions.



Pixel's firmware is a modified version of the standard IOIO board app firmware which was developed by Ytai Ben-Tsvi specifically to interface with the LED matrix hardware used by Pixel.

Below are the IOIO pins used by Pixel.  for an explanation of IOIO Pins.

IOIO Pins used for LED Matrix: In this diagram the female connector is depicted.

      +-----+
 23   | o o |   24
      | o o |   22
 20   | o o |   21
      | o o |   19
 10   | o o |   7
      | o o |   11
 27   | o o |   25
 GND  | o o |   28
      +-----+
IOIO Pins used for Grove/Expansion Ports:

3.3V Grove Port Pins: 31 (taken by proximity sensor), 32 (open), 33 (open), 34 (open)

5V Tolerant Grove Port Pins: 3 (open), 4 (open), 5 (open), 6 (open)

Pulse Sensor Female Header 3.3V: 35 (open)

